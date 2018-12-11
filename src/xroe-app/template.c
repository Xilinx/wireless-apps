// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xroe_types.h>
#include <template_str.h>

#define TEMPLATE_MAX_COMMANDS 2

int template_help_func(int argc, char **argv);

commands_t template_cmds[TEMPLATE_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", TEMPLATE_HELP_STR, template_help_func},
	/* Insert commands here */
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};


int template_help_func(int argc, char **argv)
{
	int i;
	
	printf("ip help:\n");
	
	for(i=0; template_cmds[i].cmd != NULL; i++)
	{
		printf("\t%s\t : %s", template_cmds[i].cmd, template_cmds[i].helptxt);
	}
	return 0;
}

int template_func(int argc, char **argv)
{
	int count = 0;
	int found = 0;
	
	if(argc < 2)
	{
		printf("\t%s", TEMPLATE_USAGE_STR);
		return(1);
	}

	for(count=0; template_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[1], template_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			template_cmds[count].func(argc-1, &argv[1]);
		}
	}
	
	if(!found)
	{
		printf("Command %s not found, try \"help\"\n", argv[1]);
		return(2);
	}

	return 0;
}
