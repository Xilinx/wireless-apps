// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Xilinx, Inc.
 *
 * Vasileios Bimpikas <vasileios.bimpikas@xilinx.com>
 */

#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include "xroe_framer.h"

#define XROE_RO_REG_ATTR(_name, _reg_offset, _reg_mask, _reg_addr) \
struct xroe_reg_attribute attr_##_name = \
	{ __ATTR(_name, 0444, xroe_reg_show, NULL), _reg_offset, _reg_mask, \
	 _reg_addr}
#define XROE_RW_REG_ATTR(_name, _reg_offset, _reg_mask, _reg_addr) \
struct xroe_reg_attribute attr_##_name = \
	{ __ATTR(_name, 0660, xroe_reg_show, xroe_reg_store), _reg_offset, \
	 _reg_mask, _reg_addr}

/**
 * xroe_reg_show - The generic show function for shared entries
 * @kobj:	The kernel object of the entry
 * @attr:	The attributes of the kernel object
 * @buff:	The buffer containing the revision string
 *
 * Prints the requested register's content in hex. The address, offset & mask
 * of the register are passed to the function using the xroe_reg_attribute
 * struct, during the initialisation of each attribute. This is done using
 * container_of() at the beginning of the function. To determine the current
 * eth port, sscanf is used on the current path. The port number is taken into
 * account when calculating the working address, and the normal "show" procedure
 * continues after that
 *
 * Return: the number of characters printed
 */
static ssize_t xroe_reg_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buff)
{
	int port = 0;
	int ret;
	void __iomem *working_address;
	u32 buffer, read_value;
	char *current_path = NULL;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path, "/kernel/xroe/shared/dl_data_ptr_%d/", &port);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_DL_DATA_PTR * port)));
	buffer = ioread32(working_address);
	read_value = (buffer & xroe_read_attr->mask) >> xroe_read_attr->offset;

	return sprintf(buff, "0x%x\n", read_value);
}


/**
 * xroe_reg_store - The generic store function
 * @kobj:	The kernel object of the entry
 * @attr:	The attributes of the kernel object
 * @buff:	The buffer containing the revision string
 * @count:	The number of characters typed by the user
 *
 * Stores the input value to the requested register.The address, offset & mask
 * of the register are passed to the function using the xroe_reg_attribute
 * struct, during the initialisation of each attribute. This is done using
 * container_of() at the beginning of the function. The input value is then
 * parsed in hex and the register writing takes places after that
 *
 * Return: the number of characters the user typed
 */
static ssize_t xroe_reg_store(struct kobject *kobj, struct kobj_attribute *attr,
			      const char *buff, size_t count)
{
	int ret;
	u32 input_value;
	void __iomem *working_address;
	int port = 0;
	char *current_path = NULL;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	ret = kstrtouint(buff, 16, &input_value);
	if (ret)
		return ret;
	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path,
		     "/kernel/xroe/shared/dl_data_ptr_%d/",
		     &port);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_DL_DATA_PTR * port)));

	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}

/* Add registers
*/
static XROE_RW_REG_ATTR(cc_dl_data_unroll_offset,
			ORAN_CC_DL_DATA_UNROLL_OFFSET_OFFSET,
			ORAN_CC_DL_DATA_UNROLL_OFFSET_MASK,
			ORAN_CC_DL_DATA_UNROLL_OFFSET_ADDR);

static struct attribute *shared_attrs[] = {
	&attr_cc_dl_data_unroll_offset.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(shared);

/**
 * xroe_sysfs_shared_init - Creates the xroe sysfs "shared" subdirectory & entries
 *
 * Return: 0 on success, negative value in case of failure to
 * create the sysfs group
 *
 * Creates the xroe sysfs "shared" subdirectory and entries under "xroe"
 */
int xroe_sysfs_shared_init(void)
{
	int ret;
	int i;
	char shared_dir_name[15];
	struct kobject *kobj_dir_shared;
	struct kobject *kobj_dir_shared_mem[MAX_NUM_ORAN_DL_DATA_PTR];

	kobj_dir_shared = kobject_create_and_add("shared", root_xroe_kobj);
	if (!kobj_dir_shared)
		return -ENOMEM;
	for (i = 0; i < MAX_NUM_ORAN_DL_DATA_PTR; i++) {
		snprintf(shared_dir_name, sizeof(shared_dir_name),
			 "dl_data_ptr_%d", i);
		kobj_dir_shared_mem[i] =
		kobject_create_and_add(shared_dir_name, kobj_dir_shared);
		if (!kobj_dir_shared_mem[i])
			return -ENOMEM;
		ret = sysfs_create_group(kobj_dir_shared_mem[i], *shared_groups);
		if (ret)
			kobject_put(kobj_dir_shared_mem[i]);
	}

	return ret;
}
