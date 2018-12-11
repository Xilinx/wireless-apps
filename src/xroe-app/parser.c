// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file parser.c
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
#include <string.h>

#include <xroe_types.h>
#include <xroefram_str.h>
#include <commands.h>
#include <parser.h>

/*****************************************************************************/
/**
*
* Returns help strings for top-level commands.
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
int help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "xroe-app help:\n");

	for (i = 0; i<XROE_MAX_COMMANDS - 1; i++)
	{
		str += sprintf(str, "\t%s\t : %s", cmds[i].cmd, cmds[i].helptxt);
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Dummy function for un-implemented "version" command.
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
int version_func(int argc, char **argv, char *resp)
{
	sprintf(resp, XROE_VER_STR);
	return 0;
}


/*****************************************************************************/
/**
*
* Splits the input string into an array of command tokens.
* 
*
* @param [in]	in_str   	Input string of space separated tokens.
* @param [out]	cmd_tokens	Pointer to string array to place tokens in.
*
* @return
*		- Number of tokens separated.
*
******************************************************************************/
int tokenise_input(char *in_str, char **cmd_tokens)
{
	int num_tokens = 0;
	char *tmp;

	tmp = strtok(in_str, " ");

	while ((tmp != NULL) && (num_tokens < MAX_NUMBER_TOKENS))
	{
		cmd_tokens[num_tokens] = tmp;
		num_tokens++;
		tmp = strtok(NULL, " ");
	}

	return num_tokens;
}


/*****************************************************************************/
/**
*
* Parses the input command string and calls a lower-level handler (if found).
* 
*
* @param [in]	nohw   		Ignored.
* @param [in]	command		String of space separated command tokens.
* @param [out]	response	Pointer to string to place response text in.
*
* @return
*		- 0 if no commands tokens found or no handler for command.
*		- -1 on "quit" command.
*		- Return value of lower-level command handler otherwise.
*
******************************************************************************/
int parse_command(int nohw, char *command, char *response)
{
	int retVal = 0;
	int count = 0;
	int found = 0;
	char *cmd_tokens[MAX_NUMBER_TOKENS];
	int num_tokens = tokenise_input(command, cmd_tokens);

	if (num_tokens == 0)
	{
		sprintf(response, "%s", XROE_USAGE_STR);
	}
	else if (strcmp(cmd_tokens[0], "quit") == 0)
	{
		sprintf(response, "%s", XROE_QUIT_STR);
		retVal = -1;
	}
	else
	{
		for (count = 0; cmds[count].cmd != NULL; count++)
		{
			if (strcmp(cmd_tokens[0], cmds[count].cmd) == 0)
			{
				found = 1;
				/* Call the handler function for the command given */
				retVal = cmds[count].func(num_tokens - 1, &cmd_tokens[1], response);
			}
		}

		if (!found)
		{
			sprintf(response, "Command %s not found, try \"help\"\n", cmd_tokens[0]);
		}
	}

	return retVal;
}
/** @} */