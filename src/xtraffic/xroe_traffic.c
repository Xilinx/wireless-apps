// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 Xilinx, Inc.
 *
 * Vasileios Bimpikas <vasileios.bimpikas@xilinx.com>
 */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "xroe_traffic.h"

#define DRIVER_NAME "traffic"

/*
 * TODO: to be made static as well, so that multiple instances can be used. As
 * of now, the "lp" structure is shared among the multiple source files
 */
struct traffic_local *lp;
static struct platform_driver traffic_driver;
/*
 * TODO: placeholder for the IRQ once it's been implemented
 * in the traffic block
 */
static irqreturn_t traffic_irq(int irq, void *lp)
{
	return IRQ_HANDLED;
}

/**
 * traffic_probe - Probes the device tree to locate the traffic block
 * @pdev:	The structure containing the device's details
 *
 * Probes the device tree to locate the traffic block and maps it to
 * the kernel virtual memory space
 *
 * Return: 0 on success or a negative errno on error.
 */
static int traffic_probe(struct platform_device *pdev)
{
	struct resource *r_mem; /* IO mem resources */
	struct resource *r_irq;
	struct device *dev = &pdev->dev;
	int rc = 0;

	dev_dbg(dev, "Device Tree Probing\n");
	lp = devm_kzalloc(&pdev->dev, sizeof(*lp), GFP_KERNEL);
	if (!lp)
		return -ENOMEM;

	/* Get iospace for the device */
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	lp->base_addr = devm_ioremap_resource(&pdev->dev, r_mem);
	if (IS_ERR(lp->base_addr))
		return PTR_ERR(lp->base_addr);

	dev_set_drvdata(dev, lp);
	xroe_sysfs_init();
	/* Get IRQ for the device */
	/*
	 * TODO: No IRQ *yet* in the DT from the traffic block, as it's still
	 * under development. To be added once it's in the block, and also
	 * replace with platform_get_irq_byname()
	 */
	r_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (IS_ERR(r_irq)) {
		dev_info(dev, "no IRQ found\n");
		/*
		 * TODO: Return non-zero (error) code on no IRQ found.
		 * To be implemented once the IRQ is in the block
		 */
		return 0;
	}
	rc = devm_request_irq(dev, lp->irq, &traffic_irq, 0, DRIVER_NAME, lp);
	if (rc) {
		dev_err(dev, "testmodule: Could not allocate interrupt %d.\n",
			lp->irq);
		/*
		 * TODO: Return non-zero (error) code on no IRQ found.
		 * To be implemented once the IRQ is in the block
		 */
		return 0;
	}

	return rc;
}

/**
 * traffic_init - Registers the driver
 *
 * Return: 0 on success, -1 on allocation error
 *
 * Registers the traffic driver and creates character device drivers
 * for the whole block, as well as separate ones for stats and
 * radio control.
 */
static int __init traffic_init(void)
{
	int ret;

	pr_debug("XROE traffic driver init\n");

	ret = platform_driver_register(&traffic_driver);

	return ret;
}

/**
 * traffic_exit - Destroys the driver
 *
 * Unregisters the traffic driver and destroys the character
 * device driver for the whole block, as well as the separate ones
 * for stats and radio control. Returns 0 upon successful execution
 */
static void __exit traffic_exit(void)
{
	xroe_sysfs_exit();
	platform_driver_unregister(&traffic_driver);
	pr_info("XROE traffic exit\n");
}

module_init(traffic_init);
module_exit(traffic_exit);

static const struct of_device_id traffic_of_match[] = {
	{ .compatible = "xlnx,roe-traffic-2.0", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, traffic_of_match);

static struct platform_driver traffic_driver = {
	.driver = {
		/*
		 * TODO: .name shouldn't be necessary, though removing
		 * it results in kernel panic. To investigate further
		 */
		.name = DRIVER_NAME,
		.of_match_table = traffic_of_match,
	},
	.probe = traffic_probe,
};

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Xilinx Inc.");
MODULE_DESCRIPTION("traffic - Xilinx Radio over Ethernet traffic driver");
