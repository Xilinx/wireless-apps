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
	int cc_number = 0;
	int ret;
	void __iomem *working_address;
	u32 buffer, read_value;
	char *current_path = NULL;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path, "/kernel/xroe/oran/cfg/cc_%d/", &cc_number);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_CC * cc_number)));

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
	int cc_number = 0;
	char *current_path = NULL;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	ret = kstrtouint(buff, 16, &input_value);
	if (ret)
		return ret;
	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path, "/kernel/xroe/oran/cfg/cc_%d/", &cc_number);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_CC * cc_number)));
	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}

static XROE_RW_REG_ATTR(enable, ORAN_CC_ENABLE_OFFSET,
			ORAN_CC_ENABLE_MASK,
			ORAN_CC_ENABLE_ADDR);
static XROE_RW_REG_ATTR(numerology, ORAN_CC_NUMEROLOGY_OFFSET,
			ORAN_CC_NUMEROLOGY_MASK,
			ORAN_CC_NUMEROLOGY_ADDR);
static XROE_RW_REG_ATTR(numrbs, ORAN_CC_NUMRBS_OFFSET,
			ORAN_CC_NUMRBS_MASK,
			ORAN_CC_NUMRBS_ADDR);
static XROE_RW_REG_ATTR(reload, ORAN_CC_RELOAD_OFFSET,
			ORAN_CC_RELOAD_MASK,
			ORAN_CC_RELOAD_ADDR);
static XROE_RW_REG_ATTR(dl_ctrl_offsets, ORAN_CC_DL_CTRL_OFFSETS_OFFSET,
			ORAN_CC_DL_CTRL_OFFSETS_MASK,
			ORAN_CC_DL_CTRL_OFFSETS_ADDR);
static XROE_RW_REG_ATTR(dl_ctrl_unrolled_offsets,
			ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_OFFSET,
			ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_MASK,
			ORAN_CC_DL_CTRL_UNROLLED_OFFSETS_ADDR);
static XROE_RW_REG_ATTR(ul_ctrl_offsets, ORAN_CC_UL_CTRL_OFFSETS_OFFSET,
			ORAN_CC_UL_CTRL_OFFSETS_MASK,
			ORAN_CC_UL_CTRL_OFFSETS_ADDR);
static XROE_RW_REG_ATTR(ul_ctrl_unrolled_offsets,
			ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_OFFSET,
			ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_MASK,
			ORAN_CC_UL_CTRL_UNROLLED_OFFSETS_ADDR);
static XROE_RW_REG_ATTR(dl_data_sym_start_index,
			ORAN_CC_DL_DATA_SYM_START_INDEX_OFFSET,
			ORAN_CC_DL_DATA_SYM_START_INDEX_MASK,
			ORAN_CC_DL_DATA_SYM_START_INDEX_ADDR);
static XROE_RW_REG_ATTR(dl_data_sym_num_index,
			ORAN_CC_DL_DATA_SYM_NUM_INDEX_OFFSET,
			ORAN_CC_DL_DATA_SYM_NUM_INDEX_MASK,
			ORAN_CC_DL_DATA_SYM_NUM_INDEX_ADDR);
static XROE_RW_REG_ATTR(ul_ud_iq_width, ORAN_CC_UL_UD_IQ_WIDTH_OFFSET,
			ORAN_CC_UL_UD_IQ_WIDTH_MASK,
			ORAN_CC_UL_UD_IQ_WIDTH_ADDR);
static XROE_RW_REG_ATTR(ul_ud_comp_meth, ORAN_CC_UL_UD_COMP_METH_OFFSET,
			ORAN_CC_UL_UD_COMP_METH_MASK,
			ORAN_CC_UL_UD_COMP_METH_ADDR);
static XROE_RW_REG_ATTR(ul_mplane_udcomp_param,
			ORAN_CC_UL_MPLANE_UDCOMP_PARAM_OFFSET,
			ORAN_CC_UL_MPLANE_UDCOMP_PARAM_MASK,
			ORAN_CC_UL_MPLANE_UDCOMP_PARAM_ADDR);
static XROE_RW_REG_ATTR(dl_ud_iq_width, ORAN_CC_DL_UD_IQ_WIDTH_OFFSET,
			ORAN_CC_DL_UD_IQ_WIDTH_MASK,
			ORAN_CC_DL_UD_IQ_WIDTH_ADDR);
static XROE_RW_REG_ATTR(dl_ud_comp_meth, ORAN_CC_DL_UD_COMP_METH_OFFSET,
			ORAN_CC_DL_UD_COMP_METH_MASK,
			ORAN_CC_DL_UD_COMP_METH_ADDR);
static XROE_RW_REG_ATTR(dl_mplane_udcomp_param,
			ORAN_CC_DL_MPLANE_UDCOMP_PARAM_OFFSET,
			ORAN_CC_DL_MPLANE_UDCOMP_PARAM_MASK,
			ORAN_CC_DL_MPLANE_UDCOMP_PARAM_ADDR);

static struct attribute *cc_attrs[] = {
	&attr_enable.attr.attr,
	&attr_numerology.attr.attr,
	&attr_numrbs.attr.attr,
	&attr_reload.attr.attr,
	&attr_dl_ctrl_offsets.attr.attr,
	&attr_dl_ctrl_unrolled_offsets.attr.attr,
	&attr_ul_ctrl_offsets.attr.attr,
	&attr_ul_ctrl_unrolled_offsets.attr.attr,
	&attr_dl_data_sym_start_index.attr.attr,
	&attr_dl_data_sym_num_index.attr.attr,
	&attr_ul_ud_iq_width.attr.attr,
	&attr_ul_ud_comp_meth.attr.attr,
	&attr_ul_mplane_udcomp_param.attr.attr,
	&attr_dl_ud_iq_width.attr.attr,
	&attr_dl_ud_comp_meth.attr.attr,
	&attr_dl_mplane_udcomp_param.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(cc);

/**
 * xroe_sysfs_cfg_init - Creates the "cfg" sysfs directory and entries
 *
 * Return: 0 on success, negative value in case of failure to
 * create the sysfs group
 *
 * Creates the "cfg" sysfs directory and entries, as well as the cc_#
 * subdirectories
 */
int xroe_sysfs_cfg_init(void)
{
	int ret;
	int i;
	char cc_dir_name[5];
	struct kobject *kobj_dir_cfg;
	struct kobject *kobj_dir_oran;
	struct kobject *kobj_cc[MAX_NUM_ORAN_CC];

	kobj_dir_oran = kobject_create_and_add("oran", root_xroe_kobj);
	if (!kobj_dir_oran)
		return -ENOMEM;

	kobj_dir_cfg = kobject_create_and_add("cfg", kobj_dir_oran);
	if (!kobj_dir_cfg)
		return -ENOMEM;
	for (i = 0; i < MAX_NUM_ORAN_CC; i++) {
		snprintf(cc_dir_name, sizeof(cc_dir_name),
			 "cc_%d", i);
		kobj_cc[i] = kobject_create_and_add(cc_dir_name, kobj_dir_cfg);
		if (!kobj_cc[i])
			return -ENOMEM;
		ret = sysfs_create_group(kobj_cc[i], *cc_groups);
		if (ret)
			kobject_put(kobj_cc[i]);
	}

	return ret;
}
