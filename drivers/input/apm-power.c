/*
 *  Input Power Event -> APM Bridge
 *
 *  Copyright (c) 2007 Richard Purdie
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/apm-emulation.h>

#ifdef CONFIG_ARCH_ADVANTECH
#include <linux/proc-board.h>
#include <linux/timer.h>
long timer_diff,timer_1,timer_2;
char suspend_key_flag = 0;
#endif
static void system_power_event(unsigned int keycode)
{
	switch (keycode) {
	case KEY_SUSPEND:
#ifdef CONFIG_ARCH_ADVANTECH
		if (!suspend_key_flag) {
			apm_queue_event(APM_USER_SUSPEND);
			pr_info("Requesting system suspend...\n");
			suspend_key_flag = 1;
		}
#else
		apm_queue_event(APM_USER_SUSPEND);
		pr_info("Requesting system suspend...\n");
#endif
		break;
	default:
		break;
	}
}

static void apmpower_event(struct input_handle *handle, unsigned int type,
			   unsigned int code, int value)
{
	/* only react on key down events */
#ifdef CONFIG_ARCH_ADVANTECH
	if ( !IS_ROM_5420 && !IS_ROM_3420 ) {
		if (value != 1)
			return;
	}
#else
	if (value != 1)
		return;
#endif
	switch (type) {
#ifdef CONFIG_ARCH_ADVANTECH
	case EV_KEY:
                pr_info("EV_PWR:value[%d],type[%d],code[%d]\n",value,type,code);
                if (code == KEY_SUSPEND) {
                        if (value) 
                                timer_1 = jiffies;
                        else {
                                timer_2 = jiffies;
                                if (timer_1 > timer_2)
                                        timer_diff = timer_1 - timer_2;
                                else
                                        timer_diff = timer_2 - timer_1;
                                if (timer_diff < 100 ) {
                                        if (!suspend_key_flag) {
                                                system_power_event(code);
                                                suspend_key_flag = 1;
                                        }
                                }
                        }
                }
		break;
#endif	
	case EV_PWR:
		system_power_event(code);
		break;

	default:
		break;
	}
}

static int apmpower_connect(struct input_handler *handler,
					  struct input_dev *dev,
					  const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "apm-power";

	error = input_register_handle(handle);
	if (error) {
		pr_err("Failed to register input power handler, error %d\n",
		       error);
		kfree(handle);
		return error;
	}

	error = input_open_device(handle);
	if (error) {
		pr_err("Failed to open input power device, error %d\n", error);
		input_unregister_handle(handle);
		kfree(handle);
		return error;
	}

	return 0;
}

static void apmpower_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id apmpower_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT,
		.evbit = { BIT_MASK(EV_PWR) },
	},
#ifdef CONFIG_ARCH_ADVANTECH
	{
		.flags = INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { BIT_MASK(EV_KEY) },
	},
#endif
	{ },
};

MODULE_DEVICE_TABLE(input, apmpower_ids);

static struct input_handler apmpower_handler = {
	.event =	apmpower_event,
	.connect =	apmpower_connect,
	.disconnect =	apmpower_disconnect,
	.name =		"apm-power",
	.id_table =	apmpower_ids,
};

static int __init apmpower_init(void)
{
	return input_register_handler(&apmpower_handler);
}

static void __exit apmpower_exit(void)
{
	input_unregister_handler(&apmpower_handler);
}

module_init(apmpower_init);
module_exit(apmpower_exit);

MODULE_AUTHOR("Richard Purdie <rpurdie@rpsys.net>");
MODULE_DESCRIPTION("Input Power Event -> APM Bridge");
MODULE_LICENSE("GPL");
