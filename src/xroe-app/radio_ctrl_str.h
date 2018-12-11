// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file radio_ctrl_str.h
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/**
 * RADIO_CTRL_USAGE_STR Help text for the radio_ctrl module.
 */
#define RADIO_CTRL_USAGE_STR "Usage \"radio_ctrl <options>\", try \"radio_ctrl help\" for command list\n"

/**
 * RADIO_CTRL_HELP_STR Help text for the radio_ctrl module "help" option.
 */
#define RADIO_CTRL_HELP_STR "generates this list of commands\n"

/**
 * RADIO_CTRL_STATUS_STR Help text for the radio_ctrl module "status" option.
 */
#define RADIO_CTRL_STATUS_STR "Returns the enable, error, status and loopback\n"

/**
 * RADIO_CTRL_LOOPBACK_EN_STR Help text for the radio_ctrl module "loopback_en" option.
 */
#define RADIO_CTRL_LOOPBACK_EN_STR "Enables the loopback\n"

/**
 * RADIO_CTRL_LOOPBACK_DIS_STR Help text for the radio_ctrl module "loopback_dis" option.
 */
#define RADIO_CTRL_LOOPBACK_DIS_STR "Disables the loopback\n"

/**
 * RADIO_CTRL_RADIO_ID_STR Help text for the radio_ctrl module "id" option.
 */
#define RADIO_CTRL_RADIO_ID_STR "[DEV] Returns the Radio ID tag\n"

/**
 * RADIO_CTRL_GUI_STR Help text for the radio_ctrl module "gui" option.
 */
#define RADIO_CTRL_GUI_STR "[DEV] all radio output for GUI\n"

/**
 * RADIO_CTRL_PEEK_STR Help text for the radio_ctrl module "peek" option.
 */
#define RADIO_CTRL_PEEK_STR "radio_ctrl peek <address>\n"

/**
 * RADIO_CTRL_POKE_STR Help text for the radio_ctrl module "poke" option.
 */
#define RADIO_CTRL_POKE_STR "radio_ctrl poke <address> <value>\n"
/** @} */