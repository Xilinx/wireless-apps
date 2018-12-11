// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ip_str.h
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/**
 * IP_USAGE_STR Help text for the enable module.
 */
#define IP_USAGE_STR "Usage is \"ip <command> <address> [value]\", try \"ip help\" for command list\n"

/**
 * IP_HELP_STR Help text for the IP module "help" option.
 */
#define IP_HELP_STR "generates this list of ip subcommands\n"

/**
 * IP_PEEK_STR Help text for the IP module "peek" option.
 */
#define IP_PEEK_STR "ip peek <address>\n"

/**
 * IP_POKE_STR Help text for the IP module "poke" option.
 */
#define IP_POKE_STR "ip poke <address> <value>\n"
/** @} */