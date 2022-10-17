/* da9063-onkey.c - Onkey device driver for DA9063
 * Copyright (C) 2013  Dialog Semiconductor Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/regmap.h>

#include <linux/mfd/da9063/core.h>
#include <linux/mfd/da9063/pdata.h>
#include <linux/mfd/da9063/registers.h>

struct da9063_onkey {
	struct	da9063 *hw;
	struct delayed_work work;
	struct	input_dev *input;
	int irq;
	bool key_power;
};

static void da9063_poll_on(struct work_struct *work)
{
	struct da9063_onkey *onkey = container_of(work, struct da9063_onkey,
						  work.work);
	unsigned int val;
	bool poll = true;
	int ret;

	/* poll to see when the pin is released */
	ret = regmap_read(onkey->hw->regmap, DA9063_REG_STATUS_A, &val);
	if (ret < 0) {
		dev_err(&onkey->input->dev,
			"Failed to read ON status: %d\n", ret);
		goto err_poll;
	}

	if (!(val & DA9063_NONKEY)) {
		ret = regmap_update_bits(onkey->hw->regmap,
					 DA9063_REG_CONTROL_B,
					 DA9063_NONKEY_LOCK, 0);
		if (ret < 0) {
			dev_err(&onkey->input->dev,
				"Failed to reset the Key Delay %d\n", ret);
			goto err_poll;
		}

		/* unmask the onkey interrupt again */
		ret = regmap_update_bits(onkey->hw->regmap,
					 DA9063_REG_IRQ_MASK_A,
					 DA9063_NONKEY, 0);
		if (ret < 0) {
			dev_err(&onkey->input->dev,
				"Failed to unmask the onkey IRQ: %d\n", ret);
			goto err_poll;
		}

		input_report_key(onkey->input, KEY_POWER, 0);
		input_sync(onkey->input);

		poll = false;
	}

err_poll:
	if (poll)
		schedule_delayed_work(&onkey->work, 50);
}

static irqreturn_t da9063_onkey_irq_handler(int irq, void *data)
{
	struct da9063_onkey *onkey = data;
	unsigned int val;
	int ret;

	ret = regmap_read(onkey->hw->regmap, DA9063_REG_STATUS_A, &val);
	if (onkey->key_power && (ret >= 0) && (val & DA9063_NONKEY)) {
		ret = regmap_update_bits(onkey->hw->regmap,
					 DA9063_REG_IRQ_MASK_A,
					 DA9063_NONKEY, 1);
		if (ret < 0)
			dev_err(&onkey->input->dev,
				"Failed to mask the onkey IRQ: %d\n", ret);

		input_report_key(onkey->input, KEY_POWER, 1);
		input_sync(onkey->input);

		schedule_delayed_work(&onkey->work, 0);
		dev_dbg(&onkey->input->dev, "KEY_POWER pressed.\n");
	} else {
		input_report_key(onkey->input, KEY_SLEEP, 1);
		input_sync(onkey->input);
		input_report_key(onkey->input, KEY_SLEEP, 0);
		input_sync(onkey->input);
		dev_dbg(&onkey->input->dev, "KEY_SLEEP pressed.\n");
	}

	return IRQ_HANDLED;
}

static int da9063_onkey_probe(struct platform_device *pdev)
{
	struct da9063 *da9063 = dev_get_drvdata(pdev->dev.parent);
	struct da9063_pdata *pdata = dev_get_platdata(da9063->dev);
	struct da9063_onkey *onkey;
	bool kp_tmp = true;
	int ret = 0;

	if (pdata)
		kp_tmp = pdata->key_power;

	onkey = devm_kzalloc(&pdev->dev, sizeof(struct da9063_onkey),
			     GFP_KERNEL);
	if (!onkey) {
		dev_err(&pdev->dev, "Failed to allocate memory.\n");
		ret = -ENOMEM;
		goto err;
	}

	INIT_DELAYED_WORK(&onkey->work, da9063_poll_on);

	onkey->input = devm_input_allocate_device(&pdev->dev);
	if (!onkey->input) {
		dev_err(&pdev->dev, "Failed to allocated input device.\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = platform_get_irq_byname(pdev, "ONKEY");
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to get platform IRQ.\n");
		goto err;
	}
	onkey->irq = ret;

	ret = request_threaded_irq(onkey->irq, NULL,
				   da9063_onkey_irq_handler,
				   IRQF_TRIGGER_LOW | IRQF_ONESHOT,
				   "ONKEY", onkey);
	if (ret) {
		dev_err(&pdev->dev,
			"Failed to request input device IRQ.\n");
		goto err;
	}

	onkey->hw = da9063;
	onkey->key_power = kp_tmp;
	onkey->input->evbit[0] = BIT_MASK(EV_KEY);
	onkey->input->name = DA9063_DRVNAME_ONKEY;
	onkey->input->phys = DA9063_DRVNAME_ONKEY "/input0";
	onkey->input->dev.parent = &pdev->dev;

	if (onkey->key_power)
		input_set_capability(onkey->input, EV_KEY, KEY_POWER);
	input_set_capability(onkey->input, EV_KEY, KEY_SLEEP);

	ret = input_register_device(onkey->input);
	if (ret) {
		dev_err(&pdev->dev,
			"Failed to register input device.\n");
		goto err_irq;
	}

	platform_set_drvdata(pdev, onkey);
	return 0;

err_irq:
	free_irq(onkey->irq, onkey);
	cancel_delayed_work_sync(&onkey->work);
err:
	return ret;
}

static int da9063_onkey_remove(struct platform_device *pdev)
{
	struct	da9063_onkey *onkey = platform_get_drvdata(pdev);
	free_irq(onkey->irq, onkey);
	cancel_delayed_work_sync(&onkey->work);
	input_unregister_device(onkey->input);
	return 0;
}

static struct platform_driver da9063_onkey_driver = {
	.probe	= da9063_onkey_probe,
	.remove	= da9063_onkey_remove,
	.driver	= {
		.name	= DA9063_DRVNAME_ONKEY,
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(da9063_onkey_driver);

MODULE_AUTHOR("S Twiss <stwiss.opensource@diasemi.com>");
MODULE_DESCRIPTION("Onkey device driver for Dialog DA9063");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DA9063_DRVNAME_ONKEY);

