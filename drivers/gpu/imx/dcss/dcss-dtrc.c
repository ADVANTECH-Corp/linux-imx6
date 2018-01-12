/*
 * Copyright (C) 2017 NXP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <linux/device.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <drm/drm_fourcc.h>
#include <linux/delay.h>

#include <video/imx-dcss.h>
#include "dcss-prv.h"

#define USE_CTXLD

#define DTRC_F0_OFS			0x00
#define DTRC_F1_OFS			0x60

#define DCSS_DTRC_DYDSADDR			0x00
#define DCSS_DTRC_DCDSADDR			0x04
#define DCSS_DTRC_DYTSADDR			0x08
#define DCSS_DTRC_DCTSADDR			0x0C
#define DCSS_DTRC_SIZE				0x10
#define   FRAME_WIDTH_POS			0
#define   FRAME_WIDTH_MASK			GENMASK(9, 0)
#define   FRAME_HEIGHT_POS			16
#define   FRAME_HEIGHT_MASK			GENMASK(25, 16)
#define DCSS_DTRC_SYSSA				0x14
#define DCSS_DTRC_SYSEA				0x18
#define DCSS_DTRC_SUVSSA			0x1C
#define DCSS_DTRC_SUVSEA			0x20
#define DCSS_DTRC_CROPORIG			0x24
#define DCSS_DTRC_CROPSIZE			0x28
#define DCSS_DTRC_DCTL				0x2C
#define   CROPPING_EN				BIT(18)
#define   COMPRESSION_DIS			BIT(17)
#define   PIX_DEPTH_8BIT_EN			BIT(1)
#define   CONFIG_READY				BIT(0)
#define DCSS_DTRC_DYDSADDR_EXT			0x30
#define DCSS_DTRC_DCDSADDR_EXT			0x34
#define DCSS_DTRC_DYTSADDR_EXT			0x38
#define DCSS_DTRC_DCTSADDR_EXT			0x3C
#define DCSS_DTRC_SYSSA_EXT			0x40
#define DCSS_DTRC_SYSEA_EXT			0x44
#define DCSS_DTRC_SUVSSA_EXT			0x48
#define DCSS_DTRC_SUVSEA_EXT			0x4C

#define DCSS_DTRC_INTEN				0xC0
#define DCSS_DTRC_FDINTR			0xC4
#define DCSS_DTRC_DTCTRL			0xC8
#define   CURRENT_FRAME				BIT(31)
#define   ADDRESS_ID_ENABLE			BIT(30)
#define   ENDIANNESS_10BIT			BIT(29)
#define   MERGE_ARID_ENABLE			BIT(28)
#define   NON_G1_2_SWAP_MODE_POS		24
#define   NON_G1_2_SWAP_MODE_MASK		GENMASK(27, 24)
#define   TABLE_DATA_SWAP_POS			20
#define   TABLE_DATA_SWAP_MASK			GENMASK(23, 20)
#define   TILED_SWAP_POS			16
#define   TILED_SWAP_MASK			GENMASK(19, 16)
#define   RASTER_SWAP_POS			12
#define   RASTER_SWAP_MASK			GENMASK(15, 12)
#define   BURST_LENGTH_POS			4
#define   BURST_LENGTH_MASK			GENMASK(11, 4)
#define   G1_TILED_DATA_EN			BIT(3)
#define   HOT_RESET				BIT(2)
#define   ARIDR_MODE_DETILE			0
#define   ARIDR_MODE_BYPASS			2
#define DCSS_DTRC_ARIDR				0xCC
#define DCSS_DTRC_DTID2DDR			0xD0
#define DCSS_DTRC_CONFIG			0xD4
#define DCSS_DTRC_VER				0xD8
#define DCSS_DTRC_PFCTRL			0xF0
#define DCSS_DTRC_PFCR				0xF4
#define DCSS_DTRC_TOCR				0xF8

struct dcss_dtrc_ch {
	void __iomem *base_reg;
	u32 base_ofs;

	u32 xres;
	u32 yres;
	u32 pix_format;
	u64 format_modifier;
	u32 y_dec_ofs;
	u32 uv_dec_ofs;

	int curr_frame;

	u32 dctl;

	u32 ctx_id;

	bool bypass;
	bool running;
};

struct dcss_dtrc_priv {
	struct dcss_soc *dcss;
	void __iomem *dtrc_reg;

	struct dcss_dtrc_ch ch[2];
};

#ifdef CONFIG_DEBUG_FS
static struct dcss_debug_reg dtrc_frame_debug_reg[] = {
	DCSS_DBG_REG(DCSS_DTRC_DYDSADDR),
	DCSS_DBG_REG(DCSS_DTRC_DCDSADDR),
	DCSS_DBG_REG(DCSS_DTRC_DYTSADDR),
	DCSS_DBG_REG(DCSS_DTRC_DCTSADDR),
	DCSS_DBG_REG(DCSS_DTRC_SIZE),
	DCSS_DBG_REG(DCSS_DTRC_SYSSA),
	DCSS_DBG_REG(DCSS_DTRC_SYSEA),
	DCSS_DBG_REG(DCSS_DTRC_SUVSSA),
	DCSS_DBG_REG(DCSS_DTRC_SUVSEA),
	DCSS_DBG_REG(DCSS_DTRC_CROPORIG),
	DCSS_DBG_REG(DCSS_DTRC_CROPSIZE),
	DCSS_DBG_REG(DCSS_DTRC_DCTL),
	DCSS_DBG_REG(DCSS_DTRC_DYDSADDR_EXT),
	DCSS_DBG_REG(DCSS_DTRC_DCDSADDR_EXT),
	DCSS_DBG_REG(DCSS_DTRC_DYTSADDR_EXT),
	DCSS_DBG_REG(DCSS_DTRC_DCTSADDR_EXT),
	DCSS_DBG_REG(DCSS_DTRC_SYSSA_EXT),
	DCSS_DBG_REG(DCSS_DTRC_SYSEA_EXT),
	DCSS_DBG_REG(DCSS_DTRC_SUVSSA_EXT),
	DCSS_DBG_REG(DCSS_DTRC_SUVSEA_EXT),
};

static struct dcss_debug_reg dtrc_ctrl_debug_reg[] = {
	DCSS_DBG_REG(DCSS_DTRC_INTEN),
	DCSS_DBG_REG(DCSS_DTRC_FDINTR),
	DCSS_DBG_REG(DCSS_DTRC_DTCTRL),
	DCSS_DBG_REG(DCSS_DTRC_ARIDR),
	DCSS_DBG_REG(DCSS_DTRC_DTID2DDR),
	DCSS_DBG_REG(DCSS_DTRC_CONFIG),
	DCSS_DBG_REG(DCSS_DTRC_VER),
	DCSS_DBG_REG(DCSS_DTRC_PFCTRL),
	DCSS_DBG_REG(DCSS_DTRC_PFCR),
	DCSS_DBG_REG(DCSS_DTRC_TOCR),
};

static void dcss_dtrc_dump_frame_regs(struct seq_file *s, void *data,
				      int ch, int frame)
{
	struct dcss_soc *dcss = data;
	int i;

	seq_printf(s, "\t>> Dumping F%d regs:\n", frame);
	for (i = 0; i < ARRAY_SIZE(dtrc_frame_debug_reg); i++)
		seq_printf(s, "\t%-35s(0x%04x) -> 0x%08x\n",
			   dtrc_frame_debug_reg[i].name,
			   dtrc_frame_debug_reg[i].ofs + frame * DTRC_F1_OFS,
			   dcss_readl(dcss->dtrc_priv->ch[ch].base_reg +
				      dtrc_frame_debug_reg[i].ofs +
				      frame * DTRC_F1_OFS));
}

void dcss_dtrc_dump_regs(struct seq_file *s, void *data)
{
	struct dcss_soc *dcss = data;
	int ch, fr, i;

	for (ch = 0; ch < 2; ch++) {
		seq_printf(s, ">> Dumping DTRC for CH %d:\n", ch + 1);
		for (fr = 0; fr < 2; fr++)
			dcss_dtrc_dump_frame_regs(s, data, ch, fr);

		seq_printf(s, "\t>> Dumping DTRC CTRL regs for CH %d:\n",
			   ch + 1);
		for (i = 0; i < ARRAY_SIZE(dtrc_ctrl_debug_reg); i++)
			seq_printf(s, "\t%-35s(0x%04x) -> 0x%08x\n",
				   dtrc_ctrl_debug_reg[i].name,
				   dtrc_ctrl_debug_reg[i].ofs,
				   dcss_readl(dcss->dtrc_priv->ch[ch].base_reg +
					      dtrc_ctrl_debug_reg[i].ofs));
	}
}
#endif

static int dcss_dtrc_ch_init_all(struct dcss_soc *dcss, unsigned long dtrc_base)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;
	int i;

	for (i = 0; i < 2; i++) {
		ch = &dtrc->ch[i];

		ch->base_ofs = dtrc_base + i * 0x1000;

		ch->base_reg = devm_ioremap(dcss->dev, ch->base_ofs, SZ_4K);
		if (!ch->base_reg) {
			dev_err(dcss->dev, "dtrc: unable to remap ch base\n");
			return -ENOMEM;
		}

#if defined(USE_CTXLD)
		ch->ctx_id = CTX_SB_HP;
#endif
	}

	return 0;
}

static void dcss_dtrc_write(struct dcss_dtrc_priv *dtrc, int ch_num,
			    u32 val, u32 ofs)
{
#if !defined(USE_CTXLD)
	dcss_writel(val, dtrc->ch[ch_num].base_reg + ofs);
#else
	dcss_ctxld_write(dtrc->dcss, dtrc->ch[ch_num].ctx_id,
			 val, dtrc->ch[ch_num].base_ofs + ofs);
#endif
}

static void dcss_dtrc_write_irqsafe(struct dcss_dtrc_priv *dtrc, int ch_num,
				    u32 val, u32 ofs)
{
#if !defined(USE_CTXLD)
	dcss_writel(val, dtrc->ch[ch_num].base_reg + ofs);
#else
	dcss_ctxld_write_irqsafe(dtrc->dcss, dtrc->ch[ch_num].ctx_id,
				 val, dtrc->ch[ch_num].base_ofs + ofs);
#endif
}

int dcss_dtrc_init(struct dcss_soc *dcss, unsigned long dtrc_base)
{
	struct dcss_dtrc_priv *dtrc;

	dtrc = devm_kzalloc(dcss->dev, sizeof(*dtrc), GFP_KERNEL);
	if (!dtrc)
		return -ENOMEM;

	dcss->dtrc_priv = dtrc;
	dtrc->dcss = dcss;

	return dcss_dtrc_ch_init_all(dcss, dtrc_base);
}

void dcss_dtrc_exit(struct dcss_soc *dcss)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;

	/* reset the module to default */
	dcss_writel(HOT_RESET, dtrc->dtrc_reg + DCSS_DTRC_DTCTRL);
}

void dcss_dtrc_bypass(struct dcss_soc *dcss, int ch_num)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;

	if (ch_num == 0)
		return;

	ch_num -= 1;

	if (dtrc->ch[ch_num].bypass)
		return;

	dcss_dtrc_write(dtrc, ch_num, ARIDR_MODE_BYPASS, DCSS_DTRC_DTCTRL);
	dcss_dtrc_write(dtrc, ch_num, 0, DCSS_DTRC_DYTSADDR);
	dcss_dtrc_write(dtrc, ch_num, 0, DCSS_DTRC_DCTSADDR);
	dcss_dtrc_write(dtrc, ch_num, 0x0f0e0100, DCSS_DTRC_ARIDR);
	dcss_dtrc_write(dtrc, ch_num, 0x0f0e, DCSS_DTRC_DTID2DDR);

	dtrc->ch[ch_num].bypass = true;
}
EXPORT_SYMBOL(dcss_dtrc_bypass);

void dcss_dtrc_addr_set(struct dcss_soc *dcss, int ch_num, u32 p1_ba, u32 p2_ba,
			uint64_t dec_table_ofs)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;

	if (ch_num == 0)
		return;

	ch_num -= 1;

	ch = &dtrc->ch[ch_num];

	dcss_dtrc_write(dtrc, ch_num, p1_ba, DCSS_DTRC_DYDSADDR);
	dcss_dtrc_write(dtrc, ch_num, p2_ba, DCSS_DTRC_DCDSADDR);

	dcss_dtrc_write(dtrc, ch_num, p1_ba, DTRC_F1_OFS + DCSS_DTRC_DYDSADDR);
	dcss_dtrc_write(dtrc, ch_num, p2_ba, DTRC_F1_OFS + DCSS_DTRC_DCDSADDR);

	if (!ch->running) {
		dcss_dtrc_write(dtrc, ch_num, p1_ba, DCSS_DTRC_SYSSA);
		dcss_dtrc_write(dtrc, ch_num, p1_ba + ch->xres * ch->yres,
				DCSS_DTRC_SYSEA);

		dcss_dtrc_write(dtrc, ch_num, p2_ba, DCSS_DTRC_SUVSSA);
		dcss_dtrc_write(dtrc, ch_num, p2_ba + ch->xres * ch->yres / 2,
				DCSS_DTRC_SUVSEA);

		dcss_dtrc_write(dtrc, ch_num, p1_ba,
				DTRC_F1_OFS + DCSS_DTRC_SYSSA);
		dcss_dtrc_write(dtrc, ch_num, p1_ba + ch->xres * ch->yres,
				DTRC_F1_OFS + DCSS_DTRC_SYSEA);

		dcss_dtrc_write(dtrc, ch_num, p2_ba,
				DTRC_F1_OFS + DCSS_DTRC_SUVSSA);
		dcss_dtrc_write(dtrc, ch_num, p2_ba + ch->xres * ch->yres / 2,
				DTRC_F1_OFS + DCSS_DTRC_SUVSEA);

		ch->y_dec_ofs = dec_table_ofs & 0xFFFFFFFF;
		ch->uv_dec_ofs = dec_table_ofs >> 32;
	}

	dcss_dtrc_write(dtrc, ch_num,
		p1_ba + ch->y_dec_ofs, DCSS_DTRC_DYTSADDR);
	dcss_dtrc_write(dtrc, ch_num,
		p1_ba + ch->uv_dec_ofs, DCSS_DTRC_DCTSADDR);
	dcss_dtrc_write(dtrc, ch_num,
		p1_ba + ch->y_dec_ofs, DTRC_F1_OFS + DCSS_DTRC_DYTSADDR);
	dcss_dtrc_write(dtrc, ch_num,
		p1_ba + ch->uv_dec_ofs, DTRC_F1_OFS + DCSS_DTRC_DCTSADDR);

	dtrc->ch[ch_num].bypass = false;
}
EXPORT_SYMBOL(dcss_dtrc_addr_set);

void dcss_dtrc_set_res(struct dcss_soc *dcss, int ch_num, u32 xres, u32 yres)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;
	u32 frame_height, frame_width;
	int bank;

	if (ch_num == 0)
		return;

	ch_num -= 1;

	ch = &dtrc->ch[ch_num];
	bank = dcss_readl(ch->base_reg + DCSS_DTRC_DTCTRL) >> 31;

	ch->xres = xres;
	ch->yres = yres;

	frame_height = ((yres >> 3) << FRAME_HEIGHT_POS) & FRAME_HEIGHT_MASK;
	frame_width = ((xres >> 3) << FRAME_WIDTH_POS) & FRAME_WIDTH_MASK;

	dcss_dtrc_write(dcss->dtrc_priv, ch_num, frame_height | frame_width,
			DTRC_F1_OFS * bank + DCSS_DTRC_SIZE);

	dcss_dtrc_write(dcss->dtrc_priv, ch_num, frame_height | frame_width,
			DTRC_F1_OFS * (bank ^ 1) + DCSS_DTRC_SIZE);
}
EXPORT_SYMBOL(dcss_dtrc_set_res);

void dcss_dtrc_enable(struct dcss_soc *dcss, int ch_num, bool enable)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;
	int curr_frame;
	u32 fdctl, dtctrl;

	if (ch_num == 0)
		return;

	ch_num -= 1;

	ch = &dtrc->ch[ch_num];

	if (ch->bypass)
		return;

	if (!enable) {
		ch->running = false;
		return;
	}

	if (ch->running)
		return;

	dcss_update(HOT_RESET, HOT_RESET, ch->base_reg + DCSS_DTRC_DTCTRL);
	while (dcss_readl(ch->base_reg + DCSS_DTRC_DTCTRL) & HOT_RESET)
		usleep_range(100, 200);

	dcss_dtrc_write(dcss->dtrc_priv, ch_num, 0x0f0e0100,
			DCSS_DTRC_ARIDR);
	dcss_dtrc_write(dcss->dtrc_priv, ch_num, 0x0f0e,
			DCSS_DTRC_DTID2DDR);

	dtctrl = ADDRESS_ID_ENABLE | MERGE_ARID_ENABLE |
		 ((0xF << TABLE_DATA_SWAP_POS) & TABLE_DATA_SWAP_MASK) |
		 ((0x10 << BURST_LENGTH_POS) & BURST_LENGTH_MASK);

	if (ch->format_modifier == DRM_FORMAT_MOD_VSI_G1_TILED)
		dtctrl |= G1_TILED_DATA_EN;

	dcss_dtrc_write(dtrc, ch_num, dtctrl, DCSS_DTRC_DTCTRL);

	curr_frame = dcss_readl(ch->base_reg + DCSS_DTRC_DTCTRL) >> 31;

	fdctl = PIX_DEPTH_8BIT_EN;

	if (ch->format_modifier != DRM_FORMAT_MOD_VSI_G2_TILED_COMPRESSED)
		fdctl |= COMPRESSION_DIS;

	dcss_dtrc_write(dtrc, ch_num, fdctl,
			(curr_frame ^ 1) * DTRC_F1_OFS + DCSS_DTRC_DCTL);
	dcss_dtrc_write(dtrc, ch_num, fdctl | (enable ? CONFIG_READY : 0),
			curr_frame * DTRC_F1_OFS + DCSS_DTRC_DCTL);

	ch->curr_frame = curr_frame;
	ch->dctl = fdctl;
	ch->running = true;
}
EXPORT_SYMBOL(dcss_dtrc_enable);

bool dcss_dtrc_is_running(struct dcss_soc *dcss, int ch_num)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;

	if (!ch_num)
		return false;

	ch_num -= 1;

	ch = &dtrc->ch[ch_num];

	return ch->running;
}

void dcss_dtrc_set_format_mod(struct dcss_soc *dcss, int ch_num, u64 modifier)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;
	struct dcss_dtrc_ch *ch;

	if (!ch_num)
		return;

	ch_num -= 1;

	ch = &dtrc->ch[ch_num];

	ch->format_modifier = modifier;
}
EXPORT_SYMBOL(dcss_dtrc_set_format_mod);

static void dcss_dtrc_ch_switch_banks(struct dcss_dtrc_priv *dtrc, int dtrc_ch)
{
	struct dcss_dtrc_ch *ch = &dtrc->ch[dtrc_ch];

	if (!ch->running)
		return;

	ch->curr_frame ^= 1;

	dcss_dtrc_write_irqsafe(dtrc, dtrc_ch, ch->dctl | CONFIG_READY,
				ch->curr_frame * DTRC_F1_OFS + DCSS_DTRC_DCTL);
}

void dcss_dtrc_switch_banks(struct dcss_soc *dcss)
{
	struct dcss_dtrc_priv *dtrc = dcss->dtrc_priv;

	dcss_dtrc_ch_switch_banks(dtrc, 0);
	dcss_dtrc_ch_switch_banks(dtrc, 1);
}
