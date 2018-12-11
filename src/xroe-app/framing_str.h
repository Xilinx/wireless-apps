// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/**
 * FRAMING_USAGE_STR Help text for the framing module.
 */
#define FRAMING_USAGE_STR "Usage \"framing <options>\", try \"framing help\" for command list\n"

/**
 * FRAMING_HELP_STR Help text for the framing module "help" option.
 */
#define FRAMING_HELP_STR "generates this list of commands\n"

/**
 * FRAMING_SET_FRAM_STR Help text for the framing module "set_fram" option.
 */
#define FRAMING_SET_FRAM_STR "Set framer register value, usage \"framing set_fram <antenna> <register> <value>\", \n" \
"\t\t\t<antenna>: 0-31\n" \
"\t\t\t<register>: data_pc_id, data_type, data_port, ctrl_pc_id, ctrl_type, ctrl_port\n" \
"\t\t\t<value>: see datasheet\n"

/**
 * FRAMING_GET_FRAM_STR Help text for the framing module "get_fram" option.
 */
#define FRAMING_GET_FRAM_STR "Get framer register value, usage \"framing get_fram <antenna> <register>\", \n" \
"\t\t\t<antenna>: 0-31\n" \
"\t\t\t<register>: data_pc_id, data_type, data_port, ctrl_pc_id, ctrl_type, ctrl_port\n"

/**
 * FRAMING_SET_DEFR_STR Help text for the framing module "set_defr" option.
 */
#define FRAMING_SET_DEFR_STR "Set deframer register value, usage \"framing set_defr <antenna> <register> <value>\", \n" \
"\t\t\t<antenna>: 0-31\n" \
"\t\t\t<register>: data_pc_id, ctrl_pc_id\n" \
"\t\t\t<value>: see datasheet\n"

/**
 * FRAMING_GET_DEFR_STR Help text for the framing module "get_defr" option.
 */
#define FRAMING_GET_DEFR_STR "Get deframer register value, usage \"framing get_defr <antenna> <register>\", \n" \
"\t\t\t<antenna>: 0-31\n" \
"\t\t\t<register>: data_pc_id, ctrl_pc_id,\n" \
"\t\t\t\tdbs_latency, dbs_alignment, dbs_overflow, dbs_underflow, dbs_regular, dbs_rwin\n" \
"\t\t\t\tcbs_latency, cbs_alignment, cbs_overflow, cbs_underflow, cbs_regular, cbs_rwin\n"

