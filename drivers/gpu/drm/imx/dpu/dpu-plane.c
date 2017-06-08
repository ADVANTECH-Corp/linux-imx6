/*
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

#include <drm/drmP.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_plane_helper.h>
#include <video/dpu.h>
#include "dpu-plane.h"
#include "imx-drm.h"

/* RGB formats are widely supported by all fetch units */
static const uint32_t dpu_common_formats[] = {
	/* DRM_FORMAT_ARGB8888, */
	DRM_FORMAT_XRGB8888,
	/* DRM_FORMAT_ABGR8888, */
	DRM_FORMAT_XBGR8888,
	/* DRM_FORMAT_RGBA8888, */
	DRM_FORMAT_RGBX8888,
	/* DRM_FORMAT_BGRA8888, */
	DRM_FORMAT_BGRX8888,
	DRM_FORMAT_RGB888,
	DRM_FORMAT_BGR888,
	DRM_FORMAT_RGB565,
};

static void dpu_plane_destroy(struct drm_plane *plane)
{
	struct dpu_plane *dpu_plane = to_dpu_plane(plane);

	drm_plane_cleanup(plane);
	kfree(dpu_plane);
}

static void dpu_plane_reset(struct drm_plane *plane)
{
	struct dpu_plane_state *state;

	if (plane->state) {
		__drm_atomic_helper_plane_destroy_state(plane->state);
		kfree(to_dpu_plane_state(plane->state));
		plane->state = NULL;
	}

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return;

	state->base.zpos = plane->type == DRM_PLANE_TYPE_PRIMARY ? 0 : 1;

	plane->state = &state->base;
	plane->state->plane = plane;
	plane->state->rotation = DRM_MODE_ROTATE_0;
}

static struct drm_plane_state *
dpu_drm_atomic_plane_duplicate_state(struct drm_plane *plane)
{
	struct dpu_plane_state *state, *copy;

	if (WARN_ON(!plane->state))
		return NULL;

	copy = kmalloc(sizeof(*state), GFP_KERNEL);
	if (!copy)
		return NULL;

	__drm_atomic_helper_plane_duplicate_state(plane, &copy->base);
	state = to_dpu_plane_state(plane->state);
	copy->stage = state->stage;
	copy->source = state->source;
	copy->blend = state->blend;
	copy->layer_x = state->layer_x;
	copy->layer_y = state->layer_y;
	copy->base_x = state->base_x;
	copy->base_y = state->base_y;
	copy->base_w = state->base_w;
	copy->base_h = state->base_h;
	copy->is_top = state->is_top;

	return &copy->base;
}

static void dpu_drm_atomic_plane_destroy_state(struct drm_plane *plane,
					       struct drm_plane_state *state)
{
	__drm_atomic_helper_plane_destroy_state(state);
	kfree(to_dpu_plane_state(state));
}

static const struct drm_plane_funcs dpu_plane_funcs = {
	.update_plane	= drm_atomic_helper_update_plane,
	.disable_plane	= drm_atomic_helper_disable_plane,
	.destroy	= dpu_plane_destroy,
	.reset		= dpu_plane_reset,
	.atomic_duplicate_state	= dpu_drm_atomic_plane_duplicate_state,
	.atomic_destroy_state	= dpu_drm_atomic_plane_destroy_state,
};

static int dpu_plane_atomic_check(struct drm_plane *plane,
				  struct drm_plane_state *state)
{
	struct dpu_plane_state *dpstate = to_dpu_plane_state(state);
	struct drm_crtc_state *crtc_state;
	struct drm_framebuffer *fb = state->fb;
	struct drm_gem_cma_object *cma_obj;
	int bpp;

	/* pure software check */
	if (plane->type != DRM_PLANE_TYPE_PRIMARY)
		if (WARN_ON(dpstate->base_x || dpstate->base_y ||
			    dpstate->base_w || dpstate->base_h))
			return -EINVAL;

	/* ok to disable */
	if (!fb) {
		dpstate->stage = LB_PRIM_SEL__DISABLE;
		dpstate->source = LB_SEC_SEL__DISABLE;
		dpstate->blend = ID_NONE;
		dpstate->layer_x = 0;
		dpstate->layer_y = 0;
		dpstate->base_x = 0;
		dpstate->base_y = 0;
		dpstate->base_w = 0;
		dpstate->base_h = 0;
		dpstate->is_top = false;
		return 0;
	}

	/* no scaling */
	if (state->src_w >> 16 != state->crtc_w ||
	    state->src_h >> 16 != state->crtc_h)
		return -EINVAL;

	/* no off screen */
	if (state->crtc_x < 0 || state->crtc_y < 0)
		return -EINVAL;
	if (state->crtc) {
		crtc_state =
			drm_atomic_get_existing_crtc_state(state->state,
							state->crtc);
		if (WARN_ON(!crtc_state))
			return -EINVAL;

		if (state->crtc_x + state->crtc_w >
		    crtc_state->adjusted_mode.hdisplay)
			return -EINVAL;
		if (state->crtc_y + state->crtc_h >
		    crtc_state->adjusted_mode.vdisplay)
			return -EINVAL;
	}

	/* base address alignment check */
	cma_obj = drm_fb_cma_get_gem_obj(fb, 0);
	bpp = drm_format_plane_cpp(fb->format->format, 0) * 8;
	switch (bpp) {
	case 32:
		if (cma_obj->paddr & 0x3)
			return -EINVAL;
		break;
	case 16:
		if (cma_obj->paddr & 0x1)
			return -EINVAL;
		break;
	}

	if (fb->pitches[0] > 0x10000)
		return -EINVAL;

	return 0;
}

static void dpu_plane_atomic_update(struct drm_plane *plane,
				    struct drm_plane_state *old_state)
{
	struct dpu_plane *dplane = to_dpu_plane(plane);
	struct drm_plane_state *state = plane->state;
	struct dpu_plane_state *dpstate = to_dpu_plane_state(state);
	struct drm_framebuffer *fb = state->fb;
	struct dpu_plane_res *res = &dplane->grp->res;
	struct dpu_fetchdecode *fd;
	struct dpu_layerblend *lb;
	struct dpu_constframe *cf;
	struct dpu_extdst *ed;
	struct drm_gem_cma_object *cma_obj;
	struct device *dev = plane->dev->dev;
	unsigned int src_x, src_y, src_w, src_h;
	int bpp, fd_id, lb_id;

	/*
	 * Do nothing since the plane is disabled by
	 * crtc_func->atomic_begin/flush.
	 */
	if (!fb)
		return;

	fd_id = source_to_id(dpstate->source);
	if (fd_id < 0)
		return;

	lb_id = blend_to_id(dpstate->blend);
	if (lb_id < 0)
		return;

	fd = res->fd[fd_id];
	lb = res->lb[lb_id];

	cma_obj = drm_fb_cma_get_gem_obj(fb, 0);

	src_x = state->src_x >> 16;
	src_y = state->src_y >> 16;
	src_w = state->src_w >> 16;
	src_h = state->src_h >> 16;

	bpp = drm_format_plane_cpp(fb->format->format, 0) * 8;

	fetchdecode_source_bpp(fd, bpp);
	fetchdecode_source_stride(fd, fb->pitches[0]);
	fetchdecode_src_buf_dimensions(fd, src_w, src_h);
	fetchdecode_set_fmt(fd, fb->format->format);
	fetchdecode_clipoffset(fd, src_x, src_y);
	fetchdecode_clipdimensions(fd, src_w, src_h);
	fetchdecode_layerproperty(fd, true);
	fetchdecode_framedimensions(fd, state->crtc_w, state->crtc_h);
	fetchdecode_baseaddress(fd, cma_obj->paddr);

	layerblend_pixengcfg_dynamic_prim_sel(lb, dpstate->stage);
	layerblend_pixengcfg_dynamic_sec_sel(lb, dpstate->source);
	layerblend_control(lb, BLEND);
	layerblend_pixengcfg_clken(lb, CLKEN__AUTOMATIC);
	layerblend_position(lb, dpstate->layer_x, dpstate->layer_y);

	if (plane->type == DRM_PLANE_TYPE_PRIMARY) {
		cf = res->cf[dplane->stream_id];
		constframe_framedimensions(cf,
					dpstate->base_w, dpstate->base_h);
		constframe_constantcolor(cf, 0, 0, 0, 0);

		if (state->crtc)
			framegen_sacfg(res->fg,
					dpstate->base_x, dpstate->base_y);
	}

	if (dpstate->is_top) {
		ed = res->ed[dplane->stream_id];
		extdst_pixengcfg_src_sel(ed, (extdst_src_sel_t)dpstate->blend);
	}

	dev_dbg(dev, "[PLANE:%d:%s] source-0x%02x stage-0x%02x blend-0x%02x\n",
			plane->base.id, plane->name,
			dpstate->source, dpstate->stage, dpstate->blend);
}

static const struct drm_plane_helper_funcs dpu_plane_helper_funcs = {
	.atomic_check = dpu_plane_atomic_check,
	.atomic_update = dpu_plane_atomic_update,
};

struct dpu_plane *dpu_plane_init(struct drm_device *drm,
				 unsigned int possible_crtcs,
				 unsigned int stream_id,
				 struct dpu_plane_grp *grp,
				 enum drm_plane_type type)
{
	struct dpu_plane *dpu_plane;
	struct drm_plane *plane;
	unsigned int ov_num;
	int ret;

	dpu_plane = kzalloc(sizeof(*dpu_plane), GFP_KERNEL);
	if (!dpu_plane)
		return ERR_PTR(-ENOMEM);

	dpu_plane->stream_id = stream_id;
	dpu_plane->grp = grp;

	mutex_lock(&grp->lock);
	list_add_tail(&dpu_plane->head, &grp->list);
	mutex_unlock(&grp->lock);

	plane = &dpu_plane->base;

	ret = drm_universal_plane_init(drm, plane, possible_crtcs,
				       &dpu_plane_funcs, dpu_common_formats,
				       ARRAY_SIZE(dpu_common_formats), NULL,
				       type, NULL);
	if (ret) {
		kfree(dpu_plane);
		return ERR_PTR(ret);
	}

	drm_plane_helper_add(plane, &dpu_plane_helper_funcs);

	switch (type) {
	case DRM_PLANE_TYPE_PRIMARY:
		ret = drm_plane_create_zpos_immutable_property(plane, 0);
		break;
	case DRM_PLANE_TYPE_OVERLAY:
		/* filter out the primary plane */
		ov_num = grp->hw_plane_num - 1;

		ret = drm_plane_create_zpos_property(plane, 1, 1, ov_num);
		break;
	default:
		ret = -EINVAL;
	}

	if (ret)
		return ERR_PTR(ret);

	return dpu_plane;
}