/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2018 Xilinx, Inc.
 *
 * Vasileios Bimpikas <vasileios.bimpikas@xilinx.com>
 */
#include "roe_framer_ctrl.h"
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

/* TODO: Remove hardcoded value of number of Ethernet ports and read the value
 * from the device tree.
 * For now user must modify these values to reflect the HW
 */
#define MAX_NUM_ETH_PORTS         0x1
#define MAX_NUM_ORAN_CC           2
#define MAX_NUM_ORAN_DL_DATA_PTR  5

/* TODO: Move any magic numbers in the code to defines here
*/
#define ETH_FILTER_LENGTH_IN_WORDS 0x4

#define ADDR_LOOP_OFFSET_CC           0x20
#define ADDR_LOOP_OFFSET_ETH          0x100
#define ADDR_LOOP_OFFSET_STATS        0x100
#define ADDR_LOOP_OFFSET_FILTER       0x20
#define ADDR_LOOP_OFFSET_DL_DATA_PTR  0x4


/* TODO: to be made static as well, so that multiple instances can be used. As
 * of now, the following 3 structures are shared among the multiple
 * source files
 */
extern struct framer_local *lp;
extern struct kobject *root_xroe_kobj;
struct framer_local {
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
int xroe_sysfs_stats_init(void);
int xroe_sysfs_shared_init(void);
int xroe_sysfs_defm_init(void);
int xroe_sysfs_cfg_init(void);
void xroe_sysfs_exit(void);
int utils_write32withmask(void __iomem *working_address, u32 value, u32 mask,
			  u32 offset);
