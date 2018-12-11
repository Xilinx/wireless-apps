// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file radio_ctrl.c
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
#include <radio_ctrl_str.h>
#include <roe_radio_ctrl.h>
#include <roe_framer_ctrl.h>
#include <errno.h>
#include <inttypes.h>
#include <xroe_api.h>

/**
 * RADIO_MAX_COMMANDS Number of commands handled by the radio_ctrl module.
 */
#define RADIO_MAX_COMMANDS 9

/**
 * MAX_NUMBER_OF_ANTENNAS Number of antennae supported by the radio_ctrl module.
 */
#define MAX_NUMBER_OF_ANTENNAS 8

/************************** Function Prototypes ******************************/
int RADIO_CTRL_CalulateBufStateLatency(int index, unsigned int *pBufStateLatency);
int radio_ctrl_update_values(void);

int radio_ctrl_help_func(int argc, char **argv, char *resp);
int radio_ctrl_radio_id_func(int argc, char **argv, char *resp);
int radio_ctrl_status_func(int argc, char **argv, char *resp);
int radio_ctrl_loopback_en_func(int argc, char **argv, char *resp);
int radio_ctrl_loopback_dis_func(int argc, char **argv, char *resp);
int radio_ctrl_gui_func(int argc, char **argv, char *resp);
int radio_ctrl_peek_func(int argc, char **argv, char *resp);
int radio_ctrl_poke_func(int argc, char **argv, char *resp);

/**
 * radio_ctrl_cmds The commands handled by the radio_ctrl module.
 */
commands_t radio_ctrl_cmds[RADIO_MAX_COMMANDS] = {
	/* Keep this first */
	{"help", RADIO_CTRL_HELP_STR, radio_ctrl_help_func},
	{"status", RADIO_CTRL_STATUS_STR, radio_ctrl_status_func},
	{"loopback_en", RADIO_CTRL_LOOPBACK_EN_STR, radio_ctrl_loopback_en_func},
	{"loopback_dis", RADIO_CTRL_LOOPBACK_DIS_STR, radio_ctrl_loopback_dis_func},
	{"id", RADIO_CTRL_RADIO_ID_STR, radio_ctrl_radio_id_func},
	{"gui", RADIO_CTRL_GUI_STR, radio_ctrl_gui_func},
	{"peek", RADIO_CTRL_PEEK_STR, radio_ctrl_peek_func},
	{"poke", RADIO_CTRL_POKE_STR, radio_ctrl_poke_func},
	/* Insert commands here */
	/* Keep this last - insert commands above */
	{NULL, NULL, NULL}
};

/**
 * single_antenna_status_struct Status values for an antenna.
 */
typedef struct single_antenna_status_struct{
	unsigned int Align;
	unsigned int Regular;
	unsigned int Overflow;
	unsigned int Underflow;
	unsigned int CheckError;
	unsigned int BufStateLatency;
} single_antenna_status_struct;

/**
 * antennas_status_struct Status values for an antenna array.
 */
typedef struct antennas_status_struct{
single_antenna_status_struct Antenna[MAX_NUMBER_OF_ANTENNAS];
int NumOfAntennas;
} antennas_status_struct;


/**
 * radio_ctrl_struct Control/status structure for the radio.
 */
typedef struct radio_ctrl_struct{
	unsigned int Enable;
	unsigned int Error;
	unsigned int Status;
	unsigned int Loopback;
} radio_ctrl_struct;

/**
 * RadioStatus Radio control/status variable.
 */
static radio_ctrl_struct RadioStatus;

/**
 * AntennasStatus Antennae status variable.
 */
static antennas_status_struct AntennasStatus;

/*****************************************************************************/
/**
*
* Gets antennae and radio status in JSON format into resp for GUI display.
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
int radio_ctrl_gui_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;
	int i;

	read = radio_ctrl_update_values();
	if(!read)
	{
		str += sprintf(str, "{\"Enable\": %d, \"Error\": %d, \"Status\": %d, \"Loopback\": %d"
		,RadioStatus.Enable,
		RadioStatus.Error,
		RadioStatus.Status,
		RadioStatus.Loopback);

		//for(i=0; i<AntennasStatus.NumOfAntennas; i++)
		for(i=0; i<MAX_NUMBER_OF_ANTENNAS; i++)
		{
			str += sprintf(str, ", \"Antenna%d\": {\"Align\": %u, \"Regular\": %u, \"Overflow\": %u, \"Underflow\": %u, \"CheckError\": %u, \"BufStateLatency\": %u}"
			,i,
			AntennasStatus.Antenna[i].Align,
			AntennasStatus.Antenna[i].Regular,
			AntennasStatus.Antenna[i].Overflow,
			AntennasStatus.Antenna[i].Underflow,
			AntennasStatus.Antenna[i].CheckError,
			AntennasStatus.Antenna[i].BufStateLatency);
		}
		str += sprintf(str, "}\n");
	}

	else
	{
		str += sprintf(str, "/dev/xroe/radio_ctrl not opened\n");
	}


	return 0;

}


/*****************************************************************************/
/**
*
* Enables loopback functionality on the radio module.
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
int radio_ctrl_loopback_en_func(int argc, char **argv, char *resp)
{
	int ret = 0;
	char *str = resp;

	ret = RADIO_CTRL_API_Write_Register(RADIO_CDC_LOOPBACK_ADDR, 1, RADIO_CDC_LOOPBACK_MASK, RADIO_CDC_LOOPBACK_OFFSET);

	if(ret)
	{
		str += sprintf(str, "/dev/xroe/radio_ctrl not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Disables loopback functionality on the radio module.
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
int radio_ctrl_loopback_dis_func(int argc, char **argv, char *resp)
{
	int ret = 0;
	char *str = resp;

	ret = RADIO_CTRL_API_Write_Register(RADIO_CDC_LOOPBACK_ADDR, 0, RADIO_CDC_LOOPBACK_MASK, RADIO_CDC_LOOPBACK_OFFSET);

	if(ret)
	{
		str += sprintf(str, "/dev/xroe/radio_ctrl not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Gets antennae and radio status in text format into resp.
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
int radio_ctrl_status_func(int argc, char **argv, char *resp)
{
	int read = 0;
	int i;
	char *str = resp;

	read = radio_ctrl_update_values();
	if(!read)
	{
		str += sprintf(str, "\nEnable: %u\n",  RadioStatus.Enable);
		str += sprintf(str, "Error: %u\n", RadioStatus.Error);
		str += sprintf(str, "Status: %u\n", RadioStatus.Status);
		str += sprintf(str, "Loopback: %u\n", RadioStatus.Loopback);

		//for(i=0; i<AntennasStatus.NumOfAntennas; i++)
		for(i=0; i<MAX_NUMBER_OF_ANTENNAS; i++)
		{
			str += sprintf(str, "\nAntenna no.%d\n",  i);

			str += sprintf(str, "Align: %u\n",  AntennasStatus.Antenna[i].Align);
			str += sprintf(str, "Regular: %u\n",  AntennasStatus.Antenna[i].Regular);
			str += sprintf(str, "Overflow: %u\n", AntennasStatus.Antenna[i].Overflow);
			str += sprintf(str, "Underflow: %u\n", AntennasStatus.Antenna[i].Underflow);
			str += sprintf(str, "CheckError: %u\n", AntennasStatus.Antenna[i].CheckError);
			str += sprintf(str, "BufStateLatency: %u\n", AntennasStatus.Antenna[i].BufStateLatency);
		}
	}

	else
	{
		str += sprintf(str, "/dev/xroe/radio_ctrl not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Gets radio ID into resp.
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
int radio_ctrl_radio_id_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;
	unsigned int radioIDTag = 0;

	read = RADIO_CTRL_API_Read_Register(RADIO_ID_ADDR, &radioIDTag, RADIO_ID_MASK, RADIO_ID_OFFSET);
	if(!read)
	{
		str += sprintf(str, "\nRADIO_ID: 0x%08x\n", radioIDTag);
	}

	else
	{
		str += sprintf(str, "/dev/xroe/radio_ctrl not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Returns help strings for radio_ctrl commands.
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
int radio_ctrl_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "radio_ctrl help:\n");

	for(i=0; radio_ctrl_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", radio_ctrl_cmds[i].cmd, radio_ctrl_cmds[i].helptxt);
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
int radio_ctrl_peek_func(int argc, char **argv, char *resp)
{
	int buf;
	int addr;
	int read = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", RADIO_CTRL_PEEK_STR);
		return(1);
	}

	addr = strtol(argv[0], NULL, 0);

	read = RADIO_CTRL_API_Read(addr, (uint8_t *)&buf, sizeof(buf));
	if(!read)
	{
		str += sprintf(str, "peek: 0x%08x : 0x%08x\n", addr, buf);
	}
	else
	{
		str += sprintf(str, "radio_ctrl peek 0x%08x: error %d\n", addr, read);
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
int radio_ctrl_poke_func(int argc, char **argv, char *resp)
{
	int buf;
	int addr;
	int write = 0;
	char *str = resp;

	if(argc < 2)
	{
		sprintf(str, "\t%s", RADIO_CTRL_POKE_STR);
		return(1);
	}

	addr = strtol(argv[0], NULL, 0);
	buf = strtol(argv[1], NULL, 0);

	str += sprintf(str, "addr: 0x%08x, value: 0x%08x\n", addr, buf);

	write = RADIO_CTRL_API_Write(addr, (uint8_t *)&buf, sizeof(buf));
	if(!write)
	{
		str += sprintf(str, "poke: 0x%08x : 0x%08x\n", addr, buf);
	}
	else
	{
		str += sprintf(str, "radio_ctrl poke 0x%08x: error %d\n", addr, write);
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
int radio_ctrl_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if(argc == 0)
	{
		sprintf(str, "\t%s", RADIO_CTRL_USAGE_STR);
		return(1);
	}

	for(count=0; radio_ctrl_cmds[count].cmd != NULL; count++)
	{
		if(strcmp(argv[0], radio_ctrl_cmds[count].cmd)==0)
		{
			found = 1;
			/* Call the handler function for the command given */
			radio_ctrl_cmds[count].func(argc-1, &argv[1], resp);
		}
	}

	if(!found)
	{
		str += sprintf(str, "Command %s not found, try \"help\"\n", argv[0]);
		return(2);
	}

	return 0;
}

/*****************************************************************************/
/**
*
* Updates the internal radio and antennae status variables.
* 
*
* @return
*		- Return value of RADIO_CTRL_API_Read_Register() on error.
*		- Return value of IP_API_Read_Register() on error.
*		- Return value of RADIO_CTRL_CalulateBufStateLatency() on error.
*		- Return value of RADIO_CTRL_API_Read_Register() otherwise.
*
******************************************************************************/
int radio_ctrl_update_values(void)
{
	int ret = -1;
	int i;
	AntennasStatus.NumOfAntennas = MAX_NUMBER_OF_ANTENNAS; // hardcoded "8" for the Demo

	ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_ENABLE_ADDR, &RadioStatus.Enable, RADIO_CDC_ENABLE_MASK, RADIO_CDC_ENABLE_OFFSET);
	if(!ret)
	{
		ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_ERROR_ADDR, &RadioStatus.Error, RADIO_CDC_ERROR_MASK, RADIO_CDC_ERROR_OFFSET);
	}
	if(!ret)
	{
		ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_STATUS_ADDR, &RadioStatus.Status, RADIO_CDC_STATUS_MASK, RADIO_CDC_STATUS_OFFSET);
	}
	if(!ret)
	{
		ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_LOOPBACK_ADDR, &RadioStatus.Loopback, RADIO_CDC_LOOPBACK_MASK, RADIO_CDC_LOOPBACK_OFFSET);
	}


	if(!ret)
	{
		for(i=0; i<AntennasStatus.NumOfAntennas; i++)
		{
			ret = IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_ADDR + (i*4), &AntennasStatus.Antenna[i].Align, DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_ALIGNMENT_OFFSET);
			if(!ret)
			{
				ret = IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_ADDR + (i*4), &AntennasStatus.Antenna[i].Regular, DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_REGULAR_OFFSET);
			}
			if(!ret)
			{
				ret = IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_ADDR + (i*4), &AntennasStatus.Antenna[i].Overflow, DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_OVERFLOW_OFFSET);
			}
			if(!ret)
			{
				ret = IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_ADDR + (i*4), &AntennasStatus.Antenna[i].Overflow, DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_UNDERFLOW_OFFSET);
			}
			if(!ret)
			{
				ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_ERROR_31_0_ADDR, &AntennasStatus.Antenna[i].CheckError, RADIO_CDC_ERROR_31_0_MASK, RADIO_CDC_ERROR_31_0_OFFSET + i);
			}
			if(!ret)
			{
				ret = RADIO_CTRL_CalulateBufStateLatency(i, &AntennasStatus.Antenna[i].BufStateLatency);
			}
			if(ret)
			{
				break;
			}
		}
	}

	if(!ret)
	{
		ret = RADIO_CTRL_API_Read_Register(RADIO_CDC_LOOPBACK_ADDR, &RadioStatus.Loopback, RADIO_CDC_LOOPBACK_MASK, RADIO_CDC_LOOPBACK_OFFSET);
	}

	return ret;
}


/*****************************************************************************/
/**
*
* Calculates the buffer state latency for a given antenna.
* 
*
* @param [in]	index   			Index of the antenna to calculate for.
* @param [out]	pBufStateLatency	Pointer to place result in.
*
* @return
*		- -1 if index out of range.
*		- Return value of IP_API_Read_Register() otherwise.
*
******************************************************************************/
int RADIO_CTRL_CalulateBufStateLatency(int index, unsigned int *pBufStateLatency)
{
	int ret = 0;
	unsigned int readValue = 0;
	unsigned int Defm_Rwin = 0;
	unsigned int Defm_Pdu = 0;

	if(index<0 || index>=MAX_NUMBER_OF_ANTENNAS)
	{
		ret = -1;
	}

	if(!ret)
	{
		ret =	IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_ADDR + (index*4), &readValue, DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_RWIN_OFFSET);
	}
	if(!ret)
	{
		Defm_Rwin = readValue;
		ret =	IP_API_Read_Register(DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_ADDR + (index*4), &readValue, DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_MASK, DEFM_DRPDEFM_DATA_BUFFER_STATE_LATENCY_OFFSET);
	}

	if(!ret)
	{
		Defm_Pdu = readValue;
		*pBufStateLatency = (2*Defm_Rwin + 2) * Defm_Pdu;
	}

	return ret;
}
/** @} */