/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2018 Xilinx, Inc.
 *
 * Vasileios Bimpikas <vasileios.bimpikas@xilinx.com>
 */
#include "roe_radio_ctrl.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <uapi/linux/stat.h> /* S_IRUSR, S_IWUSR */

/* TODO: to be made static as well, so that multiple instances can be used. As
 * of now, the following 3 structures are shared among the multiple
 * source files
 */
extern struct traffic_local *lp;
extern struct kobject *root_xroe_kobj;

struct traffic_local {
	int irq;
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;
};

struct xroe_reg_attribute {
	struct kobj_attribute attr;
	u32 offset;
	u32 mask;
	u32 addr;
};

int xroe_sysfs_init(void);
int xroe_sysfs_config_init(void);
void xroe_sysfs_exit(void);

int utils_write32withmask(void __iomem *working_address, u32 value, u32 mask,
			  u32 offset);
