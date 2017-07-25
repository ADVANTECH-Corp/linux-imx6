/**
 * core.h - Cadence USB3 DRD Controller Core header file
 *
 * Copyright 2017 NXP
 *
 * Authors: Peter Chen <peter.chen@nxp.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2  of
 * the License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DRIVERS_USB_CDNS3_CORE_H
#define __DRIVERS_USB_CDNS3_CORE_H

struct cdns3;
enum cdns3_roles {
	CDNS3_ROLE_HOST = 0,
	CDNS3_ROLE_GADGET,
	CDNS3_ROLE_END,
};

/**
 * struct cdns3_role_driver - host/gadget role driver
 * @start: start this role
 * @stop: stop this role
 * @irq: irq handler for this role
 * @name: role name string (host/gadget)
 */
struct cdns3_role_driver {
	int (*start)(struct cdns3 *);
	void (*stop)(struct cdns3 *);
	irqreturn_t (*irq)(struct cdns3 *);
	const char *name;
};

#define CDNS3_NUM_OF_CLKS	5
/**
 * struct cdns3 - Representation of Cadence USB3 DRD controller.
 * @dev: pointer to Cadence device struct
 * @xhci_regs: pointer to base of xhci registers
 * @xhci_res: the resource for xhci
 * @dev_regs: pointer to base of dev registers
 * @none_core_regs: pointer to base of nxp wrapper registers
 * @phy_regs: pointer to base of phy registers
 * @irq: irq number for controller
 * @roles: array of supported roles for this controller
 * @role: current role
 * @host_dev: the child host device pointer for cdns3 core
 * @gadget_dev: the child gadget device pointer for cdns3 core
 * @usbphy: usbphy for this controller
 * @cdns3_clks: Clock pointer array for cdns3 core
 * @extcon: Type-C extern connector
 * @extcon_nb: notifier block for Type-C extern connector
 * @role_switch_wq: work queue item for role switch
 */
struct cdns3 {
	struct device *dev;
	void __iomem *xhci_regs;
	struct resource *xhci_res;
	struct usbss_dev_register_block_type __iomem *dev_regs;
	void __iomem *none_core_regs;
	int irq;
	struct cdns3_role_driver *roles[CDNS3_ROLE_END];
	enum cdns3_roles role;
	struct device *host_dev;
	struct device *gadget_dev;
	void __iomem *phy_regs;
	struct usb_phy *usbphy;
	struct clk *cdns3_clks[CDNS3_NUM_OF_CLKS];
	struct extcon_dev *extcon;
	struct notifier_block extcon_nb;
	struct work_struct role_switch_wq;
};

static inline struct cdns3_role_driver *cdns3_role(struct cdns3 *cdns)
{
	WARN_ON(cdns->role >= CDNS3_ROLE_END || !cdns->roles[cdns->role]);
	return cdns->roles[cdns->role];
}

static inline int cdns3_role_start(struct cdns3 *cdns, enum cdns3_roles role)
{
	int ret;

	if (role >= CDNS3_ROLE_END)
		return 0;

	if (!cdns->roles[role])
		return -ENXIO;

	ret = cdns->roles[role]->start(cdns);
	if (!ret)
		cdns->role = role;

	return ret;
}

static inline void cdns3_role_stop(struct cdns3 *cdns)
{
	enum cdns3_roles role = cdns->role;

	if (role == CDNS3_ROLE_END)
		return;

	cdns->role = CDNS3_ROLE_END;

	cdns->roles[role]->stop(cdns);
}

#endif /* __DRIVERS_USB_CDNS3_CORE_H */
