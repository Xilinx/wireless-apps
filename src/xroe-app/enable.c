// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file enable.c
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
#include <enable_str.h>
#include <errno.h>
#include <inttypes.h>
#include <xroe_api.h>

/**
 * ENABLE_MAX_COMMANDS Number of commands handled by the enable module.
 */
#define ENABLE_MAX_COMMANDS 4

/************************** Function Prototypes ******************************/
int enable_help_func(int argc, char **argv, char *resp);
int enable_framer_func(int argc, char **argv, char *resp);
int enable_deframer_func(int argc, char **argv, char *resp);

/**
 * enable_cmds The commands handled by the enable module.
 */
commands_t enable_cmds[ENABLE_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", ENABLE_HELP_STR, enable_help_func},
	/* Insert commands here */
	{"framer", ENABLE_FRAMER_STR, enable_framer_func},
	{"deframer", ENABLE_DEFRAMER_STR, enable_deframer_func},
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};


/*****************************************************************************/
/**
*
* Enables the framer by setting the restart value low.
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
int enable_framer_func(int argc, char **argv, char *resp)
{
	return(FRAMER_API_Framer_Restart(0));
}

/*****************************************************************************/
/**
*
* Enables the deframer by setting the restart value low.
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
int enable_deframer_func(int argc, char **argv, char *resp)
{
	return(FRAMER_API_Deframer_Restart(0));
}


/*****************************************************************************/
/**
*
* Returns help strings for enable commands.
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
int enable_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "enable help:\n");

	for(i=0; enable_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", enable_cmds[i].cmd, enable_cmds[i].helptxt);
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
int enable_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", ENABLE_USAGE_STR);
		return(1);
	}

	for(count=0; enable_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], enable_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			enable_cmds[count].func(argc-1, &argv[1], resp);
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