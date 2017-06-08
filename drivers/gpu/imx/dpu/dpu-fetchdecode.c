/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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

#include <linux/io.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <video/dpu.h>
#include "dpu-prv.h"

#define PIXENGCFG_DYNAMIC		0x8
#define SRC_NUM				3
#define FD_NUM				4
static const fd_dynamic_src_sel_t fd_srcs[FD_NUM][SRC_NUM] = {
	{ FD_SRC_DISABLE, FD_SRC_FETCHECO0, FD_SRC_FETCHDECODE2 },
	{ FD_SRC_DISABLE, FD_SRC_FETCHECO1, FD_SRC_FETCHDECODE3 },
	{ FD_SRC_DISABLE, FD_SRC_FETCHECO0, FD_SRC_FETCHECO2 },
	{ FD_SRC_DISABLE, FD_SRC_FETCHECO1, FD_SRC_FETCHECO2 },
};

#define PIXENGCFG_STATUS		0xC

#define RINGBUFSTARTADDR0		0x10
#define RINGBUFWRAPADDR0		0x14
#define FRAMEPROPERTIES0		0x18
#define BASEADDRESS0			0x1C
#define SOURCEBUFFERATTRIBUTES0		0x20
#define SOURCEBUFFERDIMENSION0		0x24
#define COLORCOMPONENTBITS0		0x28
#define COLORCOMPONENTSHIFT0		0x2C
#define LAYEROFFSET0			0x30
#define CLIPWINDOWOFFSET0		0x34
#define CLIPWINDOWDIMENSIONS0		0x38
#define CONSTANTCOLOR0			0x3C
#define LAYERPROPERTY0			0x40
#define FRAMEDIMENSIONS			0x44
#define FRAMERESAMPLING			0x48
#define DECODECONTROL			0x4C
#define SOURCEBUFFERLENGTH		0x50
#define CONTROL				0x54
#define CONTROLTRIGGER			0x58
#define START				0x5C
#define FETCHTYPE			0x60
#define DECODERSTATUS			0x64
#define READADDRESS0			0x68
#define BURSTBUFFERPROPERTIES		0x6C
#define STATUS				0x70
#define HIDDENSTATUS			0x74

static const shadow_load_req_t fd_shdlreqs[] = {
	SHLDREQID_FETCHDECODE0, SHLDREQID_FETCHDECODE1,
	SHLDREQID_FETCHDECODE2, SHLDREQID_FETCHDECODE3,
};

struct dpu_fetchdecode {
	void __iomem *pec_base;
	void __iomem *base;
	struct mutex mutex;
	int id;
	bool inuse;
	struct dpu_soc *dpu;
	fetchtype_t fetchtype;
	shadow_load_req_t shdlreq;
};

static inline u32 dpu_pec_fd_read(struct dpu_fetchdecode *fd,
				  unsigned int offset)
{
	return readl(fd->pec_base + offset);
}

static inline void dpu_pec_fd_write(struct dpu_fetchdecode *fd, u32 value,
				    unsigned int offset)
{
	writel(value, fd->pec_base + offset);
}

static inline u32 dpu_fd_read(struct dpu_fetchdecode *fd, unsigned int offset)
{
	return readl(fd->base + offset);
}

static inline void dpu_fd_write(struct dpu_fetchdecode *fd, u32 value,
				unsigned int offset)
{
	writel(value, fd->base + offset);
}

int fetchdecode_pixengcfg_dynamic_src_sel(struct dpu_fetchdecode *fd,
					  fd_dynamic_src_sel_t src)
{
	int i;

	mutex_lock(&fd->mutex);
	for (i = 0; i < SRC_NUM; i++) {
		if (fd_srcs[fd->id][i] == src) {
			dpu_pec_fd_write(fd, src, PIXENGCFG_DYNAMIC);
			mutex_unlock(&fd->mutex);
			return 0;
		}
	}
	mutex_unlock(&fd->mutex);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(fetchdecode_pixengcfg_dynamic_src_sel);

static inline u32 rgb_color(u8 r, u8 g, u8 b, u8 a)
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

static inline u32 yuv_color(u8 y, u8 u, u8 v)
{
	return (y << 24) | (u << 16) | (v << 8);
}

void fetchdecode_shden(struct dpu_fetchdecode *fd, bool enable)
{
	u32 val;

	mutex_lock(&fd->mutex);
	val = dpu_fd_read(fd, STATICCONTROL);
	if (enable)
		val |= SHDEN;
	else
		val &= ~SHDEN;
	dpu_fd_write(fd, val, STATICCONTROL);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_shden);

void fetchdecode_baddr_autoupdate(struct dpu_fetchdecode *fd, u8 layer_mask)
{
	u32 val;

	mutex_lock(&fd->mutex);
	val = dpu_fd_read(fd, STATICCONTROL);
	val &= ~BASEADDRESSAUTOUPDATE_MASK;
	val |= BASEADDRESSAUTOUPDATE(layer_mask);
	dpu_fd_write(fd, val, STATICCONTROL);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_baddr_autoupdate);

void fetchdecode_baseaddress(struct dpu_fetchdecode *fd, dma_addr_t paddr)
{
	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, paddr, BASEADDRESS0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_baseaddress);

void fetchdecode_source_bpp(struct dpu_fetchdecode *fd, int bpp)
{
	u32 val;

	mutex_lock(&fd->mutex);
	val = dpu_fd_read(fd, SOURCEBUFFERATTRIBUTES0);
	val &= ~0x3f0000;
	val |= BITSPERPIXEL(bpp);
	dpu_fd_write(fd, val, SOURCEBUFFERATTRIBUTES0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_source_bpp);

void fetchdecode_source_stride(struct dpu_fetchdecode *fd, int stride)
{
	u32 val;

	mutex_lock(&fd->mutex);
	val = dpu_fd_read(fd, SOURCEBUFFERATTRIBUTES0);
	val &= ~0xffff;
	val |= STRIDE(stride);
	dpu_fd_write(fd, val, SOURCEBUFFERATTRIBUTES0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_source_stride);

void fetchdecode_src_buf_dimensions(struct dpu_fetchdecode *fd, unsigned int w,
				    unsigned int h)
{
	u32 val;

	val = LINEWIDTH(w) | LINECOUNT(h);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, SOURCEBUFFERDIMENSION0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_src_buf_dimensions);

void fetchdecode_set_fmt(struct dpu_fetchdecode *fd, u32 fmt)
{
	u32 bits, shift;
	int i;

	for (i = 0; i < ARRAY_SIZE(dpu_pixel_format_matrix); i++) {
		if (dpu_pixel_format_matrix[i].pixel_format == fmt) {
			bits = dpu_pixel_format_matrix[i].bits;
			shift = dpu_pixel_format_matrix[i].shift;

			mutex_lock(&fd->mutex);
			dpu_fd_write(fd, bits, COLORCOMPONENTBITS0);
			dpu_fd_write(fd, shift, COLORCOMPONENTSHIFT0);
			mutex_unlock(&fd->mutex);
			return;
		}
	}

	WARN_ON(1);
}
EXPORT_SYMBOL_GPL(fetchdecode_set_fmt);

void fetchdecode_layeroffset(struct dpu_fetchdecode *fd, unsigned int x,
			     unsigned int y)
{
	u32 val;

	val = LAYERXOFFSET(x) | LAYERYOFFSET(y);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, LAYEROFFSET0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_layeroffset);

void fetchdecode_clipoffset(struct dpu_fetchdecode *fd, unsigned int x,
			    unsigned int y)
{
	u32 val;

	val = CLIPWINDOWXOFFSET(x) | CLIPWINDOWYOFFSET(y);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, CLIPWINDOWOFFSET0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_clipoffset);

void fetchdecode_layerproperty(struct dpu_fetchdecode *fd, bool enable)
{
	u32 val;

	if (enable)
		val = SOURCEBUFFERENABLE | CLIPWINDOWENABLE;
	else
		val = 0;

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, LAYERPROPERTY0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_layerproperty);

void fetchdecode_clipdimensions(struct dpu_fetchdecode *fd, unsigned int w,
				unsigned int h)
{
	u32 val;

	val = CLIPWINDOWWIDTH(w) | CLIPWINDOWHEIGHT(h);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, CLIPWINDOWDIMENSIONS0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_clipdimensions);

void fetchdecode_framedimensions(struct dpu_fetchdecode *fd, unsigned int w,
				 unsigned int h)
{
	u32 val;

	val = FRAMEWIDTH(w) | FRAMEHEIGHT(h);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, FRAMEDIMENSIONS);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_framedimensions);

void fetchdecode_rgb_constantcolor(struct dpu_fetchdecode *fd,
					u8 r, u8 g, u8 b, u8 a)
{
	u32 val;

	val = rgb_color(r, g, b, a);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, CONSTANTCOLOR0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_rgb_constantcolor);

void fetchdecode_yuv_constantcolor(struct dpu_fetchdecode *fd, u8 y, u8 u, u8 v)
{
	u32 val;

	val = yuv_color(y, u, v);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, CONSTANTCOLOR0);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_yuv_constantcolor);

void fetchdecode_controltrigger(struct dpu_fetchdecode *fd, bool trigger)
{
	u32 val;

	val = trigger ? SHDTOKGEN : 0;

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, val, CONTROLTRIGGER);
	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(fetchdecode_controltrigger);

int fetchdecode_fetchtype(struct dpu_fetchdecode *fd, fetchtype_t *type)
{
	struct dpu_soc *dpu = fd->dpu;
	u32 val;

	mutex_lock(&fd->mutex);
	val = dpu_fd_read(fd, FETCHTYPE);
	val &= FETCHTYPE_MASK;
	mutex_unlock(&fd->mutex);

	switch (val) {
	case FETCHTYPE__DECODE:
		dev_dbg(dpu->dev, "FetchDecode%d with RL and RLAD decoder\n",
				fd->id);
		break;
	case FETCHTYPE__LAYER:
		dev_dbg(dpu->dev, "FetchDecode%d with fractional "
				"plane(8 layers)\n", fd->id);
		break;
	case FETCHTYPE__WARP:
		dev_dbg(dpu->dev, "FetchDecode%d with arbitrary warping and "
				"fractional plane(8 layers)\n", fd->id);
		break;
	case FETCHTYPE__ECO:
		dev_dbg(dpu->dev, "FetchDecode%d with minimum feature set for "
				"alpha, chroma and coordinate planes\n",
				fd->id);
		break;
	case FETCHTYPE__PERSP:
		dev_dbg(dpu->dev, "FetchDecode%d with affine, perspective and "
				"arbitrary warping\n", fd->id);
		break;
	case FETCHTYPE__ROT:
		dev_dbg(dpu->dev, "FetchDecode%d with affine and arbitrary "
				"warping\n", fd->id);
		break;
	case FETCHTYPE__DECODEL:
		dev_dbg(dpu->dev, "FetchDecode%d with RL and RLAD decoder, "
				"reduced feature set\n", fd->id);
		break;
	case FETCHTYPE__LAYERL:
		dev_dbg(dpu->dev, "FetchDecode%d with fractional "
				"plane(8 layers), reduced feature set\n",
				fd->id);
		break;
	case FETCHTYPE__ROTL:
		dev_dbg(dpu->dev, "FetchDecode%d with affine and arbitrary "
				"warping, reduced feature set\n", fd->id);
		break;
	default:
		dev_warn(dpu->dev, "Invalid fetch type %u for FetchDecode%d\n",
				val, fd->id);
		return -EINVAL;
	}

	*type = val;
	return 0;
}
EXPORT_SYMBOL_GPL(fetchdecode_fetchtype);

shadow_load_req_t fetchdecode_to_shdldreq_t(struct dpu_fetchdecode *fd)
{
	shadow_load_req_t t = 0;

	switch (fd->id) {
	case 0:
		t = SHLDREQID_FETCHDECODE0;
		break;
	case 1:
		t = SHLDREQID_FETCHDECODE1;
		break;
	case 2:
		t = SHLDREQID_FETCHDECODE2;
		break;
	case 3:
		t = SHLDREQID_FETCHDECODE3;
		break;
	default:
		break;
	}

	return t;
}
EXPORT_SYMBOL_GPL(fetchdecode_to_shdldreq_t);

struct dpu_fetchdecode *dpu_fd_get(struct dpu_soc *dpu, int id)
{
	struct dpu_fetchdecode *fd;
	int i;

	for (i = 0; i < ARRAY_SIZE(fd_ids); i++)
		if (fd_ids[i] == id)
			break;

	if (i == ARRAY_SIZE(fd_ids))
		return ERR_PTR(-EINVAL);

	fd = dpu->fd_priv[i];

	mutex_lock(&fd->mutex);

	if (fd->inuse) {
		fd = ERR_PTR(-EBUSY);
		goto out;
	}

	fd->inuse = true;
out:
	mutex_unlock(&fd->mutex);

	return fd;
}
EXPORT_SYMBOL_GPL(dpu_fd_get);

void dpu_fd_put(struct dpu_fetchdecode *fd)
{
	mutex_lock(&fd->mutex);

	fd->inuse = false;

	mutex_unlock(&fd->mutex);
}
EXPORT_SYMBOL_GPL(dpu_fd_put);

int dpu_fd_init(struct dpu_soc *dpu, unsigned int id,
		unsigned long pec_base, unsigned long base)
{
	struct dpu_fetchdecode *fd;
	int ret, i;

	fd = devm_kzalloc(dpu->dev, sizeof(*fd), GFP_KERNEL);
	if (!fd)
		return -ENOMEM;

	dpu->fd_priv[id] = fd;

	fd->pec_base = devm_ioremap(dpu->dev, pec_base, SZ_16);
	if (!fd->pec_base)
		return -ENOMEM;

	fd->base = devm_ioremap(dpu->dev, base, SZ_1K);
	if (!fd->base)
		return -ENOMEM;

	fd->dpu = dpu;
	fd->id = id;
	for (i = 0; i < ARRAY_SIZE(fd_ids); i++) {
		if (fd_ids[i] == id) {
			fd->shdlreq = fd_shdlreqs[i];
			break;
		}
	}
	mutex_init(&fd->mutex);

	ret = fetchdecode_pixengcfg_dynamic_src_sel(fd, FD_SRC_DISABLE);
	if (ret < 0)
		return ret;

	ret = fetchdecode_fetchtype(fd, &fd->fetchtype);
	if (ret < 0)
		return ret;

	fetchdecode_baddr_autoupdate(fd, 0x0);
	fetchdecode_shden(fd, true);

	mutex_lock(&fd->mutex);
	dpu_fd_write(fd, SETNUMBUFFERS(16) | SETBURSTLENGTH(16),
			BURSTBUFFERMANAGEMENT);
	mutex_unlock(&fd->mutex);

	return 0;
}