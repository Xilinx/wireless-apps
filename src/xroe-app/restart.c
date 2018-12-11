// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file restart.c
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
#include <restart_str.h>
#include <errno.h>
#include <inttypes.h>
#include <xroe_api.h>

/**
 * RESTART_MAX_COMMANDS Number of commands handled by the restart module.
 */
#define RESTART_MAX_COMMANDS 4

/************************** Function Prototypes ******************************/
int restart_help_func(int argc, char **argv, char *resp);
int restart_xxv_func(int argc, char **argv, char *resp);

/**
 * restart_cmds The commands handled by the restart module.
 */
commands_t restart_cmds[RESTART_MAX_COMMANDS] = {
	/* Keep this first */
	{ "help", RESTART_HELP_STR, restart_help_func },
	/* Insert commands here */
	{ "xxv", RESTART_XXV_RESET_STR, restart_xxv_func },
	/* Keep this last - insert commands above */
	{ NULL, NULL, NULL }
};


/*****************************************************************************/
/**
*
* Restarts the XXV Ethernet interface.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- Return value of XXV_API_Reset()
*
******************************************************************************/
int restart_xxv_func(int argc, char **argv, char *resp)
{
	return(XXV_API_Reset());
}


/*****************************************************************************/
/**
*
* Returns help strings for restart commands.
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
int restart_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "restart help:\n");

	for (i = 0; restart_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", restart_cmds[i].cmd, restart_cmds[i].helptxt);
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
int restart_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if (argc == 0)
	{
		sprintf(str, "\t%s", RESTART_USAGE_STR);
		return(1);
	}

	for (count = 0; restart_cmds[count].cmd != NULL; count++)
	{
		if (strcmp(argv[0], restart_cmds[count].cmd) == 0)
		{
			found = 1;
			/* Call the handler function for the command given */
			restart_cmds[count].func(argc - 1, &argv[1], resp);
		}
	}

	if (!found)
	{
		str += sprintf(str, "Command %s not found, try \"help\"\n", argv[0]);
		return(2);
	}

	return 0;
}
/** @} */