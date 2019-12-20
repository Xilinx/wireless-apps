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
#include "xroe_framer.h"

#define XROE_RO_REG_ATTR(_name, _reg_offset, _reg_mask, _reg_addr) \
struct xroe_reg_attribute attr_##_name = \
	{ __ATTR(_name, 0444, xroe_reg_show, NULL), _reg_offset, _reg_mask, \
	 _reg_addr}
#define XROE_RW_REG_ATTR(_name, _reg_offset, _reg_mask, _reg_addr) \
struct xroe_reg_attribute attr_##_name = \
	{ __ATTR(_name, 0660, xroe_reg_show, xroe_reg_store), _reg_offset, \
	 _reg_mask, _reg_addr}
   
#define CFG_REG_ARRAY_SIZE 15

/**
 * xroe_reg_show - The generic show function
 * @kobj:	The kernel object of the entry
 * @attr:	The attributes of the kernel object
 * @buff:	The buffer containing the revision string
 *
 * Prints the requested register's content in hex. The address, offset & mask
 * of the register are passed to the function using the xroe_reg_attribute
 * struct, during the initialisation of each attribute. This is done using
 * container_of() at the beginning of the function
 *
 * Return: the number of characters printed
 */
static ssize_t xroe_reg_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buff)
{
	void __iomem *working_address;
	u32 buffer, read_value;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	working_address = (void __iomem *)(lp->base_addr +
					   xroe_read_attr->addr);
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
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	ret = kstrtouint(buff, CFG_REG_ARRAY_SIZE, &input_value);
	if (ret)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
					   xroe_read_attr->addr);
	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}

static XROE_RO_REG_ATTR(version_major, CFG_MAJOR_REVISION_OFFSET,
			CFG_MAJOR_REVISION_MASK,
			CFG_MAJOR_REVISION_ADDR);
static XROE_RO_REG_ATTR(version_minor, CFG_MINOR_REVISION_OFFSET,
			CFG_MINOR_REVISION_MASK,
			CFG_MINOR_REVISION_ADDR);
static XROE_RO_REG_ATTR(version_revision, CFG_VERSION_REVISION_OFFSET,
			CFG_VERSION_REVISION_MASK,
			CFG_VERSION_REVISION_ADDR);
static XROE_RW_REG_ATTR(enable, CFG_MASTER_INT_ENABLE_OFFSET,
			CFG_MASTER_INT_ENABLE_MASK,
			CFG_MASTER_INT_ENABLE_ADDR);
static XROE_RW_REG_ATTR(framer_disable, FRAM_DISABLE_OFFSET,
			FRAM_DISABLE_MASK,
			FRAM_DISABLE_ADDR);
static XROE_RW_REG_ATTR(deframer_disable, DEFM_DISABLE_OFFSET,
			DEFM_DISABLE_MASK,
			DEFM_DISABLE_ADDR);
static XROE_RW_REG_ATTR(xxv_reset, CFG_USER_RW_OUT_OFFSET,
			CFG_USER_RW_OUT_MASK,
			CFG_USER_RW_OUT_ADDR);
static XROE_RO_REG_ATTR(config_no_of_fram_ants, CFG_CONFIG_NO_OF_FRAM_ANTS_OFFSET,
			CFG_CONFIG_NO_OF_FRAM_ANTS_MASK,
			CFG_CONFIG_NO_OF_FRAM_ANTS_ADDR);
static XROE_RO_REG_ATTR(config_no_of_defm_ants, CFG_CONFIG_NO_OF_DEFM_ANTS_OFFSET,
			CFG_CONFIG_NO_OF_DEFM_ANTS_MASK,
			CFG_CONFIG_NO_OF_DEFM_ANTS_ADDR);
static XROE_RO_REG_ATTR(config_no_of_eth_ports, CFG_CONFIG_NO_OF_ETH_PORTS_OFFSET,
			CFG_CONFIG_NO_OF_ETH_PORTS_MASK,
			CFG_CONFIG_NO_OF_ETH_PORTS_ADDR);
static XROE_RO_REG_ATTR(config_eth_speed, CFG_CONFIG_ETH_SPEED_OFFSET,
			CFG_CONFIG_ETH_SPEED_MASK,
			CFG_CONFIG_ETH_SPEED_ADDR);
static XROE_RO_REG_ATTR(config_xran_support_mode, CFG_CONFIG_XRAN_SUPPORT_MODE_OFFSET,
			CFG_CONFIG_XRAN_SUPPORT_MODE_MASK,
			CFG_CONFIG_XRAN_SUPPORT_MODE_ADDR);
static XROE_RO_REG_ATTR(config_xran_max_cc, CFG_CONFIG_XRAN_MAX_CC_OFFSET,
			CFG_CONFIG_XRAN_MAX_CC_MASK,
			CFG_CONFIG_XRAN_MAX_CC_ADDR);
static XROE_RO_REG_ATTR(config_xran_max_dl_symbols, CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_OFFSET,
			CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_MASK,
			CFG_CONFIG_XRAN_MAX_DL_SYMBOLS_ADDR);

static struct attribute *main_attrs[] = {
	&attr_version_major.attr.attr,
	&attr_version_minor.attr.attr,
	&attr_version_revision.attr.attr,
	&attr_enable.attr.attr,
	&attr_framer_disable.attr.attr,
	&attr_deframer_disable.attr.attr,
	&attr_xxv_reset.attr.attr,
	&attr_config_no_of_fram_ants.attr.attr,
	&attr_config_no_of_defm_ants.attr.attr,
	&attr_config_no_of_eth_ports.attr.attr,
	&attr_config_eth_speed.attr.attr,
	&attr_config_xran_support_mode.attr.attr,
	&attr_config_xran_max_cc.attr.attr,
	&attr_config_xran_max_dl_symbols.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(main);

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

	root_xroe_kobj = kobject_create_and_add("xroe", kernel_kobj);
	if (!root_xroe_kobj)
		return -ENOMEM;
	ret = sysfs_create_group(root_xroe_kobj, *main_groups);
	if (ret)
		kobject_put(root_xroe_kobj);
	ret = xroe_sysfs_stats_init();
	if (ret)
		return ret;
	ret = xroe_sysfs_shared_init();
	if (ret)
		return ret;
	ret = xroe_sysfs_defm_init();
	if (ret)
		return ret;
	ret = xroe_sysfs_cfg_init();
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
