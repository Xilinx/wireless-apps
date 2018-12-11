// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

 /** 
* @file parser.h
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/**
 * MAX_NUMBER_TOKENS Maximum number of tokens a command string may contain.
 */
#define MAX_NUMBER_TOKENS 10

/************************** Function Prototypes ******************************/
int tokenise_input(char *in_str, char **cmd_tokens);
int parse_command(int nohw, char *command, char *response);
/** @} */