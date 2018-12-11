// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file ip.c
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
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <inttypes.h>

#include <xroe_types.h>
#include <ip_str.h>
#include <xroe_api.h>

/**
 * IP_MAX_COMMANDS Number of commands handled by the IP module.
 */
#define IP_MAX_COMMANDS 4

/************************** Function Prototypes ******************************/
int ip_help_func(int argc, char **argv, char *resp);
int ip_peek_func(int argc, char **argv, char *resp);
int ip_poke_func(int argc, char **argv, char *resp);

/**
 * ip_cmds The commands handled by the IP module.
 */
commands_t ip_cmds[IP_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", IP_HELP_STR, ip_help_func},
	/* Insert commands here */
	{"peek", IP_PEEK_STR, ip_peek_func},
	{"poke", IP_POKE_STR, ip_poke_func},
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};


/*****************************************************************************/
/**
*
* Returns help strings for IP commands.
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
int ip_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "ip help:\n");

	for(i=0; ip_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", ip_cmds[i].cmd, ip_cmds[i].helptxt);
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Returns the 32-bits at the given memory address.
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
int ip_peek_func(int argc, char **argv, char *resp)
{
	int buf;
	int addr;
	int read = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", IP_PEEK_STR);
		return(1);
	}

	addr = strtol(argv[0], NULL, 0);

	read = IP_API_Read(addr, (uint8_t *)&buf, sizeof(buf));
	if(!read)
	{
		str += sprintf(str, "peek: 0x%08x : 0x%08x\n", addr, buf);
	}
	else
	{
		str += sprintf(str, "ip peek 0x%08x: error %d\n", addr, read);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Writes the 32-bits given to the given memory address.
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
int ip_poke_func(int argc, char **argv, char *resp)
{
	int buf;
	int addr;
	int write = 0;
	char *str = resp;

	if(argc < 2)
	{
		sprintf(str, "\t%s", IP_POKE_STR);
		return(1);
	}

	addr = strtol(argv[0], NULL, 0);
	buf = strtol(argv[1], NULL, 0);

	str += sprintf(str, "addr: 0x%08x, value: 0x%08x\n", addr, buf);

	write = IP_API_Write(addr, (uint8_t *)&buf, sizeof(buf));
	if(!write)
	{
		str += sprintf(str, "poke: 0x%08x : 0x%08x\n", addr, buf);
	}
	else
	{
		str += sprintf(str, "ip poke 0x%08x: error %d\n", addr, write);
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
int ip_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", IP_USAGE_STR);
		return(1);
	}

	for(count=0; ip_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], ip_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			ip_cmds[count].func(argc-1, &argv[1], str);
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