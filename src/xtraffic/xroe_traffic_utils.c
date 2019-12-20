// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 Xilinx, Inc.
 *
 * Vasileios Bimpikas <vasileios.bimpikas@xilinx.com>
 */
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include "xroe_traffic.h"

/*
 * Declare the root kernel object. This is also declared as an exter in the 
 * main header file so it can be picked up by the calls that create the 
 * sysfs structure
 */
struct kobject *root_xroe_kobj;

/**
 * xroe_sysfs_init - Creates the xroe sysfs directory and entries
 *
 * Return: 0 on success, negative value in case of failure to
 * create the sysfs group
 *
 * Creates the xroe sysfs directory and entries, as well as the
 * subdirectories for deframer, cfg etc
 */
int xroe_sysfs_init(void)
{
	int ret;

	root_xroe_kobj = kobject_create_and_add("traffic", kernel_kobj);
	if (!root_xroe_kobj)
		return -ENOMEM;
	// ret = sysfs_create_group(root_xroe_kobj, *main_groups);
	ret = xroe_sysfs_config_init();

	return ret;
}

/**
 * xroe_sysfs_exit - Deletes the xroe sysfs directory and entries
 *
 * Deletes the xroe sysfs directory and entries
 *
 */
void xroe_sysfs_exit(void)
{
	kobject_del(root_xroe_kobj);
}

/**
 * utils_write32withmask - Writes a masked 32-bit value
 * @working_address:	The starting address to write
 * @value:			The value to be written
 * @mask:			The mask to be used
 * @offset:			The offset from the provided starting address
 *
 * Writes a 32-bit value to the provided address with the input mask
 *
 * Return: 0 on success
 */
int utils_write32withmask(void __iomem *working_address, u32 value,
			  u32 mask, u32 offset)
{
	u32 read_register_value = 0;
	u32 register_value_to_write = 0;
	u32 delta = 0, buffer = 0;

	read_register_value = ioread32(working_address);
	buffer = (value << offset);
	register_value_to_write = read_register_value & ~mask;
	delta = buffer & mask;
	register_value_to_write |= delta;
	iowrite32(register_value_to_write, working_address);
	return 0;
}
