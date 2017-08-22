/*
 * Freescale i.MX drm driver
 *
 * Copyright (C) 2011 Sascha Hauer, Pengutronix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/component.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <drm/drmP.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_of.h>
#include <drm/imx_drm.h>
#include <video/imx-ipu-v3.h>
#include <video/dpu.h>

#include "imx-drm.h"

struct imx_drm_component {
	struct device_node *of_node;
	struct list_head list;
};

struct imx_drm_crtc {
	struct drm_crtc				*crtc;
	struct imx_drm_crtc_helper_funcs	imx_drm_helper_funcs;
};

struct {
	struct mutex mutex;
	struct list_head ioctls;
} imx_ioctls = {
	.mutex = __MUTEX_INITIALIZER(imx_ioctls.mutex),
	.ioctls = LIST_HEAD_INIT(imx_ioctls.ioctls),
};

#if IS_ENABLED(CONFIG_DRM_FBDEV_EMULATION)
static int legacyfb_depth = 16;
module_param(legacyfb_depth, int, 0444);
#endif

static void imx_drm_driver_lastclose(struct drm_device *drm)
{
	struct imx_drm_device *imxdrm = drm->dev_private;

	drm_fbdev_cma_restore_mode(imxdrm->fbhelper);
}

static int imx_drm_enable_vblank(struct drm_device *drm, unsigned int pipe)
{
	struct imx_drm_device *imxdrm = drm->dev_private;
	struct imx_drm_crtc *imx_drm_crtc = imxdrm->crtc[pipe];
	int ret;

	if (!imx_drm_crtc)
		return -EINVAL;

	if (!imx_drm_crtc->imx_drm_helper_funcs.enable_vblank)
		return -ENOSYS;

	ret = imx_drm_crtc->imx_drm_helper_funcs.enable_vblank(
			imx_drm_crtc->crtc);

	return ret;
}

static void imx_drm_disable_vblank(struct drm_device *drm, unsigned int pipe)
{
	struct imx_drm_device *imxdrm = drm->dev_private;
	struct imx_drm_crtc *imx_drm_crtc = imxdrm->crtc[pipe];

	if (!imx_drm_crtc)
		return;

	if (!imx_drm_crtc->imx_drm_helper_funcs.disable_vblank)
		return;

	imx_drm_crtc->imx_drm_helper_funcs.disable_vblank(imx_drm_crtc->crtc);
}

static long
imx_drm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
       struct drm_file *file_priv = NULL;
       struct drm_device *dev = NULL;

#define MAX_DATA_STACK 128
       char data[MAX_DATA_STACK];
       unsigned int in_size;
       struct imx_drm_ioctl *pos;

       file_priv = filp->private_data;
       if (!file_priv) {
               return -EFAULT;
       }

       dev = file_priv->minor->dev;
       if (!dev) {
               return -EFAULT;
       }

       in_size = _IOC_SIZE(cmd);
       if (copy_from_user(data, (void __user *) arg, in_size) != 0) {
               return -EFAULT;
       }

       list_for_each_entry(pos, &imx_ioctls.ioctls, next) {
               if (pos->ioctl->cmd == cmd) {
                       long ret;
                       mutex_lock(&imx_ioctls.mutex);
                       ret = pos->ioctl->func(dev, data, file_priv);
                       mutex_unlock(&imx_ioctls.mutex);
                       return ret;
               }
       }


       return drm_ioctl(filp, cmd, arg);
}

static const struct file_operations imx_drm_driver_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	.release = drm_release,
	.unlocked_ioctl = imx_drm_ioctl,
	.mmap = drm_gem_cma_mmap,
	.poll = drm_poll,
	.read = drm_read,
	.llseek = noop_llseek,
};

void imx_drm_connector_destroy(struct drm_connector *connector)
{
	drm_connector_unregister(connector);
	drm_connector_cleanup(connector);
}
EXPORT_SYMBOL_GPL(imx_drm_connector_destroy);

void imx_drm_encoder_destroy(struct drm_encoder *encoder)
{
	drm_encoder_cleanup(encoder);
}
EXPORT_SYMBOL_GPL(imx_drm_encoder_destroy);

/*
 * imx_drm_add_crtc - add a new crtc
 */
int imx_drm_add_crtc(struct drm_device *drm, struct drm_crtc *crtc,
		struct imx_drm_crtc **new_crtc, struct drm_plane *primary_plane,
		const struct imx_drm_crtc_helper_funcs *imx_drm_helper_funcs,
		struct device_node *port)
{
	struct imx_drm_device *imxdrm = drm->dev_private;
	struct imx_drm_crtc *imx_drm_crtc;

	/*
	 * The vblank arrays are dimensioned by MAX_CRTC - we can't
	 * pass IDs greater than this to those functions.
	 */
	if (imxdrm->pipes >= MAX_CRTC)
		return -EINVAL;

	if (imxdrm->drm->open_count)
		return -EBUSY;

	imx_drm_crtc = kzalloc(sizeof(*imx_drm_crtc), GFP_KERNEL);
	if (!imx_drm_crtc)
		return -ENOMEM;

	imx_drm_crtc->imx_drm_helper_funcs = *imx_drm_helper_funcs;
	imx_drm_crtc->crtc = crtc;

	crtc->port = port;

	imxdrm->crtc[imxdrm->pipes++] = imx_drm_crtc;

	*new_crtc = imx_drm_crtc;

	drm_crtc_helper_add(crtc,
			imx_drm_crtc->imx_drm_helper_funcs.crtc_helper_funcs);

	drm_crtc_init_with_planes(drm, crtc, primary_plane, NULL,
			imx_drm_crtc->imx_drm_helper_funcs.crtc_funcs, NULL);

	return 0;
}
EXPORT_SYMBOL_GPL(imx_drm_add_crtc);

/*
 * imx_drm_remove_crtc - remove a crtc
 */
int imx_drm_remove_crtc(struct imx_drm_crtc *imx_drm_crtc)
{
	struct imx_drm_device *imxdrm = imx_drm_crtc->crtc->dev->dev_private;
	unsigned int pipe = drm_crtc_index(imx_drm_crtc->crtc);

	drm_crtc_cleanup(imx_drm_crtc->crtc);

	imxdrm->crtc[pipe] = NULL;

	kfree(imx_drm_crtc);

	return 0;
}
EXPORT_SYMBOL_GPL(imx_drm_remove_crtc);

int imx_drm_encoder_parse_of(struct drm_device *drm,
	struct drm_encoder *encoder, struct device_node *np)
{
	uint32_t crtc_mask = drm_of_find_possible_crtcs(drm, np);

	/*
	 * If we failed to find the CRTC(s) which this encoder is
	 * supposed to be connected to, it's because the CRTC has
	 * not been registered yet.  Defer probing, and hope that
	 * the required CRTC is added later.
	 */
	if (crtc_mask == 0)
		return -EPROBE_DEFER;

	encoder->possible_crtcs = crtc_mask;

	/* FIXME: this is the mask of outputs which can clone this output. */
	encoder->possible_clones = ~0;

	return 0;
}
EXPORT_SYMBOL_GPL(imx_drm_encoder_parse_of);

int
imx_drm_register_ioctl(struct drm_ioctl_desc *ioctl_desc)
{
	struct imx_drm_ioctl *__ioctl = kzalloc(sizeof(*__ioctl), GFP_KERNEL);

	if (!__ioctl)
		return -EFAULT;

	__ioctl->ioctl = ioctl_desc;
	INIT_LIST_HEAD(&__ioctl->next);

	mutex_lock(&imx_ioctls.mutex);
	list_add_tail(&__ioctl->next, &imx_ioctls.ioctls);
	mutex_unlock(&imx_ioctls.mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(imx_drm_register_ioctl);

int
imx_drm_unregister_ioctl(struct drm_ioctl_desc *ioctl_desc)
{
	int err = -ENOENT;
	struct imx_drm_ioctl *pos;

	mutex_lock(&imx_ioctls.mutex);
	list_for_each_entry(pos, &imx_ioctls.ioctls, next) {
		if (pos->ioctl->cmd == ioctl_desc->cmd) {
			list_del(&pos->next);
			kfree(pos);
			err = 0;
			break;
		}
	}
	mutex_unlock(&imx_ioctls.mutex);

	return err;
}
EXPORT_SYMBOL_GPL(imx_drm_unregister_ioctl);


static const struct drm_ioctl_desc imx_drm_ioctls[] = {
	/* none so far */
};

EXPORT_SYMBOL_GPL(imx_drm_ioctls);

static struct drm_driver imx_drm_driver = {
	.driver_features	= DRIVER_MODESET | DRIVER_GEM | DRIVER_PRIME |
				  DRIVER_ATOMIC | DRIVER_RENDER,
	.lastclose		= imx_drm_driver_lastclose,
	.gem_free_object_unlocked = drm_gem_cma_free_object,
	.gem_vm_ops		= &drm_gem_cma_vm_ops,
	.dumb_create		= drm_gem_cma_dumb_create,
	.dumb_map_offset	= drm_gem_cma_dumb_map_offset,
	.dumb_destroy		= drm_gem_dumb_destroy,

	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle,
	.gem_prime_import	= drm_gem_prime_import,
	.gem_prime_export	= drm_gem_prime_export,
	.gem_prime_get_sg_table	= drm_gem_cma_prime_get_sg_table,
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table,
	.gem_prime_vmap		= drm_gem_cma_prime_vmap,
	.gem_prime_vunmap	= drm_gem_cma_prime_vunmap,
	.gem_prime_mmap		= drm_gem_cma_prime_mmap,
	.get_vblank_counter	= drm_vblank_no_hw_counter,
	.enable_vblank		= imx_drm_enable_vblank,
	.disable_vblank		= imx_drm_disable_vblank,
	.ioctls			= imx_drm_ioctls,
	.num_ioctls		= ARRAY_SIZE(imx_drm_ioctls),
	.fops			= &imx_drm_driver_fops,
	.name			= "imx-drm",
	.desc			= "i.MX DRM graphics",
	.date			= "20120507",
	.major			= 1,
	.minor			= 0,
	.patchlevel		= 0,
};

static int compare_of(struct device *dev, void *data)
{
	struct device_node *np = data;

	/* Special case for DI, dev->of_node may not be set yet */
	if (strcmp(dev->driver->name, "imx-ipuv3-crtc") == 0) {
		struct ipu_client_platformdata *pdata = dev->platform_data;

		return pdata->of_node == np;
	} else if (strcmp(dev->driver->name, "imx-dpu-crtc") == 0) {
		struct dpu_client_platformdata *pdata = dev->platform_data;

		return pdata->of_node == np;
	}

	/* Special case for LDB, one device for two channels */
	if (of_node_cmp(np->name, "lvds-channel") == 0) {
		np = of_get_parent(np);
		of_node_put(np);
	}

	return dev->of_node == np;
}

static int compare_str(struct device *dev, void *data)
{
	return !strncmp(dev_name(dev), (char *) data, strlen(dev_name(dev)));
}

static int
add_display_components(struct device *dev, struct component_match **matchptr)
{
	struct device_node *ep, *port, *remote;
	int i;

	if (!dev->of_node)
		return -EINVAL;

	/*
	 * Bind the crtc's ports first, so that drm_of_find_possible_crtcs()
	 * called from encoder's .bind callbacks works as expected
	 */
	for (i = 0; ; i++) {
		port = of_parse_phandle(dev->of_node, "ports", i);
		if (!port)
			break;

		if (!of_device_is_available(port->parent)) {
			of_node_put(port);
			continue;
		}

		component_match_add(dev, matchptr, compare_of, port);
		of_node_put(port);
	}

	if (i == 0) {
		dev_err(dev, "missing 'ports' property\n");
		return -ENODEV;
	}

	if (!(*matchptr)) {
		dev_err(dev, "no available port\n");
		return -ENODEV;
	}

	/*
	 * For bound crtcs, bind the encoders attached to their remote endpoint
	 */
	for (i = 0; ; i++) {
		port = of_parse_phandle(dev->of_node, "ports", i);
		if (!port)
			break;

		if (!of_device_is_available(port->parent)) {
			of_node_put(port);
			continue;
		}

		for_each_child_of_node(port, ep) {
			remote = of_graph_get_remote_port_parent(ep);
			if (!remote || !of_device_is_available(remote)) {
				of_node_put(remote);
				continue;
			} else if (!of_device_is_available(remote->parent)) {
				dev_warn(dev, "parent device of %s is not available\n",
						remote->full_name);
				of_node_put(remote);
				continue;
			}

			component_match_add(dev, matchptr, compare_of, remote);
			of_node_put(remote);
		}
		of_node_put(port);
	}

	return 0;
}


static void add_dpu_bliteng_components(struct device *dev,
		struct component_match **matchptr)
{
	int i;
	char blit[128];
	void *tmp_blit;

	/* we assume that the platform device id starts from 0 */
	for (i = 0; i < MAX_DPU; i++) {
		memset(blit, 0, sizeof(blit));
		snprintf(blit, sizeof(blit), "%s.%d", "imx-drm-dpu-bliteng", i);

		tmp_blit = kmemdup(blit, sizeof(blit), GFP_KERNEL);
		component_match_add(dev, matchptr, compare_str, tmp_blit);
	}
}


static int imx_drm_bind(struct device *dev)
{
	struct drm_device *drm;
	struct imx_drm_device *imxdrm;
	int ret;

	drm = drm_dev_alloc(&imx_drm_driver, dev);
	if (IS_ERR(drm))
		return PTR_ERR(drm);

	imxdrm = devm_kzalloc(dev, sizeof(*imxdrm), GFP_KERNEL);
	if (!imxdrm) {
		ret = -ENOMEM;
		goto err_unref;
	}

	imxdrm->drm = drm;
	drm->dev_private = imxdrm;

	/*
	 * enable drm irq mode.
	 * - with irq_enabled = true, we can use the vblank feature.
	 *
	 * P.S. note that we wouldn't use drm irq handler but
	 *      just specific driver own one instead because
	 *      drm framework supports only one irq handler and
	 *      drivers can well take care of their interrupts
	 */
	drm->irq_enabled = true;

	/*
	 * set max width and height as default value(4096x4096).
	 * this value would be used to check framebuffer size limitation
	 * at drm_mode_addfb().
	 */
	drm->mode_config.min_width = 64;
	drm->mode_config.min_height = 64;
	drm->mode_config.max_width = 4096;
	drm->mode_config.max_height = 4096;

	drm_mode_config_init(drm);

	ret = drm_vblank_init(drm, MAX_CRTC);
	if (ret)
		goto err_kms;

	dev_set_drvdata(dev, drm);

	/* Now try and bind all our sub-components */
	ret = component_bind_all(dev, drm);
	if (ret)
		goto err_vblank;

	drm_mode_config_reset(drm);

	/*
	 * All components are now initialised, so setup the fb helper.
	 * The fb helper takes copies of key hardware information, so the
	 * crtcs/connectors/encoders must not change after this point.
	 */
#if IS_ENABLED(CONFIG_DRM_FBDEV_EMULATION)
	if (legacyfb_depth != 16 && legacyfb_depth != 32) {
		dev_warn(dev, "Invalid legacyfb_depth.  Defaulting to 16bpp\n");
		legacyfb_depth = 16;
	}
	imxdrm->fbhelper = drm_fbdev_cma_init(drm, legacyfb_depth,
				drm->mode_config.num_crtc, MAX_CRTC);
	if (IS_ERR(imxdrm->fbhelper)) {
		ret = PTR_ERR(imxdrm->fbhelper);
		imxdrm->fbhelper = NULL;
		goto err_unbind;
	}
#endif

	drm_kms_helper_poll_init(drm);

	ret = drm_dev_register(drm, 0);
	if (ret)
		goto err_fbhelper;

	return 0;

err_fbhelper:
	drm_kms_helper_poll_fini(drm);
#if IS_ENABLED(CONFIG_DRM_FBDEV_EMULATION)
	if (imxdrm->fbhelper)
		drm_fbdev_cma_fini(imxdrm->fbhelper);
err_unbind:
#endif
	component_unbind_all(drm->dev, drm);
err_vblank:
	drm_vblank_cleanup(drm);
err_kms:
	drm_mode_config_cleanup(drm);
err_unref:
	drm_dev_unref(drm);

	return ret;
}

static void imx_drm_unbind(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct imx_drm_device *imxdrm = drm->dev_private;

	drm_dev_unregister(drm);

	drm_kms_helper_poll_fini(drm);

	if (imxdrm->fbhelper)
		drm_fbdev_cma_fini(imxdrm->fbhelper);

	drm_mode_config_cleanup(drm);

	component_unbind_all(drm->dev, drm);
	dev_set_drvdata(dev, NULL);

	drm_dev_unref(drm);
}

static const struct component_master_ops imx_drm_ops = {
	.bind = imx_drm_bind,
	.unbind = imx_drm_unbind,
};

static int imx_drm_platform_probe(struct platform_device *pdev)
{

	struct component_match *match = NULL;
	int ret;

	ret = add_display_components(&pdev->dev, &match);
	if (ret)
		return ret;

	add_dpu_bliteng_components(&pdev->dev, &match);

	ret = component_master_add_with_match(&pdev->dev, &imx_drm_ops, match);


	if (!ret)
		ret = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32));

	return ret;
}

static int imx_drm_platform_remove(struct platform_device *pdev)
{
	component_master_del(&pdev->dev, &imx_drm_ops);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int imx_drm_suspend(struct device *dev)
{
	struct drm_device *drm_dev = dev_get_drvdata(dev);
	struct imx_drm_device *imxdrm;

	/* The drm_dev is NULL before .load hook is called */
	if (drm_dev == NULL)
		return 0;

	drm_kms_helper_poll_disable(drm_dev);

	imxdrm = drm_dev->dev_private;
	imxdrm->state = drm_atomic_helper_suspend(drm_dev);
	if (IS_ERR(imxdrm->state)) {
		drm_kms_helper_poll_enable(drm_dev);
		return PTR_ERR(imxdrm->state);
	}

	return 0;
}

static int imx_drm_resume(struct device *dev)
{
	struct drm_device *drm_dev = dev_get_drvdata(dev);
	struct imx_drm_device *imx_drm;

	if (drm_dev == NULL)
		return 0;

	imx_drm = drm_dev->dev_private;
	drm_atomic_helper_resume(drm_dev, imx_drm->state);
	drm_kms_helper_poll_enable(drm_dev);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(imx_drm_pm_ops, imx_drm_suspend, imx_drm_resume);

static const struct of_device_id imx_drm_dt_ids[] = {
	{ .compatible = "fsl,imx-display-subsystem", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, imx_drm_dt_ids);

static struct platform_driver imx_drm_pdrv = {
	.probe		= imx_drm_platform_probe,
	.remove		= imx_drm_platform_remove,
	.driver		= {
		.name	= "imx-drm",
		.pm	= &imx_drm_pm_ops,
		.of_match_table = imx_drm_dt_ids,
	},
};
module_platform_driver(imx_drm_pdrv);

MODULE_AUTHOR("Sascha Hauer <s.hauer@pengutronix.de>");
MODULE_DESCRIPTION("i.MX drm driver core");
MODULE_LICENSE("GPL");
