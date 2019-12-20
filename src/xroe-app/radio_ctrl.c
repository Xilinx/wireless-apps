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
#define RADIO_MAX_COMMANDS 7

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
		str += sprintf(str, "/sys/kernel/traffic not opened gui read - radio_ctrl_gui_func\n");
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

	ret = TRAFGEN_SYSFS_API_Write("radio_loopback", "enabled");

	if(ret)
	{
		str += sprintf(str, "Error writing to xroe_traffic_gen/radio_loopback\n");
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

	ret = TRAFGEN_SYSFS_API_Write("radio_loopback", "disabled");

	if(ret)
	{
		str += sprintf(str, "Error writing to xroe_traffic_gen/radio_loopback\n");
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
		str += sprintf(str, "/sys/kernel/traffic not opened radio_ctrl_status_func\n");
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
	char buff[256];

	read = TRAFGEN_SYSFS_API_Read("radio_id", buff);
	if(!read)
	{
		strncat(str, buff, 255);
	}
	else
	{
		str += sprintf(str, "/sys/kernel/traffic/ not opened radio_ctrl_radio_id_func\n");
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
*		- Return value of TRAFGEN_SYSFS_API_Read() on error.
*		- Return value of IP_API_Read_Register() on error.
*		- Return value of RADIO_CTRL_CalulateBufStateLatency() on error.
*		- Return value of TRAFGEN_SYSFS_API_Read() otherwise.
*
/sys/kernel/traffic not opened gui read - radio_ctrl_gui_func
root@om0_pl00:~# find /sys/kernel/traffic
/sys/kernel/traffic
/sys/kernel/traffic/radio_gpio_cdc_ledmode2
/sys/kernel/traffic/radio_cdc_loopback
/sys/kernel/traffic/radio_gpio_cdc_ledgpio
/sys/kernel/traffic/radio_source_enable
/sys/kernel/traffic/radio_sw_trigger
/sys/kernel/traffic/radio_cdc_error_127_96
/sys/kernel/traffic/radio_app_scratch_reg_2
/sys/kernel/traffic/radio_timeout_status
/sys/kernel/traffic/radio_cdc_error_31_0
/sys/kernel/traffic/radio_app_scratch_reg_0
/sys/kernel/traffic/radio_sink_enable
/sys/kernel/traffic/radio_timeout_value
/sys/kernel/traffic/fram_pause_data_size
/sys/kernel/traffic/radio_cdc_error
/sys/kernel/traffic/radio_cdc_error_95_64
/sys/kernel/traffic/radio_cdc_status_127_96
/sys/kernel/traffic/radio_cdc_status_95_64
/sys/kernel/traffic/radio_timeout_enable
/sys/kernel/traffic/radio_cdc_error_63_32
/sys/kernel/traffic/radio_cdc_status
/sys/kernel/traffic/radio_cdc_status_63_32
/sys/kernel/traffic/radio_id
/sys/kernel/traffic/radio_gpio_cdc_dipstatus
/sys/kernel/traffic/fram_packet_data_size
/sys/kernel/traffic/radio_app_scratch_reg_3
/sys/kernel/traffic/radio_cdc_enable
/sys/kernel/traffic/radio_cdc_status_31_0
/sys/kernel/traffic/radio_app_scratch_reg_1

******************************************************************************/
int radio_ctrl_update_values(void)
{
	int ret = -1;
	int i;
	char buff[256];
	unsigned int ant_error[4];

	AntennasStatus.NumOfAntennas = MAX_NUMBER_OF_ANTENNAS; // hardcoded "8" for the Demo


	ret = TRAFGEN_SYSFS_API_Read("radio_source_enable", buff);
	if(!ret)
	{
		RadioStatus.Enable = strtoul(buff, NULL, 0);

		ret = TRAFGEN_SYSFS_API_Read("radio_cdc_error_31_0", buff);
	}
	if(!ret)
	{
		RadioStatus.Error = strtoul(buff, NULL, 0);

		ret = TRAFGEN_SYSFS_API_Read("radio_cdc_status_31_0", buff);
	}
	if(!ret)
	{
		RadioStatus.Status = strtoul(buff, NULL, 0);

		ret = TRAFGEN_SYSFS_API_Read("radio_cdc_loopback", buff);
	}
	if(!ret)
	{
		RadioStatus.Loopback = strtoul(buff, NULL, 0);

		ret = TRAFGEN_SYSFS_API_Read("radio_cdc_error_31_0", buff);
	}
	if(!ret)
	{
		ant_error[0] = strtoul( buff, NULL, 0);
		ant_error[1] = strtoul( "0x0", NULL, 0);
		ant_error[2] = strtoul( "0x0", NULL, 0);
		ant_error[3] = strtoul( "0x0", NULL, 0);

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
				AntennasStatus.Antenna[i].CheckError = (ant_error[i/32] & 1<<(i%32)) >> (i%32);
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
		ret = TRAFGEN_SYSFS_API_Read("radio_cdc_loopback", buff);
		if(!ret)
		{		
			RadioStatus.Loopback = strtoul(buff, NULL, 0);
		}
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