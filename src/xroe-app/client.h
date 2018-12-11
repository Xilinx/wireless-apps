// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file client.h
* @addtogroup comms_lib
* @{
*
*  A sample communication module for UNIX, UDP/IP and TCP/IP sockets.
*
******************************************************************************/
/************************** Function Prototypes ******************************/
int client_send_message(in_addr_t in_addr, int port, char *command);
/** @} */