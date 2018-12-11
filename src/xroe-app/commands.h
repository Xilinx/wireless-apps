// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file commands.h
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <xroefram_str.h>

/**
 * XROE_MAX_COMMANDS Number of commands handled at the top level.
 */
#define XROE_MAX_COMMANDS 11

/************************** Function Prototypes ******************************/
int help_func(int argc, char **argv, char *resp);
int ecpri_func(int argc, char **argv, char *resp);
int ip_func(int argc, char **argv, char *resp);
int stats_func(int argc, char **argv, char *resp);
int version_func(int argc, char **argv, char *resp);
int enable_func(int argc, char **argv, char *resp);
int disable_func(int argc, char **argv, char *resp);
int framing_func(int argc, char **argv, char *resp);
int restart_func(int argc, char **argv, char *resp);
int radio_ctrl_func(int argc, char **argv, char *resp);

/**
 * cmds The top-level commands handled by the command parser.
 */
commands_t cmds[XROE_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", XROE_HELP_STR, help_func}, /**< "help" command */
	/* Insert commands here */
	{"ecpri", ECPRI_STR, ecpri_func}, /**< "ecpri" command */
	{"ip", IP_STR, ip_func}, /**< "stats" command */
	{"stats", STATS_STR, stats_func}, /**< "stats" command */
	{"version", XROE_VER_STR, version_func}, /**< "version" command */
	{"enable", XROE_ENABLE_STR, enable_func}, /**< "enable" command */
	{"disable", XROE_DISABLE_STR, disable_func}, /**< "disable" command */
	{"restart", XROE_RESTART_STR, restart_func}, /**< "restart" command */
	{"framing", XROE_FRAM_STR, framing_func}, /**< "framing" command */
	{"radio", RADIO_CTRL_STR, radio_ctrl_func}, /**< "radio" command */
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL} /**< NULL command to terminate array */
};
/** @} */