// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file disable.c
* @addtogroup command_parser
* @{
*
*  A sample command parser for the RoE Framer software modules.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <xroe_types.h>
#include <disable_str.h>
#include <errno.h>
#include <inttypes.h>
#include <xroe_api.h>

/**
 * DISABLE_MAX_COMMANDS Number of commands handled by the disable module.
 */
#define DISABLE_MAX_COMMANDS 4

/************************** Function Prototypes ******************************/
int disable_help_func(int argc, char **argv, char *resp);
int disable_framer_func(int argc, char **argv, char *resp);
int disable_deframer_func(int argc, char **argv, char *resp);

/**
 * disable_cmds The commands handled by the disable module.
 */
commands_t disable_cmds[DISABLE_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", DISABLE_HELP_STR, disable_help_func},
	/* Insert commands here */
	{"framer", DISABLE_FRAMER_STR, disable_framer_func},
	{"deframer", DISABLE_DEFRAMER_STR, disable_deframer_func},
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};


/*****************************************************************************/
/**
*
* Disables the framer by setting the restart value high.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- Return value of FRAMER_API_Framer_Restart()
*
******************************************************************************/
int disable_framer_func(int argc, char **argv, char *resp)
{
	return(FRAMER_API_Framer_Restart(1));
}

/*****************************************************************************/
/**
*
* Disables the deframer by setting the restart value high.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- Return value of FRAMER_API_Deframer_Restart()
*
******************************************************************************/
int disable_deframer_func(int argc, char **argv, char *resp)
{
	return(FRAMER_API_Deframer_Restart(1));
}


/*****************************************************************************/
/**
*
* Returns help strings for disable commands.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0
*
******************************************************************************/
int disable_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "disable help:\n");

	for(i=0; disable_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", disable_cmds[i].cmd, disable_cmds[i].helptxt);
	}
	return 0;
}


/*****************************************************************************/
/**
*
* Parses the input command string and calls a handler function (if found).
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 1 if no commands tokens found.
*		- 2 if no handler found for command.
*		- 0 otherwise.
*
******************************************************************************/
int disable_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", DISABLE_USAGE_STR);
		return(1);
	}

	for(count=0; disable_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], disable_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			disable_cmds[count].func(argc-1, &argv[1], resp);
		}
	}

	if(!found)
	{
		str += sprintf(str, "Command %s not found, try \"help\"\n", argv[0]);
		return(2);
	}

	return 0;
}
/** @} */