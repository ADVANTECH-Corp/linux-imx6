/*
 * Copyright (C) 2017 NXP.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <linux/arm-smccc.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/watchdog.h>
#include <soc/imx/fsl_sip.h>

#define DEFAULT_TIMEOUT 10
/*
 * Software timer tick implemented in scfw side, support 10ms to 0xffffffff ms
 * in theory, but for normal case, 1s~60s is enough, you can change this max
 * value in case it's not enough.
 */
#define MAX_TIMEOUT 60

static struct watchdog_device imx8_wdd;

static int imx8_wdt_ping(struct watchdog_device *wdog)
{
	struct arm_smccc_res res;

	arm_smccc_smc(FSL_SIP_SRTC, FSL_SIP_SRTC_PING_WDOG, 0, 0, 0, 0, 0, 0,
			&res);

	return res.a0;
}

static int imx8_wdt_start(struct watchdog_device *wdog)
{
	struct arm_smccc_res res;

	/* no block */
	arm_smccc_smc(FSL_SIP_SRTC, FSL_SIP_SRTC_START_WDOG, 0, 0, 0, 0, 0, 0,
			&res);
	if (res.a0)
		return res.a0;
	/* TODO: change to SC_TIMER_WDOG_ACTION_PARTITION after SCFW support */
	arm_smccc_smc(FSL_SIP_SRTC, FSL_SIP_SRTC_SET_WDOG_ACT,
			SC_TIMER_WDOG_ACTION_BOARD, 0, 0, 0, 0, 0, &res);
	return res.a0;
}

static int imx8_wdt_stop(struct watchdog_device *wdog)
{
	struct arm_smccc_res res;

	arm_smccc_smc(FSL_SIP_SRTC, FSL_SIP_SRTC_STOP_WDOG, 0, 0, 0, 0, 0, 0,
			&res);

	return res.a0;
}

static int imx8_wdt_set_timeout(struct watchdog_device *wdog,
				unsigned int timeout)
{
	struct arm_smccc_res res;

	arm_smccc_smc(FSL_SIP_SRTC, FSL_SIP_SRTC_SET_TIMEOUT_WDOG,
			timeout * 1000, 0, 0, 0, 0, 0, &res);

	return res.a0;
}

static const struct watchdog_ops imx8_wdt_ops = {
	.owner = THIS_MODULE,
	.start = imx8_wdt_start,
	.stop  = imx8_wdt_stop,
	.ping  = imx8_wdt_ping,
	.set_timeout = imx8_wdt_set_timeout,
};

static const struct watchdog_info imx8_wdt_info = {
	.identity	= "i.MX8 watchdog timer",
	.options	= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static int imx8_wdt_probe(struct platform_device *pdev)
{
	struct watchdog_device *wdt = &imx8_wdd;
	int err;

	platform_set_drvdata(pdev, wdt);
	/* init the wdd */
	wdt->info = &imx8_wdt_info;
	wdt->ops = &imx8_wdt_ops;
	wdt->min_timeout = 1;
	wdt->max_timeout = MAX_TIMEOUT;
	wdt->parent = &pdev->dev;
	watchdog_set_drvdata(wdt, NULL);

	err = watchdog_init_timeout(wdt, DEFAULT_TIMEOUT, &pdev->dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init the wdog timeout:%d\n",
				err);
		return err;
	}

	err = watchdog_register_device(wdt);
	if (err) {
		dev_err(&pdev->dev, "Failed to register watchdog device\n");
		return err;
	}

	return 0;
}

static int imx8_wdt_remove(struct platform_device *pdev)
{
	struct watchdog_device *wdt = platform_get_drvdata(pdev);

	imx8_wdt_stop(wdt);

	watchdog_unregister_device(wdt);

	return 0;
}

static void imx8_wdt_shutdown(struct platform_device *pdev)
{
	struct watchdog_device *wdt = platform_get_drvdata(pdev);

	if (watchdog_active(wdt))
		imx8_wdt_stop(wdt);
}

static const struct of_device_id imx8_wdt_dt_ids[] = {
	{ .compatible = "fsl,imx8-wdt", },
	{ /*sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx8_wdt_dt_ids);

static struct platform_driver imx8_wdt_driver = {
	.probe		= imx8_wdt_probe,
	.remove		= imx8_wdt_remove,
	.shutdown	= imx8_wdt_shutdown,
	.driver		= {
		.name	= "imx8-wdt",
		.of_match_table = imx8_wdt_dt_ids,
	},
};

module_platform_driver(imx8_wdt_driver);

MODULE_AUTHOR("Robin Gong <yibin.gong@nxp.com>");
MODULE_DESCRIPTION("NXP i.MX8 watchdog driver");
MODULE_LICENSE("GPL v2");
