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

/**
 * xroe_reg_show - The generic show function for stats entries
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
	ret = sscanf(current_path, "/kernel/xroe/stats/eth_port_%d/", &port);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_STATS * port)));
	buffer = ioread32(working_address);
	read_value = (buffer & xroe_read_attr->mask) >> xroe_read_attr->offset;

	return sprintf(buff, "0x%x\n", read_value);
}

static XROE_RO_REG_ATTR(total_rx_good_pkt,
			STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_MASK,
			STATS_ETH_STATS_TOTAL_RX_GOOD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_pkt,
			STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_MASK,
			STATS_ETH_STATS_TOTAL_RX_BAD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_fcs,
			STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_OFFSET,
			STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_MASK,
			STATS_ETH_STATS_TOTAL_RX_BAD_FCS_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_user_pkt,
			STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_OFFSET,
			STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_MASK,
			STATS_ETH_STATS_USER_DATA_RX_PACKETS_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_good_user_pkt,
			STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_MASK,
			STATS_ETH_STATS_USER_DATA_RX_GOOD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_user_pkt,
			STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_MASK,
			STATS_ETH_STATS_USER_DATA_RX_BAD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_user_fcs,
			STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_OFFSET,
			STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_MASK,
			STATS_ETH_STATS_USER_DATA_RX_BAD_FCS_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_user_ctrl_pkt,
			STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_OFFSET,
			STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_MASK,
			STATS_ETH_STATS_USER_CTRL_RX_PACKETS_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_good_user_ctrl_pkt,
			STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_MASK,
			STATS_ETH_STATS_USER_CTRL_RX_GOOD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_user_ctrl_pkt,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_OFFSET,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_MASK,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_PKT_CNT_ADDR);
static XROE_RO_REG_ATTR(total_rx_bad_user_ctrl_fcs,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_OFFSET,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_MASK,
			STATS_ETH_STATS_USER_CTRL_RX_BAD_FCS_CNT_ADDR);
static XROE_RO_REG_ATTR(rx_user_pkt_rate,
			STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_OFFSET,
			STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_MASK,
			STATS_ETH_STATS_USER_DATA_RX_PKTS_RATE_ADDR);
static XROE_RO_REG_ATTR(rx_user_ctrl_pkt_rate,
			STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_OFFSET,
			STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_MASK,
			STATS_ETH_STATS_USER_CTRL_RX_PKTS_RATE_ADDR);

static struct attribute *stats_attrs[] = {
	&attr_total_rx_good_pkt.attr.attr,
	&attr_total_rx_bad_pkt.attr.attr,
	&attr_total_rx_bad_fcs.attr.attr,
	&attr_total_rx_user_pkt.attr.attr,
	&attr_total_rx_good_user_pkt.attr.attr,
	&attr_total_rx_bad_user_pkt.attr.attr,
	&attr_total_rx_bad_user_fcs.attr.attr,
	&attr_total_rx_user_ctrl_pkt.attr.attr,
	&attr_total_rx_good_user_ctrl_pkt.attr.attr,
	&attr_total_rx_bad_user_ctrl_pkt.attr.attr,
	&attr_total_rx_bad_user_ctrl_fcs.attr.attr,
	&attr_rx_user_pkt_rate.attr.attr,
	&attr_rx_user_ctrl_pkt_rate.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(stats);

/**
 * xroe_sysfs_stats_init - Creates the xroe sysfs "stats" subdirectory & entries
 *
 * Return: 0 on success, negative value in case of failure to
 * create the sysfs group
 *
 * Creates the xroe sysfs "stats" subdirectory and entries under "xroe"
 */
int xroe_sysfs_stats_init(void)
{
	int ret;
	int i;
	char eth_port_dir_name[11];
	struct kobject *kobj_dir_stats;
	struct kobject *kobj_dir_eth_ports[MAX_NUM_ETH_PORTS];

	kobj_dir_stats = kobject_create_and_add("stats", root_xroe_kobj);
	if (!kobj_dir_stats)
		return -ENOMEM;
	for (i = 0; i < MAX_NUM_ETH_PORTS; i++) {
		snprintf(eth_port_dir_name, sizeof(eth_port_dir_name),
			 "eth_port_%d", i);
		kobj_dir_eth_ports[i] =
		kobject_create_and_add(eth_port_dir_name, kobj_dir_stats);
		if (!kobj_dir_eth_ports[i])
			return -ENOMEM;
		ret = sysfs_create_group(kobj_dir_eth_ports[i], *stats_groups);
		if (ret)
			kobject_put(kobj_dir_eth_ports[i]);
	}

	return ret;
}
