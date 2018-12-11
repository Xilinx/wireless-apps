// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file xroefram_str.h
* @addtogroup sample_app
* @{
*
*  A sample application to demonstrate the RoE Framer software modules.
*
******************************************************************************/

/**
 * XROE_USAGE_STR Help text for the application, listing command line options.
 */
#define XROE_USAGE_STR "Xilinx Radio Over Ethernet Framer Application (xroe-app)\n" \
__DATE__ " " __TIME__ "\n" \
"Usage: <app> [options] [\"command [args]\"]\n" \
"Options:\n" \
"  -d daemonise server\n" \
"  -s soft server mode, no local hardware\n" \
"  -c send command to server\n" \
"  -n <ip_addr> with -c send command to remote app at <ip_addr>\n" \
"  -p <port> with -n specifies remote port to send to, with -d or -s specifies server listen port\n" \
"  -h produces this help\n" \
"\n" \
"Commands:\n" \
"  <command> [args]\n" \
"Try: <app> [-n <ip_addr> [-p <port>]] -c \"help\" for list of server commands\n"

/**
 * XROE_QUIT_STR Response string for quit command.
 */
#define XROE_QUIT_STR "Received quit command, exiting\n"

/**
 * XROE_HELP_STR Help text for "help" command.
 */
#define XROE_HELP_STR "generates this list of commands\n"

/**
 * XROE_VER_STR Help text for "version" command.
 */
#define XROE_VER_STR "Not yet supported\n"

/**
 * XROE_ENABLE_STR Help text for "enable" command.
 */
#define XROE_ENABLE_STR "Enable framer/deframer\n"

/**
 * XROE_FRAM_STR Help text for "framing" command.
 */
#define XROE_FRAM_STR "Framer/deframer control commands\n"

/**
 * XROE_DISABLE_STR Help text for "disable" command.
 */
#define XROE_DISABLE_STR "Disable framer/deframer\n"

/**
 * XROE_RESTART_STR Help text for "restart" command.
 */
#define XROE_RESTART_STR "XXV Reset\n"

/**
 * ECPRI_STR Help text for "ecpri" command.
 */
#define ECPRI_STR "eCPRI commands\n"

/**
 * IP_STR Help text for "ip" command.
 */
#define IP_STR "Raw IP memory device access\n"

/**
 * STATS_STR Help text for "stats" command.
 */
#define STATS_STR "Stats device access\n"

/**
 * RADIO_CTRL_STR Help text for "radio" command.
 */
#define RADIO_CTRL_STR "Radio control device access\n"
/** @} */