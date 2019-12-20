// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file stats.c
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
#include <errno.h>
#include <inttypes.h>

#include <xroe_types.h>
#include <stats_str.h>
#include <roe_framer_ctrl.h>
#include <xroe_api.h>

/**
 * STATS_MAX_COMMANDS Number of commands handled by the stats module.
 */
#define STATS_MAX_COMMANDS 8

/**
 * total_packets_struct Totals packets count.
 */
typedef struct total_packets_struct {
	unsigned int GoodPacketsCount;
	unsigned int BadPacketsCount;
	unsigned int PacketsWithBadFCSCount;
} total_packets_struct;

/**
 * packets_struct Packet type count.
 */
typedef struct packets_struct {
	unsigned int TotalPacketsCount;
	unsigned int GoodPacketsCount;
	unsigned int BadPacketsCount;
	unsigned int PacketsWithBadFCSCount;
} packets_struct;

/**
 * stats_struct Statistics structure.
 */
typedef struct stats_struct {
	total_packets_struct TotalPackets;
	packets_struct UserPackets;
	packets_struct ControlPackets;
	unsigned int DataPacketsRate;
	unsigned int ControlPacketsRate;
	unsigned int FramerRestartCount;
	unsigned int FramerEnable;
	unsigned int DeFramerEnable;
	unsigned int XXV_Reset;
} stats_struct;

/**
 * Stats Statistics variable.
 */
static stats_struct Stats;

/************************** Function Prototypes ******************************/
int stats_help_func(int argc, char **argv, char *resp);
int stats_sw_func(int argc, char **argv, char *resp);
int stats_user_func(int argc, char **argv, char *resp);
int stats_ctrl_func(int argc, char **argv, char *resp);
int stats_rate_func(int argc, char **argv, char *resp);
int stats_all_func(int argc, char **argv, char *resp);
int stats_all_gui_func(int argc, char **argv, char *resp);

int stats_update_values(void);

/**
 * stats_cmds The commands handled by the stats module.
 */
commands_t stats_cmds[STATS_MAX_COMMANDS] = {
	/* Keep this first */
	{ "help", STATS_HELP_STR, stats_help_func },
	/* Insert commands here */
	{ "sw", STATS_SW_STR, stats_sw_func },
	{ "user", STATS_USER_STR, stats_user_func },
	{ "control", STATS_CTRL_STR, stats_ctrl_func },
	{ "rate", STATS_RATE_STR, stats_rate_func },
	{ "all", STATS_ALL_STR, stats_all_func },
	{ "gui", STATS_GUI_NUM_STR, stats_all_gui_func },
	/* Keep this last - insert commands above */
	{ NULL, NULL, NULL }
};

/*****************************************************************************/
/**
*
* Dummy function, returns the help text.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_sw_func(int argc, char **argv, char *resp)
{
	sprintf(resp, STATS_SW_STR);
	return 0;
}

/*****************************************************************************/
/**
*
* Reads user packet statistics.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_user_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;

	read = stats_update_values();
	if (!read)
	{
		str += sprintf(str, "\nTotal user data packets count: %u\n", Stats.UserPackets.TotalPacketsCount);
		str += sprintf(str, "Good user data packets: %u\n", Stats.UserPackets.GoodPacketsCount);
		str += sprintf(str, "Bad user data packets: %u\n", Stats.UserPackets.BadPacketsCount);
		str += sprintf(str, "User data packets with bad FCS: %u\n", Stats.UserPackets.PacketsWithBadFCSCount);
	}

	else
	{
		str += sprintf(str, "/dev/xroe/stats not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Reads control packet statistics.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_ctrl_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;

	read = stats_update_values();
	if (!read)
	{
		str += sprintf(str, "Total control packets: %u\n", Stats.ControlPackets.TotalPacketsCount);
		str += sprintf(str, "Good control packets: %u\n", Stats.ControlPackets.GoodPacketsCount);
		str += sprintf(str, "Bad control packets: %u\n", Stats.ControlPackets.BadPacketsCount);
		str += sprintf(str, "Control packets with bad FCS: %u\n", Stats.ControlPackets.PacketsWithBadFCSCount);
	}

	else
	{
		str += sprintf(str, "/dev/xroe/stats not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Reads data and control packet rates.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_rate_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;

	read = stats_update_values();
	if (!read)
	{
		str += sprintf(str, "Data packets rate: %u\n", Stats.DataPacketsRate);
		str += sprintf(str, "Control packets rate: %u\n\n", Stats.ControlPacketsRate);
	}

	else
	{
		str += sprintf(str, "/dev/xroe/stats not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Reads all statistics.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_all_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;

	read = stats_update_values();
	if (!read)
	{
		str += sprintf(str, "Total packets count: %u\n", Stats.TotalPackets.GoodPacketsCount + Stats.TotalPackets.BadPacketsCount);
		str += sprintf(str, "Good packets: %u\n", Stats.TotalPackets.GoodPacketsCount);
		str += sprintf(str, "Bad packets: %u\n", Stats.TotalPackets.BadPacketsCount);
		str += sprintf(str, "Total packets with bad FCS: %u\n", Stats.TotalPackets.PacketsWithBadFCSCount);

		str += sprintf(str, "\nTotal user data packets count: %u\n", Stats.UserPackets.TotalPacketsCount);
		str += sprintf(str, "Good user data packets: %u\n", Stats.UserPackets.GoodPacketsCount);
		str += sprintf(str, "Bad user data packets: %u\n", Stats.UserPackets.BadPacketsCount);
		str += sprintf(str, "User data packets with bad FCS: %u\n", Stats.UserPackets.PacketsWithBadFCSCount);

		str += sprintf(str, "\nTotal control packets: %u\n", Stats.ControlPackets.TotalPacketsCount);
		str += sprintf(str, "Good control packets: %u\n", Stats.ControlPackets.GoodPacketsCount);
		str += sprintf(str, "Bad control packets: %u\n", Stats.ControlPackets.BadPacketsCount);
		str += sprintf(str, "Control packets with bad FCS: %u\n", Stats.ControlPackets.PacketsWithBadFCSCount);

		str += sprintf(str, "\nData packets rate: %u\n", Stats.DataPacketsRate);
		str += sprintf(str, "Control packets rate: %u\n\n", Stats.ControlPacketsRate);


	}

	else
	{
		str += sprintf(str, "/dev/xroe/stats not opened\n");
	}

	return 0;

}

/*****************************************************************************/
/**
*
* Reads all statistics and returns them in JSON format for GUI display.
* 
*
* @param [in]	argc   Number of string arguments.
* @param [in]	argv   Array of strings containg arguments.
* @param [out]	resp   Pointer to string to place response text in.
*
* @return
*		- 0.
*
******************************************************************************/
int stats_all_gui_func(int argc, char **argv, char *resp)
{
	int read = 0;
	char *str = resp;

	read = stats_update_values();
	if (!read)
	{
		str += sprintf(str, "{\"DataPacketsRate\": %u, \"UserPackets\": {\"PacketsWithBadFCSCount\": %u, \"BadPacketsCount\": %u, \"TotalPacketsCount\": %u, \"GoodPacketsCount\": %u}, \"ControlPacketsRate\": %u, \"FramerRestartCount\": %u, \"FramerEnable\": %u, \"DeFramerEnable\": %u, \"XXV_Reset\": %u, \"ControlPackets\": {\"PacketsWithBadFCSCount\": %u, \"BadPacketsCount\": %u, \"TotalPacketsCount\": %u, \"GoodPacketsCount\": %u}, \"TotalPackets\": {\"PacketsWithBadFCSCount\": %u, \"BadPacketsCount\": %u, \"GoodPacketsCount\": %u}}\n"
			, Stats.DataPacketsRate,
			Stats.UserPackets.PacketsWithBadFCSCount,
			Stats.UserPackets.BadPacketsCount,
			Stats.UserPackets.TotalPacketsCount,
			Stats.UserPackets.GoodPacketsCount,
			Stats.ControlPacketsRate,
			Stats.FramerRestartCount,
			Stats.FramerEnable,
			Stats.DeFramerEnable,
			Stats.XXV_Reset,
			Stats.ControlPackets.PacketsWithBadFCSCount,
			Stats.ControlPackets.BadPacketsCount,
			Stats.ControlPackets.TotalPacketsCount,
			Stats.ControlPackets.GoodPacketsCount,
			Stats.TotalPackets.PacketsWithBadFCSCount,
			Stats.TotalPackets.BadPacketsCount,
			Stats.TotalPackets.GoodPacketsCount);
	}

	else
	{
		str += sprintf(str, "/dev/xroe/stats not opened\n");
	}


	return 0;

}


/*****************************************************************************/
/**
*
* Returns help strings for stats commands.
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
int stats_help_func(int argc, char **argv, char *resp)
{
	int i;
	char *str = resp;

	str += sprintf(str, "stats help:\n");

	for (i = 0; stats_cmds[i].cmd != NULL; i++)
	{
		str += sprintf(str, "\t%s\t : %s", stats_cmds[i].cmd, stats_cmds[i].helptxt);
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
int stats_func(int argc, char **argv, char *resp)
{
	int count = 0;
	int found = 0;
	char *str = resp;

	if (argc == 0)
	{
		sprintf(str, "\t%s", STATS_USAGE_STR);
		return(1);
	}

	for (count = 0; stats_cmds[count].cmd != NULL; count++)
	{
		if (strcmp(argv[0], stats_cmds[count].cmd) == 0)
		{
			found = 1;
			/* Call the handler function for the command given */
			stats_cmds[count].func(argc - 1, &argv[1], resp);
		}
	}

	if (!found)
	{
		str += sprintf(str, "Command %s not found, try \"help\"\n", argv[0]);
		return(2);
	}

	return 0;
}


/*****************************************************************************/
/**
*
* Updates the internal status variables.
* 
*
* @return
*		- Return value of STATS_SYSFS_API_Read() on error.
*		- Return value of IP_API_Read_Register() on error.
*		- Return value of IP_API_Read_Register() otherwise.
*
******************************************************************************/
int stats_update_values(void)
{
	int ret = -1;
	char buff[256];

	ret = STATS_SYSFS_API_Read("total_rx_good_pkt", buff);
	Stats.TotalPackets.GoodPacketsCount = strtoul(buff, NULL, 0);
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_pkt", buff);
		Stats.TotalPackets.BadPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_fcs", buff);
		Stats.TotalPackets.PacketsWithBadFCSCount = strtoul(buff, NULL, 0);
	}

	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_user_pkt", buff);
		Stats.UserPackets.TotalPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_good_user_pkt", buff);
		Stats.UserPackets.GoodPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_user_pkt", buff);
		Stats.UserPackets.BadPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_user_fcs", buff);
		Stats.UserPackets.PacketsWithBadFCSCount = strtoul(buff, NULL, 0);
	}

	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_user_ctrl_pkt", buff);
		Stats.ControlPackets.TotalPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_good_user_ctrl_pkt", buff);
		Stats.ControlPackets.GoodPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_user_ctrl_pkt", buff);
		Stats.ControlPackets.BadPacketsCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("total_rx_bad_user_ctrl_fcs", buff);
		Stats.ControlPackets.PacketsWithBadFCSCount = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("rx_user_pkt_rate", buff);
		Stats.DataPacketsRate = strtoul(buff, NULL, 0);
	}
	if (!ret)
	{
		ret = STATS_SYSFS_API_Read("rx_user_ctrl_pkt_rate", buff);
		Stats.ControlPacketsRate = strtoul(buff, NULL, 0);
	}


	if (!ret)
	{
		ret = IP_API_Read_Register(FRAM_AUTO_RESTART_CNT_ADDR, &Stats.FramerRestartCount, FRAM_AUTO_RESTART_CNT_MASK, FRAM_AUTO_RESTART_CNT_OFFSET);
	}

	if (!ret)
	{
		ret = IP_API_Read_Register(FRAM_RESTART_ADDR, &Stats.FramerEnable, FRAM_RESTART_MASK, FRAM_RESTART_OFFSET);
	}

	if (!ret)
	{
		ret = IP_API_Read_Register(DEFM_RESTART_ADDR, &Stats.DeFramerEnable, DEFM_RESTART_MASK, DEFM_RESTART_OFFSET);
	}

	if (!ret)
	{
		ret = IP_API_Read_Register(CFG_USER_RW_OUT_ADDR, &Stats.XXV_Reset, CFG_USER_RW_OUT_MASK, CFG_USER_RW_OUT_OFFSET);
	}

	return ret;
}
/** @} */