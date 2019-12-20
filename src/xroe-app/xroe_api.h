// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file xroe_api.h
* @addtogroup framer_driver_api
* @{
*
*  Radio over Ethernet Framer driver API library 
*
*
******************************************************************************/
#ifndef XROE_API_H		/* prevent circular inclusions */
#define XROE_API_H		/* by using protection macros */
/************************** Function Prototypes ******************************/
int IP_API_Read(int addr, uint8_t *pRead, int length);
int IP_API_Write(int addr, uint8_t *pWrite, int length);
int IP_API_Read_Register(int addr, unsigned int *pRead, int Mask, int Offset);
int IP_API_Write_Register(int addr, unsigned int pWrite, int Mask, int Offset);
int STATS_SYSFS_API_Read(const char *name, char *resp);
int FRAMER_API_Framer_Restart(int restart);
int FRAMER_API_Deframer_Restart(int restart);
int TRAFGEN_SYSFS_API_Read(const char *name, char *resp);
int TRAFGEN_SYSFS_API_Write(const char *name, char *val);
int XXV_API_Reset(void);
#endif /* end of protection macro */
/** @} */