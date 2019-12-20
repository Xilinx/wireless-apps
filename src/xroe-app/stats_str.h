// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file restart_str.h
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/**
 * STATS_USAGE_STR Help text for the stats module.
 */
#define STATS_USAGE_STR "Usage \"stats <options>\", try \"stats help\" for command list\n"

/**
 * STATS_HELP_STR Help text for the stats module "help" option.
 */
#define STATS_HELP_STR "generates this list of commands\n"

/**
 * STATS_SW_STR Help text for the stats module "sw" option.
 */
#define STATS_SW_STR "Software statistics not supported\n"

/**
 * STATS_USER_STR Help text for the stats module "user" option.
 */
#define STATS_USER_STR "Returns user data packets stats\n"

/**
 * STATS_CTRL_STR Help text for the stats module "control" option.
 */
#define STATS_CTRL_STR "Returns control packets stats\n"

/**
 * STATS_RATE_STR Help text for the stats module "rate" option.
 */
#define STATS_RATE_STR "Returns packet rate stats\n"

/**
 * STATS_ALL_STR Help text for the stats module "all" option.
 */
#define STATS_ALL_STR "Returns a summary of all stats\n"

/**
 * STATS_DEV_NUM_STR Help text for the stats module "rev" option.
 */
#define STATS_DEV_NUM_STR "[DEV] get the internal revision number of the stats device\n"

/**
 * STATS_GUI_NUM_STR Help text for the stats module "gui" option.
 */
#define STATS_GUI_NUM_STR "[DEV] all stats output for GUI\n"
/** @} */