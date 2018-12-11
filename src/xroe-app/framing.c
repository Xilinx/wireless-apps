// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file framing.c
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
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xroe_types.h>
#include <xroe_api.h>
#include <framing_str.h>
#include <roe_framer_ctrl.h>

/**
 * FRAMING_MAX_COMMANDS Number of commands handled by the framing module.
 */
#define FRAMING_MAX_COMMANDS 6

/**
 * FRAMING_FRAMER_REG_NUM Number of registers valid for framer commands.
 */
#define FRAMING_FRAMER_REG_NUM 6

/**
 * FRAMING_DEFRAMER_REG_NUM Number of registers valid for deframer commands.
 */
#define FRAMING_DEFRAMER_REG_NUM 14

/**
 * FRAMING_DEFRAMER_WR_REG_NUM Number of registers valid for deframer writes.
 */
#define FRAMING_DEFRAMER_WR_REG_NUM 2

/**
 * FRAMING_MAX_REG_NAME_LENGTH Macimum length of a register name.
 */
#define FRAMING_MAX_REG_NAME_LENGTH 16

/**
 * FRAMING_REG_SIZE Sizee in bytes of framer/deframer registers.
 */
#define FRAMING_REG_SIZE 4

/************************** Function Prototypes ******************************/
int framing_help_func(int argc, char **argv, char *resp);
int framing_set_fram_func(int argc, char **argv, char *resp);
int framing_get_fram_func(int argc, char **argv, char *resp);
int framing_set_defr_func(int argc, char **argv, char *resp);
int framing_get_defr_func(int argc, char **argv, char *resp);

/**
 * framing_cmds The commands handled by the framing module.
 */
commands_t framing_cmds[FRAMING_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", FRAMING_HELP_STR, framing_help_func},
	/* Insert commands here */
	{"set_fram", FRAMING_SET_FRAM_STR, framing_set_fram_func},
	{"get_fram", FRAMING_GET_FRAM_STR, framing_get_fram_func},
	{"set_defr", FRAMING_SET_DEFR_STR, framing_set_defr_func},
	{"get_defr", FRAMING_GET_DEFR_STR, framing_get_defr_func},
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};

/**
 * register_access Base structure to define framer/deframer register fields.
 */
struct register_access
{
	uint32_t addr; /**< Register address */
	uint32_t offset; /**< Field offset in register */
	uint32_t mask; /**< Field mask in register */
	char name[FRAMING_MAX_REG_NAME_LENGTH]; /**< Register field name */
};

/**
 * framer_regs Register fields in the framer.
 */
struct register_access framer_regs[FRAMING_FRAMER_REG_NUM] = {
	{FRAM_DRPFRAM_DATA_PC_ID_ADDR, FRAM_DRPFRAM_DATA_PC_ID_OFFSET, FRAM_DRPFRAM_DATA_PC_ID_MASK, "data_pc_id"},
	{FRAM_DRPFRAM_DATA_MESSAGE_TYPE_ADDR, FRAM_DRPFRAM_DATA_MESSAGE_TYPE_OFFSET, FRAM_DRPFRAM_DATA_MESSAGE_TYPE_MASK,"data_type"},
	{FRAM_DRPFRAM_DATA_ETHERNET_PORT_ADDR, FRAM_DRPFRAM_DATA_ETHERNET_PORT_OFFSET, FRAM_DRPFRAM_DATA_ETHERNET_PORT_MASK, "data_port"},
	{FRAM_DRPFRAM_CTRL_PC_ID_ADDR, FRAM_DRPFRAM_CTRL_PC_ID_OFFSET, FRAM_DRPFRAM_CTRL_PC_ID_MASK, "ctrl_pc_id"},
	{FRAM_DRPFRAM_CTRL_MESSAGE_TYPE_ADDR, FRAM_DRPFRAM_CTRL_MESSAGE_TYPE_OFFSET, FRAM_DRPFRAM_CTRL_MESSAGE_TYPE_MASK, "ctrl_type"},
	{FRAM_DRPFRAM_CTRL_ETHERNET_PORT_ADDR, FRAM_DRPFRAM_CTRL_ETHERNET_PORT_OFFSET, FRAM_DRPFRAM_CTRL_ETHERNET_PORT_MASK, "ctrl_port"}
};

/**
 * deframer_regs Register fields in the deframer.
 */
struct register_access deframer_regs[FRAMING_DEFRAMER_REG_NUM] = {
	{DEFM_DRPDEFM_DATA_PC_ID_ADDR, DEFM_DRPDEFM_DATA_PC_ID_OFFSET, DEFM_DRPDEFM_DATA_PC_ID_MASK, "data_pc_id"},
	{DEFM_DRPDEFM_CTRL_PC_ID_ADDR, DEFM_DRPDEFM_CTRL_PC_ID_OFFSET, DEFM_DRPDEFM_CTRL_PC_ID_MASK, "ctrl_pc_id"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_MASK, "dbs_latency"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_MASK, "dbs_alignment"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_MASK, "dbs_overflow"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_MASK, "dbs_underflow"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_MASK, "dbs_regular"},
	{DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_ADDR, DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_OFFSET, DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_MASK, "dbs_rwin"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_LATENCY_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_LATENCY_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_LATENCY_MASK, "cbs_latency"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_ALIGNMENT_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_ALIGNMENT_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_ALIGNMENT_MASK, "cbs_alignment"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_OVERFLOW_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_OVERFLOW_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_OVERFLOW_MASK, "cbs_overflow"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_UNDERFLOW_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_UNDERFLOW_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_UNDERFLOW_MASK, "cbs_underflow"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_REGULAR_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_REGULAR_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_REGULAR_MASK, "cbs_regular"},
	{DEFM_DRPDEFM_CTRL_BUFFER_STATE_RWIN_ADDR, DEFM_DRPDEFM_CTRL_BUFFER_STATE_RWIN_OFFSET, DEFM_DRPDEFM_CTRL_BUFFER_STATE_RWIN_MASK, "cbs_rwin"}
};

/*****************************************************************************/
/**
*
* Returns help strings for framing commands.
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
int framing_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "framing help:\n");

	for (i = 0; framing_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", framing_cmds[i].cmd, framing_cmds[i].helptxt);
	}
	return 0;
}

/*****************************************************************************/
/**
*
* Sets field in framing register.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0 Success
*       - 1 Not enough arguments
*       - 2 Register field not found
*
******************************************************************************/
int framing_set_fram_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	int addr = 0;
	uint32_t buf = 0;
	uint32_t data =0;
	uint32_t antenna = 0;
	char *str = resp;
	
	if (argc < 3)
	{
		sprintf(str, "\t%s", FRAMING_SET_FRAM_STR);
		return(1);
	}

	syslog(LOG_ERR, "%s:%d set_fram: antenna: %s, register: %s, value: %s\n", __FILE__, __LINE__, argv[0], argv[1], argv[2]);

	data = (uint32_t)strtol(argv[2], NULL, 0);
	antenna = (uint32_t)strtol(argv[0], NULL, 0);

	syslog(LOG_ERR, "%s:%d set_fram: antenna: %d, value: %08x\n", __FILE__, __LINE__, antenna, data);

	for(count=0; count < FRAMING_FRAMER_REG_NUM && !found; count++)
	{
		if(strcmp(argv[1], framer_regs[count].name)==0)
		{
			found = 1;
			addr = framer_regs[count].addr+(antenna*FRAMING_REG_SIZE);
			/* Do a read/modify/write on the given register */
			IP_API_Read(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
			syslog(LOG_ERR, "%s:%d set_fram: addr = %08x, mask = %08x, offset = %d, read value = %08x\n", __FILE__, __LINE__, addr, framer_regs[count].mask, framer_regs[count].offset, buf);
			buf &= ~framer_regs[count].mask;
			syslog(LOG_ERR, "%s:%d set_fram: intermediate value = %08x\n", __FILE__, __LINE__, buf);
			buf |= ((data << framer_regs[count].offset) & framer_regs[count].mask);
			syslog(LOG_ERR, "%s:%d set_fram: addr = %08x, mask = %08x, offset = %d, data = %08x, write value = %08x\n", __FILE__, __LINE__, addr, framer_regs[count].mask, framer_regs[count].offset, data, buf);
			IP_API_Write(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
		}
	}
	
	if(!found)
	{
		str += sprintf(str, "Register %s not found, try \"help\"\n", argv[1]);
		return(2);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Gets field from framing register.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0 Success
*       - 1 Not enough arguments
*       - 2 Register field not found
*
******************************************************************************/
int framing_get_fram_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	int addr = 0;
	uint32_t buf = 0;
	uint32_t antenna = 0;
	char *str = resp;

	if (argc < 2)
	{
		sprintf(str, "\t%s", FRAMING_GET_FRAM_STR);
		return(1);
	}

	antenna = (uint32_t)strtol(argv[0], NULL, 0);

	for(count=0; count < FRAMING_FRAMER_REG_NUM && !found; count++)
	{
		if(strcmp(argv[1], framer_regs[count].name)==0)
		{
			found = 1;
			/* Read the given register */
			addr = framer_regs[count].addr+(antenna*FRAMING_REG_SIZE);
			IP_API_Read(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
			syslog(LOG_ERR, "%s:%d get_fram : addr = %08x, mask = %08x, offset = %d, value = %08x\n", __FILE__, __LINE__, addr, framer_regs[count].mask, framer_regs[count].offset, buf);
			buf &= framer_regs[count].mask;
			buf >>= framer_regs[count].offset;
			str += sprintf(str, "%s:0x%08x\n", argv[1], buf);
		}
	}
	
	if(!found)
	{
		str += sprintf(str, "Register %s not found, try \"help\"\n", argv[1]);
		return(2);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Sets field in deframing register.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0 Success
*       - 1 Not enough arguments
*       - 2 Register field not found
*
******************************************************************************/
int framing_set_defr_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	int addr = 0;
	uint32_t buf = 0;
	uint32_t data =0;
	uint32_t antenna = 0;
	char *str = resp;
	
	if (argc < 3)
	{
		sprintf(str, "\t%s", FRAMING_SET_DEFR_STR);
		return(1);
	}

	data = (uint32_t)strtol(argv[2], NULL, 0);
	antenna = (uint32_t)strtol(argv[0], NULL, 0);

	for(count=0; count < FRAMING_DEFRAMER_WR_REG_NUM && !found; count++)
	{
		if(strcmp(argv[1], deframer_regs[count].name)==0)
		{
			found = 1;
			addr = deframer_regs[count].addr+(antenna*FRAMING_REG_SIZE);
			/* Do a read/modify/write on the given register */
			IP_API_Read(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
			syslog(LOG_ERR, "%s:%d set_defr: addr = %08x, mask = %08x, offset = %d, read value = %08x\n", __FILE__, __LINE__, addr, deframer_regs[count].mask, deframer_regs[count].offset, buf);
			buf &= ~deframer_regs[count].mask;
			buf |= ((data << deframer_regs[count].offset) & deframer_regs[count].mask);
			syslog(LOG_ERR, "%s:%d set_defr: addr = %08x, mask = %08x, offset = %d, write value = %08x\n", __FILE__, __LINE__, addr, deframer_regs[count].mask, deframer_regs[count].offset, buf);
			IP_API_Write(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
		}
	}
	
	if(!found)
	{
		str += sprintf(str, "Register %s not found, try \"help\"\n", argv[1]);
		return(2);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Gets field from framing register.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0 Success
*       - 1 Not enough arguments
*       - 2 Register field not found
*
******************************************************************************/
int framing_get_defr_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	int addr = 0;
	uint32_t buf = 0;
	uint32_t antenna = 0;
	char *str = resp;

	if (argc < 2)
	{
		sprintf(str, "\t%s", FRAMING_GET_DEFR_STR);
		return(1);
	}

	antenna = (uint32_t)strtol(argv[0], NULL, 0);

	for(count=0; count < FRAMING_DEFRAMER_REG_NUM && !found; count++)
	{
		if(strcmp(argv[1], deframer_regs[count].name)==0)
		{
			found = 1;
			addr = deframer_regs[count].addr+(antenna*FRAMING_REG_SIZE);
			/* Read the given register */
			IP_API_Read(addr, (uint8_t *)&buf, FRAMING_REG_SIZE);
			syslog(LOG_ERR, "%s:%d get_defr: addr = %08x, mask = %08x, offset = %d, value = %08x\n", __FILE__, __LINE__, addr, deframer_regs[count].mask, deframer_regs[count].offset, buf);
			buf &= deframer_regs[count].mask;
			buf >>= deframer_regs[count].offset;
			str += sprintf(str, "%s:0x%08x\n", argv[1], buf);
		}
	}
	
	if(!found)
	{
		str += sprintf(str, "Register %s not found, try \"help\"\n", argv[1]);
		return(2);
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
int framing_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if (argc == 0)
	{
		sprintf(str, "\t%s", FRAMING_USAGE_STR);
		return(1);
	}
	
	for(count=0; framing_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], framing_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			framing_cmds[count].func(argc-1, &argv[1], resp);
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
