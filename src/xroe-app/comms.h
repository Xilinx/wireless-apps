// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file comms.h
* @addtogroup comms_lib
* @{
*
*  A sample communication module for UNIX, UDP/IP and TCP/IP sockets.
*
******************************************************************************/

/**
 * XROE_SOCKET_FILE File name for UNIX socket.
 */
#define XROE_SOCKET_FILE "/tmp/xroe"

/**
 * MAX_RESPONSE_LENGTH Length of response string.
 */
#define MAX_RESPONSE_LENGTH 1024

/**
 * ETH_P_ECPRI Protocol number for eCPRI.
 */
#define ETH_P_ECPRI 0xAEFE

/**
 * ETH_P_1914_3 Protocol number for 1914.3.
 */
#define ETH_P_1914_3 0xFC3D

/**
 * XROE_COMM_PORT Default port number for UDP/IP and TCP/IP traffic.
 */
#define XROE_COMM_PORT 0xAEFE

/**
 * NUM_LISTEN_SOCKETS Number of sockets in the open socket list.
 */
#define NUM_LISTEN_SOCKETS 3

/************************** Function Prototypes ******************************/
int open_connections(int nohw, int port);
int get_message(int nohw, char *command);
void send_response(char *response);
void close_connections(int nohw);

extern int sock_ip; /**< File descriptor number for UDP/IP socket */
extern int port_ip; /**< Port number for TCP/IP and UDP/IP socket */
/** @} */