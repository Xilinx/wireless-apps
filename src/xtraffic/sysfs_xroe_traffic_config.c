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

	ret = kstrtouint(buff, 16, &input_value);
	if (ret)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
					   xroe_read_attr->addr);
	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}


static XROE_RO_REG_ATTR(radio_id,                  RADIO_ID_OFFSET,
                                                   RADIO_ID_MASK,
                                                   RADIO_ID_ADDR);

static XROE_RW_REG_ATTR(radio_timeout_enable,      RADIO_TIMEOUT_ENABLE_OFFSET,
                                                   RADIO_TIMEOUT_ENABLE_MASK,
                                                   RADIO_TIMEOUT_ENABLE_ADDR);

static XROE_RO_REG_ATTR(radio_timeout_status,      RADIO_TIMEOUT_STATUS_OFFSET,
                                                   RADIO_TIMEOUT_STATUS_MASK,
                                                   RADIO_TIMEOUT_STATUS_ADDR);

static XROE_RW_REG_ATTR(radio_timeout_value,       RADIO_TIMEOUT_VALUE_OFFSET,
                                                   RADIO_TIMEOUT_VALUE_MASK,
                                                   RADIO_TIMEOUT_VALUE_ADDR);

static XROE_RW_REG_ATTR(radio_gpio_cdc_ledmode2,   RADIO_GPIO_CDC_LEDMODE2_OFFSET,
                                                   RADIO_GPIO_CDC_LEDMODE2_MASK,
                                                   RADIO_GPIO_CDC_LEDMODE2_ADDR);

static XROE_RW_REG_ATTR(radio_gpio_cdc_ledgpio,    RADIO_GPIO_CDC_LEDGPIO_OFFSET,
                                                   RADIO_GPIO_CDC_LEDGPIO_MASK,
                                                   RADIO_GPIO_CDC_LEDGPIO_ADDR);

static XROE_RO_REG_ATTR(radio_gpio_cdc_dipstatus,  RADIO_GPIO_CDC_DIPSTATUS_OFFSET,
                                                   RADIO_GPIO_CDC_DIPSTATUS_MASK,
                                                   RADIO_GPIO_CDC_DIPSTATUS_ADDR);

static XROE_RW_REG_ATTR(radio_sw_trigger,          RADIO_SW_TRIGGER_OFFSET,
                                                   RADIO_SW_TRIGGER_MASK,
                                                   RADIO_SW_TRIGGER_ADDR);

static XROE_RW_REG_ATTR(radio_cdc_enable,          RADIO_CDC_ENABLE_OFFSET,
                                                   RADIO_CDC_ENABLE_MASK,
                                                   RADIO_CDC_ENABLE_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_error,           RADIO_CDC_ERROR_OFFSET,
                                                   RADIO_CDC_ERROR_MASK,
                                                   RADIO_CDC_ERROR_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_status,          RADIO_CDC_STATUS_OFFSET,
                                                   RADIO_CDC_STATUS_MASK,
                                                   RADIO_CDC_STATUS_ADDR);

static XROE_RW_REG_ATTR(radio_cdc_loopback,        RADIO_CDC_LOOPBACK_OFFSET,
                                                   RADIO_CDC_LOOPBACK_MASK,
                                                   RADIO_CDC_LOOPBACK_ADDR);

static XROE_RW_REG_ATTR(radio_source_enable,       RADIO_SOURCE_ENABLE_OFFSET,
                                                   RADIO_SOURCE_ENABLE_MASK,
                                                   RADIO_SOURCE_ENABLE_ADDR);

static XROE_RW_REG_ATTR(radio_sink_enable,         RADIO_SINK_ENABLE_OFFSET,
                                                   RADIO_SINK_ENABLE_MASK,
                                                   RADIO_SINK_ENABLE_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_error_31_0,      RADIO_CDC_ERROR_31_0_OFFSET,
                                                   RADIO_CDC_ERROR_31_0_MASK,
                                                   RADIO_CDC_ERROR_31_0_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_error_63_32,     RADIO_CDC_ERROR_63_32_OFFSET,
                                                   RADIO_CDC_ERROR_63_32_MASK,
                                                   RADIO_CDC_ERROR_63_32_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_error_95_64,     RADIO_CDC_ERROR_95_64_OFFSET,
                                                   RADIO_CDC_ERROR_95_64_MASK,
                                                   RADIO_CDC_ERROR_95_64_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_error_127_96,    RADIO_CDC_ERROR_127_96_OFFSET,
                                                   RADIO_CDC_ERROR_127_96_MASK,
                                                   RADIO_CDC_ERROR_127_96_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_status_31_0,     RADIO_CDC_STATUS_31_0_OFFSET,
                                                   RADIO_CDC_STATUS_31_0_MASK,
                                                   RADIO_CDC_STATUS_31_0_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_status_63_32,    RADIO_CDC_STATUS_63_32_OFFSET,
                                                   RADIO_CDC_STATUS_63_32_MASK,
                                                   RADIO_CDC_STATUS_63_32_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_status_95_64,    RADIO_CDC_STATUS_95_64_OFFSET,
                                                   RADIO_CDC_STATUS_95_64_MASK,
                                                   RADIO_CDC_STATUS_95_64_ADDR);

static XROE_RO_REG_ATTR(radio_cdc_status_127_96,   RADIO_CDC_STATUS_127_96_OFFSET,
                                                   RADIO_CDC_STATUS_127_96_MASK,
                                                   RADIO_CDC_STATUS_127_96_ADDR);

static XROE_RW_REG_ATTR(radio_app_scratch_reg_0,   RADIO_APP_SCRATCH_REG_0_OFFSET,
                                                   RADIO_APP_SCRATCH_REG_0_MASK,
                                                   RADIO_APP_SCRATCH_REG_0_ADDR);

static XROE_RW_REG_ATTR(radio_app_scratch_reg_1,   RADIO_APP_SCRATCH_REG_1_OFFSET,
                                                   RADIO_APP_SCRATCH_REG_1_MASK,
                                                   RADIO_APP_SCRATCH_REG_1_ADDR);

static XROE_RW_REG_ATTR(radio_app_scratch_reg_2,   RADIO_APP_SCRATCH_REG_2_OFFSET,
                                                   RADIO_APP_SCRATCH_REG_2_MASK,
                                                   RADIO_APP_SCRATCH_REG_2_ADDR);

static XROE_RW_REG_ATTR(radio_app_scratch_reg_3,   RADIO_APP_SCRATCH_REG_3_OFFSET,
                                                   RADIO_APP_SCRATCH_REG_3_MASK,
                                                   RADIO_APP_SCRATCH_REG_3_ADDR);

static XROE_RW_REG_ATTR(fram_packet_data_size,     FRAM_PACKET_DATA_SIZE_OFFSET,
                                                   FRAM_PACKET_DATA_SIZE_MASK,
                                                   FRAM_PACKET_DATA_SIZE_ADDR);

static XROE_RW_REG_ATTR(fram_pause_data_size,      FRAM_PAUSE_DATA_SIZE_OFFSET,
                                                   FRAM_PAUSE_DATA_SIZE_MASK,
                                                   FRAM_PAUSE_DATA_SIZE_ADDR);

static struct attribute *main_attrs[] = {
    &attr_radio_id.attr.attr,
    &attr_radio_timeout_enable.attr.attr,
    &attr_radio_timeout_status.attr.attr,
    &attr_radio_timeout_value.attr.attr,
    &attr_radio_gpio_cdc_ledmode2.attr.attr,
    &attr_radio_gpio_cdc_ledgpio.attr.attr,
    &attr_radio_gpio_cdc_dipstatus.attr.attr,
    &attr_radio_sw_trigger.attr.attr,
    &attr_radio_cdc_enable.attr.attr,
    &attr_radio_cdc_error.attr.attr,
    &attr_radio_cdc_status.attr.attr,
    &attr_radio_cdc_loopback.attr.attr,
    &attr_radio_source_enable.attr.attr,
    &attr_radio_sink_enable.attr.attr,
    &attr_radio_cdc_error_31_0.attr.attr,
    &attr_radio_cdc_error_63_32.attr.attr,
    &attr_radio_cdc_error_95_64.attr.attr,
    &attr_radio_cdc_error_127_96.attr.attr,
    &attr_radio_cdc_status_31_0.attr.attr,
    &attr_radio_cdc_status_63_32.attr.attr,
    &attr_radio_cdc_status_95_64.attr.attr,
    &attr_radio_cdc_status_127_96.attr.attr,
    &attr_radio_app_scratch_reg_0.attr.attr,
    &attr_radio_app_scratch_reg_1.attr.attr,
    &attr_radio_app_scratch_reg_2.attr.attr,
    &attr_radio_app_scratch_reg_3.attr.attr,
    &attr_fram_packet_data_size.attr.attr,
    &attr_fram_pause_data_size.attr.attr,

    NULL,
};


/* This is a MACRO call that will create main_groups
*/
ATTRIBUTE_GROUPS(main);

/*
 * Function to add the SYSFS entries
 */
int xroe_sysfs_config_init(void)
{
	int ret;

    /* Note, root_xroe_kobj is declared extern in main header file
    */
	ret = sysfs_create_group(root_xroe_kobj, *main_groups);

	return ret;
}
