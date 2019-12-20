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
#define XROE_MSG_FILTER_REG_ATTR(_name, _reg_offset, _reg_mask, _reg_addr) \
struct xroe_reg_attribute attr_##_name = \
	{ __ATTR(_name, 0660, xroe_msg_filter_reg_show, \
		 xroe_msg_filter_reg_store), _reg_offset, _reg_mask, _reg_addr}

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
	int port = 0;
	int ret;
	void __iomem *working_address;
	u32 buffer, read_value;
	char *current_path = NULL;
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path,
		     "/kernel/xroe/deframer/eth_config/eth_port_%d/", &port);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_ETH * port)));
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
		     "/kernel/xroe/deframer/eth_config/eth_port_%d/",
		     &port);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_ETH * port)));

	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}

/**
 * xroe_msg_filter_reg_show - The generic show function for msg_filter entries
 * @kobj:	The kernel object of the entry
 * @attr:	The attributes of the kernel object
 * @buff:	The buffer containing the revision string
 *
 * Prints the requested register's content in hex. The address, offset & mask
 * of the register are passed to the function using the xroe_reg_attribute
 * struct, during the initialisation of each attribute. This is done using
 * container_of() at the beginning of the function. To determine the current
 * eth port, sscanf is used on the current path. Another sscanf is done
 * immediately after, this time for the message filter number. After that, these
 * 2 numbers are taken into account when calculating the working address, and
 * the normal "show" procedure continues
 *
 * Return: the number of characters printed
 */
static ssize_t xroe_msg_filter_reg_show(struct kobject *kobj,
					struct kobj_attribute *attr, char *buff)
{
	int port = 0;
	int ret;
	int filter_number = 0;
	void __iomem *working_address;
	u32 buffer, read_value;
	char *current_path = NULL;
	char filter_dir[70];
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path,
		     "/kernel/xroe/deframer/eth_config/eth_port_%d/", &port);
	if (ret < 0)
		return ret;
	snprintf(filter_dir,
		 sizeof(filter_dir),
		 "/kernel/xroe/deframer/eth_config/eth_port_%d/message_filter_%%d/",
		 port);
	ret = sscanf(current_path, filter_dir, &filter_number);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_ETH * port) +
			   (ADDR_LOOP_OFFSET_FILTER * filter_number)));

	buffer = ioread32(working_address);
	read_value = (buffer & xroe_read_attr->mask) >> xroe_read_attr->offset;

	return sprintf(buff, "0x%x\n", read_value);
}

/**
 * xroe_msg_filter_reg_store - The generic store function for msg_filter entries
 * @kobj:	The kernel object of the entry
 * @attr:	The attributes of the kernel object
 * @buff:	The buffer containing the revision string
 * @count:	The number of characters typed by the user
 *
 * Stores the input value to the requested register.The address, offset & mask
 * of the register are passed to the function using the xroe_reg_attribute
 * struct, during the initialisation of each attribute. This is done using
 * container_of() at the beginning of the function. To determine the current
 * eth port, sscanf is used on the current path. Another sscanf is done
 * immediately after, this time for the message filter number. After that, these
 * 2 numbers are taken into account when calculating the working address.
 * The input value is then parsed in hex and the register writing takes places
 * after that
 *
 * Return: the number of characters the user typed
 */
static ssize_t xroe_msg_filter_reg_store(struct kobject *kobj,
					 struct kobj_attribute *attr,
					 const char *buff, size_t count)
{
	int ret;
	u32 input_value;
	void __iomem *working_address;
	int port = 0;
	int filter_number = 0;
	char *current_path = NULL;
	char filter_dir[70];
	struct xroe_reg_attribute *xroe_read_attr = container_of(attr,
	struct xroe_reg_attribute, attr);

	ret = kstrtouint(buff, 16, &input_value);
	if (ret)
		return ret;
	current_path = kobject_get_path(kobj, GFP_KERNEL);
	ret = sscanf(current_path,
		     "/kernel/xroe/deframer/eth_config/eth_port_%d/",
		     &port);
	if (ret < 0)
		return ret;
	snprintf(filter_dir,
		 sizeof(filter_dir),
		 "/kernel/xroe/deframer/eth_config/eth_port_%d/message_filter_%%d/",
		 port);
	ret = sscanf(current_path, filter_dir, &filter_number);
	if (ret < 0)
		return ret;
	working_address = (void __iomem *)(lp->base_addr +
			  (xroe_read_attr->addr + (ADDR_LOOP_OFFSET_ETH * port) +
			   (ADDR_LOOP_OFFSET_FILTER * filter_number)));
	utils_write32withmask(working_address, input_value,
			      xroe_read_attr->mask, xroe_read_attr->offset);

	return count;
}

static XROE_MSG_FILTER_REG_ATTR(b31_0, DEFM_USER_DATA_FILTER_W0_31_0_OFFSET,
			DEFM_USER_DATA_FILTER_W0_31_0_MASK,
			DEFM_USER_DATA_FILTER_W0_31_0_ADDR);
static XROE_MSG_FILTER_REG_ATTR(b63_32, DEFM_USER_DATA_FILTER_W0_63_32_OFFSET,
			DEFM_USER_DATA_FILTER_W0_63_32_MASK,
			DEFM_USER_DATA_FILTER_W0_63_32_ADDR);
static XROE_MSG_FILTER_REG_ATTR(b95_64, DEFM_USER_DATA_FILTER_W0_95_64_OFFSET,
			DEFM_USER_DATA_FILTER_W0_95_64_MASK,
			DEFM_USER_DATA_FILTER_W0_95_64_ADDR);
static XROE_MSG_FILTER_REG_ATTR(b127_96, DEFM_USER_DATA_FILTER_W0_127_96_OFFSET,
			DEFM_USER_DATA_FILTER_W0_127_96_MASK,
			DEFM_USER_DATA_FILTER_W0_127_96_ADDR);
static XROE_MSG_FILTER_REG_ATTR(mask, DEFM_USER_DATA_FILTER_W0_MASK_OFFSET,
			DEFM_USER_DATA_FILTER_W0_MASK_MASK,
			DEFM_USER_DATA_FILTER_W0_MASK_ADDR);

static XROE_RW_REG_ATTR(dest_addr_31_0, ETH_DEST_ADDR_31_0_OFFSET,
			ETH_DEST_ADDR_31_0_MASK,
			ETH_DEST_ADDR_31_0_ADDR);
static XROE_RW_REG_ATTR(dest_addr_47_32, ETH_DEST_ADDR_47_32_OFFSET,
			ETH_DEST_ADDR_47_32_MASK,
			ETH_DEST_ADDR_47_32_ADDR);
static XROE_RW_REG_ATTR(src_addr_31_0, ETH_SRC_ADDR_31_0_OFFSET,
			ETH_SRC_ADDR_31_0_MASK,
			ETH_SRC_ADDR_31_0_ADDR);
static XROE_RW_REG_ATTR(src_addr_47_32, ETH_SRC_ADDR_47_32_OFFSET,
			ETH_SRC_ADDR_47_32_MASK,
			ETH_SRC_ADDR_47_32_ADDR);
static XROE_RW_REG_ATTR(vlan_id, ETH_VLAN_ID_OFFSET,
			ETH_VLAN_ID_MASK,
			ETH_VLAN_ID_ADDR);
static XROE_RW_REG_ATTR(vlan_dei, ETH_VLAN_DEI_OFFSET,
			ETH_VLAN_DEI_MASK,
			ETH_VLAN_DEI_ADDR);
static XROE_RW_REG_ATTR(vlan_pcp, ETH_VLAN_PCP_OFFSET,
			ETH_VLAN_PCP_MASK,
			ETH_VLAN_PCP_ADDR);
static XROE_RW_REG_ATTR(ipv4_version, ETH_IPV4_VERSION_OFFSET,
			ETH_IPV4_VERSION_MASK,
			ETH_IPV4_VERSION_ADDR);
static XROE_RW_REG_ATTR(ipv4_ihl, ETH_IPV4_IHL_OFFSET,
			ETH_IPV4_IHL_MASK,
			ETH_IPV4_IHL_ADDR);
static XROE_RW_REG_ATTR(ipv4_dscp, ETH_IPV4_DSCP_OFFSET,
			ETH_IPV4_DSCP_MASK,
			ETH_IPV4_DSCP_ADDR);
static XROE_RW_REG_ATTR(ipv4_ecn, ETH_IPV4_ECN_OFFSET,
			ETH_IPV4_ECN_MASK,
			ETH_IPV4_ECN_ADDR);
static XROE_RW_REG_ATTR(ipv4_id, ETH_IPV4_ID_OFFSET,
			ETH_IPV4_ID_MASK,
			ETH_IPV4_ID_ADDR);
static XROE_RW_REG_ATTR(ipv4_flags, ETH_IPV4_FLAGS_OFFSET,
			ETH_IPV4_FLAGS_MASK,
			ETH_IPV4_FLAGS_ADDR);
static XROE_RW_REG_ATTR(ipv4_fragment_offset, ETH_IPV4_FRAGMENT_OFFSET_OFFSET,
			ETH_IPV4_FRAGMENT_OFFSET_MASK,
			ETH_IPV4_FRAGMENT_OFFSET_ADDR);
static XROE_RW_REG_ATTR(ipv4_time_to_live, ETH_IPV4_TIME_TO_LIVE_OFFSET,
			ETH_IPV4_TIME_TO_LIVE_MASK,
			ETH_IPV4_TIME_TO_LIVE_ADDR);
static XROE_RW_REG_ATTR(ipv4_protocol, ETH_IPV4_PROTOCOL_OFFSET,
			ETH_IPV4_PROTOCOL_MASK,
			ETH_IPV4_PROTOCOL_ADDR);
static XROE_RW_REG_ATTR(ipv4_source_add, ETH_IPV4_SOURCE_ADD_OFFSET,
			ETH_IPV4_SOURCE_ADD_MASK,
			ETH_IPV4_SOURCE_ADD_ADDR);
static XROE_RW_REG_ATTR(ipv4_destination_add, ETH_IPV4_DESTINATION_ADD_OFFSET,
			ETH_IPV4_DESTINATION_ADD_MASK,
			ETH_IPV4_DESTINATION_ADD_ADDR);
static XROE_RW_REG_ATTR(udp_source_port, ETH_UDP_SOURCE_PORT_OFFSET,
			ETH_UDP_SOURCE_PORT_MASK,
			ETH_UDP_SOURCE_PORT_ADDR);
static XROE_RW_REG_ATTR(udp_destination_port, ETH_UDP_DESTINATION_PORT_OFFSET,
			ETH_UDP_DESTINATION_PORT_MASK,
			ETH_UDP_DESTINATION_PORT_ADDR);
static XROE_RW_REG_ATTR(ipv6_v, ETH_IPV6_V_OFFSET,
			ETH_IPV6_V_MASK,
			ETH_IPV6_V_ADDR);
static XROE_RW_REG_ATTR(ipv6_traffic_class, ETH_IPV6_TRAFFIC_CLASS_OFFSET,
			ETH_IPV6_TRAFFIC_CLASS_MASK,
			ETH_IPV6_TRAFFIC_CLASS_ADDR);
static XROE_RW_REG_ATTR(ipv6_flow_label, ETH_IPV6_FLOW_LABEL_OFFSET,
			ETH_IPV6_FLOW_LABEL_MASK,
			ETH_IPV6_FLOW_LABEL_ADDR);
static XROE_RW_REG_ATTR(ipv6_next_header, ETH_IPV6_NEXT_HEADER_OFFSET,
			ETH_IPV6_NEXT_HEADER_MASK,
			ETH_IPV6_NEXT_HEADER_ADDR);
static XROE_RW_REG_ATTR(ipv6_hop_limit, ETH_IPV6_HOP_LIMIT_OFFSET,
			ETH_IPV6_HOP_LIMIT_MASK,
			ETH_IPV6_HOP_LIMIT_ADDR);
static XROE_RW_REG_ATTR(ipv6_source_add_31_0, ETH_IPV6_SOURCE_ADD_31_0_OFFSET,
			ETH_IPV6_SOURCE_ADD_31_0_MASK,
			ETH_IPV6_SOURCE_ADD_31_0_ADDR);
static XROE_RW_REG_ATTR(ipv6_source_add_63_32, ETH_IPV6_SOURCE_ADD_63_32_OFFSET,
			ETH_IPV6_SOURCE_ADD_63_32_MASK,
			ETH_IPV6_SOURCE_ADD_63_32_ADDR);
static XROE_RW_REG_ATTR(ipv6_source_add_95_64, ETH_IPV6_SOURCE_ADD_95_64_OFFSET,
			ETH_IPV6_SOURCE_ADD_95_64_MASK,
			ETH_IPV6_SOURCE_ADD_95_64_ADDR);
static XROE_RW_REG_ATTR(ipv6_source_add_127_96,
			ETH_IPV6_SOURCE_ADD_127_96_OFFSET,
			ETH_IPV6_SOURCE_ADD_127_96_MASK,
			ETH_IPV6_SOURCE_ADD_127_96_ADDR);
static XROE_RW_REG_ATTR(ipv6_destination_add_31_0,
			ETH_IPV6_DESTINATION_ADD_31_0_OFFSET,
			ETH_IPV6_DESTINATION_ADD_31_0_MASK,
			ETH_IPV6_DESTINATION_ADD_31_0_ADDR);
static XROE_RW_REG_ATTR(ipv6_destination_add_63_32,
			ETH_IPV6_DESTINATION_ADD_63_32_OFFSET,
			ETH_IPV6_DESTINATION_ADD_63_32_MASK,
			ETH_IPV6_DESTINATION_ADD_63_32_ADDR);
static XROE_RW_REG_ATTR(ipv6_destination_add_95_64,
			ETH_IPV6_DESTINATION_ADD_95_64_OFFSET,
			ETH_IPV6_DESTINATION_ADD_95_64_MASK,
			ETH_IPV6_DESTINATION_ADD_95_64_ADDR);
static XROE_RW_REG_ATTR(ipv6_destination_add_127_96,
			ETH_IPV6_DESTINATION_ADD_127_96_OFFSET,
			ETH_IPV6_DESTINATION_ADD_127_96_MASK,
			ETH_IPV6_DESTINATION_ADD_127_96_ADDR);

static struct attribute *msg_filters_attrs[] = {
	&attr_b31_0.attr.attr,
	&attr_b63_32.attr.attr,
	&attr_b95_64.attr.attr,
	&attr_b127_96.attr.attr,
	&attr_mask.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(msg_filters);

static struct attribute *eth_config_attrs[] = {
	&attr_dest_addr_31_0.attr.attr,
	&attr_dest_addr_47_32.attr.attr,
	&attr_src_addr_31_0.attr.attr,
	&attr_src_addr_47_32.attr.attr,
	&attr_vlan_id.attr.attr,
	&attr_vlan_dei.attr.attr,
	&attr_vlan_pcp.attr.attr,
	&attr_ipv4_version.attr.attr,
	&attr_ipv4_ihl.attr.attr,
	&attr_ipv4_dscp.attr.attr,
	&attr_ipv4_ecn.attr.attr,
	&attr_ipv4_id.attr.attr,
	&attr_ipv4_flags.attr.attr,
	&attr_ipv4_fragment_offset.attr.attr,
	&attr_ipv4_time_to_live.attr.attr,
	&attr_ipv4_protocol.attr.attr,
	&attr_ipv4_source_add.attr.attr,
	&attr_ipv4_destination_add.attr.attr,
	&attr_udp_source_port.attr.attr,
	&attr_udp_destination_port.attr.attr,
	&attr_ipv6_v.attr.attr,
	&attr_ipv6_traffic_class.attr.attr,
	&attr_ipv6_flow_label.attr.attr,
	&attr_ipv6_next_header.attr.attr,
	&attr_ipv6_hop_limit.attr.attr,
	&attr_ipv6_source_add_31_0.attr.attr,
	&attr_ipv6_source_add_63_32.attr.attr,
	&attr_ipv6_source_add_95_64.attr.attr,
	&attr_ipv6_source_add_127_96.attr.attr,
	&attr_ipv6_destination_add_31_0.attr.attr,
	&attr_ipv6_destination_add_63_32.attr.attr,
	&attr_ipv6_destination_add_95_64.attr.attr,
	&attr_ipv6_destination_add_127_96.attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(eth_config);

/**
 * xroe_sysfs_defm_init - Creates the "deframer" sysfs directory and entries
 *
 * Return: 0 on success, negative value in case of failure to
 * create the sysfs group
 *
 * Creates the "deframer" sysfs directory and entries, as well as the eth_port_#
 * subdirectories
 */
int xroe_sysfs_defm_init(void)
{
	int ret;
	int i;
	int j;
	char msg_filter_dir_name[17];
	char eth_port_dir_name[11];
	struct kobject *kobj_eth_msg_filter[ETH_FILTER_LENGTH_IN_WORDS];
	struct kobject *kobj_dir_eth_ports[MAX_NUM_ETH_PORTS];
	struct kobject *kobj_dir_defm;
	struct kobject *kobj_dir_eth_config;

	kobj_dir_defm = kobject_create_and_add("deframer", root_xroe_kobj);
	if (!kobj_dir_defm)
		return -ENOMEM;

	kobj_dir_eth_config = kobject_create_and_add("eth_config",
						     kobj_dir_defm);
	if (!kobj_dir_eth_config)
		return -ENOMEM;

	for (i = 0; i < MAX_NUM_ETH_PORTS; i++) {
		snprintf(eth_port_dir_name, sizeof(eth_port_dir_name),
			 "eth_port_%d", i);
		kobj_dir_eth_ports[i] =
		kobject_create_and_add(eth_port_dir_name, kobj_dir_eth_config);
		if (!kobj_dir_eth_ports[i])
			return -ENOMEM;
		ret = sysfs_create_group(kobj_dir_eth_ports[i],
					 *eth_config_groups);
		if (ret)
			kobject_put(kobj_dir_eth_ports[i]);
		for (j = 0; j < ETH_FILTER_LENGTH_IN_WORDS; j++) {
			snprintf(msg_filter_dir_name,
				 sizeof(msg_filter_dir_name),
				 "message_filter_%d", j);
			kobj_eth_msg_filter[j] =
			kobject_create_and_add(msg_filter_dir_name,
					       kobj_dir_eth_ports[i]);
			if (!kobj_eth_msg_filter[j])
				return -ENOMEM;
			ret = sysfs_create_group(kobj_eth_msg_filter[j],
						 *msg_filters_groups);
			if (ret)
				kobject_put(kobj_eth_msg_filter[j]);
		}
	}

	return ret;
}
