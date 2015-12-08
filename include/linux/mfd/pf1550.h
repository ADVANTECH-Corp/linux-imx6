/*
 * pf1550.h - mfd head file for PF1550
 *
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Robin Gong <yibin.gong@freescale.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LINUX_MFD_PF1550_H
#define __LINUX_MFD_PF1550_H

#include <linux/i2c.h>

enum chips { PF1550 = 1, };

enum pf1550_pmic_reg {
	/* PMIC regulator part */
	PF1550_PMIC_REG_DEVICE_ID		= 0x00,
	PF1550_PMIC_REG_OTP_FLAVOR		= 0x01,
	PF1550_PMIC_REG_SILICON_REV		= 0x02,

	PF1550_PMIC_REG_INT_CATEGORY		= 0x06,
	PF1550_PMIC_REG_SW_INT_STAT0		= 0x08,
	PF1550_PMIC_REG_SW_INT_MASK0		= 0x09,
	PF1550_PMIC_REG_SW_INT_SENSE0		= 0x0A,
	PF1550_PMIC_REG_SW_INT_STAT1		= 0x0B,
	PF1550_PMIC_REG_SW_INT_MASK1		= 0x0C,
	PF1550_PMIC_REG_SW_INT_SENSE1		= 0x0D,
	PF1550_PMIC_REG_SW_INT_STAT2		= 0x0E,
	PF1550_PMIC_REG_SW_INT_MASK2		= 0x0F,
	PF1550_PMIC_REG_SW_INT_SENSE2		= 0x10,
	PF1550_PMIC_REG_LDO_INT_STAT0		= 0x18,
	PF1550_PMIC_REG_LDO_INT_MASK0		= 0x19,
	PF1550_PMIC_REG_LDO_INT_SENSE0		= 0x1A,
	PF1550_PMIC_REG_TEMP_INT_STAT0		= 0x20,
	PF1550_PMIC_REG_TEMP_INT_MASK0		= 0x21,
	PF1550_PMIC_REG_TEMP_INT_SENSE0		= 0x22,
	PF1550_PMIC_REG_ONKEY_INT_STAT0		= 0x24,
	PF1550_PMIC_REG_ONKEY_INT_MASK0		= 0x25,
	PF1550_PMIC_REG_ONKEY_INT_SENSE0	= 0x26,
	PF1550_PMIC_REG_MISC_INT_STAT0		= 0x28,
	PF1550_PMIC_REG_MISC_INT_MASK0		= 0x29,
	PF1550_PMIC_REG_MISC_INT_SENSE0		= 0x2A,

	PF1550_PMIC_REG_COINCELL_CONTROL	= 0x30,

	PF1550_PMIC_REG_SW1_VOLT		= 0x32,
	PF1550_PMIC_REG_SW1_STBY_VOLT		= 0x33,
	PF1550_PMIC_REG_SW1_SLP_VOLT		= 0x34,
	PF1550_PMIC_REG_SW1_CTRL		= 0x35,
	PF1550_PMIC_REG_SW1_CTRL1		= 0x36,
	PF1550_PMIC_REG_SW2_VOLT		= 0x38,
	PF1550_PMIC_REG_SW2_STBY_VOLT		= 0x39,
	PF1550_PMIC_REG_SW2_SLP_VOLT		= 0x3A,
	PF1550_PMIC_REG_SW2_CTRL		= 0x3B,
	PF1550_PMIC_REG_SW2_CTRL1		= 0x3C,
	PF1550_PMIC_REG_SW3_VOLT		= 0x3E,
	PF1550_PMIC_REG_SW3_STBY_VOLT		= 0x3F,
	PF1550_PMIC_REG_SW3_SLP_VOLT		= 0x40,
	PF1550_PMIC_REG_SW3_CTRL		= 0x41,
	PF1550_PMIC_REG_SW3_CTRL1		= 0x42,
	PF1550_PMIC_REG_VSNVS_CTRL		= 0x48,
	PF1550_PMIC_REG_VREFDDR_CTRL		= 0x4A,
	PF1550_PMIC_REG_LDO1_VOLT		= 0x4C,
	PF1550_PMIC_REG_LDO1_CTRL		= 0x4D,
	PF1550_PMIC_REG_LDO2_VOLT		= 0x4F,
	PF1550_PMIC_REG_LDO2_CTRL		= 0x50,
	PF1550_PMIC_REG_LDO3_VOLT		= 0x52,
	PF1550_PMIC_REG_LDO3_CTRL		= 0x53,
	PF1550_PMIC_REG_PWRCTRL0		= 0x58,
	PF1550_PMIC_REG_PWRCTRL1		= 0x59,
	PF1550_PMIC_REG_PWRCTRL2		= 0x5A,
	PF1550_PMIC_REG_PWRCTRL3		= 0x5B,
	PF1550_PMIC_REG_SW1_PWRDN_SEQ		= 0x5F,
	PF1550_PMIC_REG_SW2_PWRDN_SEQ		= 0x60,
	PF1550_PMIC_REG_SW3_PWRDN_SEQ		= 0x61,
	PF1550_PMIC_REG_LDO1_PWRDN_SEQ		= 0x62,
	PF1550_PMIC_REG_LDO2_PWRDN_SEQ		= 0x63,
	PF1550_PMIC_REG_LDO3_PWRDN_SEQ		= 0x64,
	PF1550_PMIC_REG_VREFDDR_PWRDN_SEQ	= 0x65,

	PF1550_PMIC_REG_STATE_INFO		= 0x67,
	PF1550_PMIC_REG_I2C_ADDR		= 0x68,
	PF1550_PMIC_REG_IO_DRV0			= 0x69,
	PF1550_PMIC_REG_IO_DRV1			= 0x6A,
	PF1550_PMIC_REG_RC_16MHZ		= 0x6B,
	PF1550_PMIC_REG_KEY			= 0x6F,

	/* charger part */
	PF1550_CHARG_REG_CHG_INT		= 0x80,
	PF1550_CHARG_REG_CHG_INT_MASK		= 0x82,
	PF1550_CHARG_REG_CHG_INT_OK		= 0x84,
	PF1550_CHARG_REG_VBUS_SNS		= 0x86,
	PF1550_CHARG_REG_CHG_SNS		= 0x87,
	PF1550_CHARG_REG_BATT_SNS		= 0x88,
	PF1550_CHARG_REG_CHG_OPER		= 0x89,
	PF1550_CHARG_REG_CHG_TMR		= 0x8A,
	PF1550_CHARG_REG_CHG_EOC_CNFG		= 0x8D,
	PF1550_CHARG_REG_CHG_CURR_CNFG		= 0x8E,
	PF1550_CHARG_REG_BATT_REG		= 0x8F,
	PF1550_CHARG_REG_BATFET_CNFG		= 0x91,
	PF1550_CHARG_REG_THM_REG_CNFG		= 0x92,
	PF1550_CHARG_REG_VBUS_INLIM_CNFG	= 0x94,
	PF1550_CHARG_REG_VBUS_LIN_DPM		= 0x95,
	PF1550_CHARG_REG_USB_PHY_LDO_CNFG	= 0x96,
	PF1550_CHARG_REG_DBNC_DELAY_TIME	= 0x98,
	PF1550_CHARG_REG_CHG_INT_CNFG		= 0x99,
	PF1550_CHARG_REG_THM_ADJ_SETTING	= 0x9A,
	PF1550_CHARG_REG_VBUS2SYS_CNFG		= 0x9B,
	PF1550_CHARG_REG_LED_PWM		= 0x9C,
	PF1550_CHARG_REG_FAULT_BATFET_CNFG	= 0x9D,
	PF1550_CHARG_REG_LED_CNFG		= 0x9E,
	PF1550_CHARG_REG_CHGR_KEY2		= 0x9F,

	PF1550_PMIC_REG_END			= 0xff,
};

#define PF1550_CHG_PRECHARGE		0
#define PF1550_CHG_CONSTANT_CURRENT	1
#define PF1550_CHG_CONSTANT_VOL		2
#define PF1550_CHG_EOC			3
#define PF1550_CHG_DONE			4
#define PF1550_CHG_TIMER_FAULT		6
#define PF1550_CHG_SUSPEND		7
#define PF1550_CHG_OFF_INV		8
#define PF1550_CHG_BAT_OVER		9
#define PF1550_CHG_OFF_TEMP		10
#define PF1550_CHG_LINEAR_ONLY		12
#define PF1550_CHG_SNS_MASK		0xf

#define PF1550_BAT_NO_VBUS		0
#define PF1550_BAT_LOW_THAN_PRECHARG	1
#define PF1550_BAT_CHARG_FAIL		2
#define PF1550_BAT_HIGH_THAN_PRECHARG	4
#define PF1550_BAT_OVER_VOL		5
#define	PF1550_BAT_NO_DETECT		6
#define PF1550_BAT_SNS_MASK		0x7

#define PF1550_VBUS_UVLO		BIT(2)
#define PF1550_VBUS_IN2SYS		BIT(3)
#define PF1550_VBUS_OVLO		BIT(4)
#define PF1550_VBUS_VALID		BIT(5)

#define PF1550_CHARG_REG_BATT_REG_CHGCV_MASK		0x3f
#define PF1550_CHARG_REG_BATT_REG_VMINSYS_SHIFT		6
#define PF1550_CHARG_REG_BATT_REG_VMINSYS_MASK		0x3
#define PF1550_CHARG_REG_THM_REG_CNFG_REGTEMP_SHIFT	2
#define PF1550_CHARG_REG_THM_REG_CNFG_REGTEMP_MASK	0x3

#define PMIC_IRQ_SW1_LS		BIT(0)
#define PMIC_IRQ_SW2_LS		BIT(1)
#define PMIC_IRQ_SW3_LS		BIT(2)
#define PMIC_IRQ_SW1_HS		BIT(0)
#define PMIC_IRQ_SW2_HS		BIT(1)
#define PMIC_IRQ_SW3_HS		BIT(2)
#define PMIC_IRQ_LDO1_FAULT	BIT(0)
#define PMIC_IRQ_LDO2_FAULT	BIT(1)
#define PMIC_IRQ_LDO3_FAULT	BIT(2)
#define PMIC_IRQ_TEMP_110	BIT(0)
#define PMIC_IRQ_TEMP_125	BIT(1)

#define ONKEY_IRQ_PUSHI		BIT(0)
#define ONKEY_IRQ_1SI		BIT(1)
#define ONKEY_IRQ_2SI		BIT(2)
#define ONKEY_IRQ_3SI		BIT(3)
#define ONKEY_IRQ_4SI		BIT(4)
#define ONKEY_IRQ_8SI		BIT(5)

#define CHARG_IRQ_BAT2SOCI	BIT(1)
#define CHARG_IRQ_BATI		BIT(2)
#define CHARG_IRQ_CHGI		BIT(3)
#define CHARG_IRQ_VBUSI		BIT(5)
#define CHARG_IRQ_DPMI		BIT(6)
#define CHARG_IRQ_THMI		BIT(7)

enum pf1550_irq {
	PF1550_PMIC_IRQ_SW1_LS,
	PF1550_PMIC_IRQ_SW2_LS,
	PF1550_PMIC_IRQ_SW3_LS,
	PF1550_PMIC_IRQ_SW1_HS,
	PF1550_PMIC_IRQ_SW2_HS,
	PF1550_PMIC_IRQ_SW3_HS,
	PF1550_PMIC_IRQ_LDO1_FAULT,
	PF1550_PMIC_IRQ_LDO2_FAULT,
	PF1550_PMIC_IRQ_LDO3_FAULT,
	PF1550_PMIC_IRQ_TEMP_110,
	PF1550_PMIC_IRQ_TEMP_125,

	PF1550_ONKEY_IRQ_PUSHI,
	PF1550_ONKEY_IRQ_1SI,
	PF1550_ONKEY_IRQ_2SI,
	PF1550_ONKEY_IRQ_3SI,
	PF1550_ONKEY_IRQ_4SI,
	PF1550_ONKEY_IRQ_8SI,

	PF1550_CHARG_IRQ_BAT2SOCI,
	PF1550_CHARG_IRQ_BATI,
	PF1550_CHARG_IRQ_CHGI,
	PF1550_CHARG_IRQ_VBUSI,
	PF1550_CHARG_IRQ_DPMI,
	PF1550_CHARG_IRQ_THMI,
};

enum pf1550_regulators {
	PF1550_SW1,
	PF1550_SW2,
	PF1550_SW3,
	PF1550_VREFDDR,
	PF1550_LDO1,
	PF1550_LDO2,
	PF1550_LDO3,
};

struct pf1550_irq_info {
	unsigned int irq;
	const char *name;
	unsigned int virq;
};

struct pf1550_dev {
	struct device *dev;
	struct i2c_client *i2c;
	int type;
	struct regmap *regmap;
	struct regmap_irq_chip_data *irq_data_regulator;
	struct regmap_irq_chip_data *irq_data_onkey;
	struct regmap_irq_chip_data *irq_data_charger;
	int irq;
};

#endif /* __LINUX_MFD_PF1550_H */
