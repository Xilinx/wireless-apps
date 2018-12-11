// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file client.c
* @addtogroup comms_lib
* @{
*
*  A sample communication module for UNIX, UDP/IP and TCP/IP sockets.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <netinet/in.h>

#include <client.h>
#include <comms.h>

/*****************************************************************************/
/**
*
* Sends commands to an instance of the sample application.
* This function takes a user-supplied command string and send it either to the
* sample application listening on the local UNIX socket, or if addr is not NULL
* then the sample application listening on the remore TCP/IP socket at addr.
* 
*
* @param [in]	addr   	Address of remote application to send command to.
* @param [in]	port   	Port of remote application.
* @param [in]	command Pointer to command string.
*
* @return
*		- 0
*
******************************************************************************/
int client_send_message(in_addr_t addr, int port, char *command)
{
	struct sockaddr *serv_addr;
	struct sockaddr_in in_serv_addr;
	struct sockaddr_un un_serv_addr;
	int sockfd, servlen, n;
	char buffer[MAX_RESPONSE_LENGTH];

	if (addr)
	{
		/* Remote network connection */
		bzero((char *)&in_serv_addr, sizeof(in_serv_addr));
		in_serv_addr.sin_family = AF_INET;
		in_serv_addr.sin_addr.s_addr = addr;
		in_serv_addr.sin_port = htons(port ? port : XROE_COMM_PORT);
		servlen = sizeof(in_serv_addr);

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("Creating socket");
			exit(0);
		}

		serv_addr = (struct sockaddr *)&in_serv_addr;
	}
	else /* Unix file socket */
	{
		bzero((char *)&un_serv_addr, sizeof(un_serv_addr));
		un_serv_addr.sun_family = AF_UNIX;
		strcpy(un_serv_addr.sun_path, XROE_SOCKET_FILE);
		servlen = strlen(un_serv_addr.sun_path) + sizeof(un_serv_addr.sun_family);

		if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		{
			perror("Creating socket");
			exit(0);
		}

		serv_addr = (struct sockaddr *)&un_serv_addr;
	}

	/* Connect to daemon */
	if (connect(sockfd, serv_addr, servlen) < 0)
	{
		perror("Connecting");
		exit(0);
	}
	bzero(buffer, MAX_RESPONSE_LENGTH);

	/* Send message to daemon and read response */
	write(sockfd, command, strlen(command));
	n = read(sockfd, buffer, MAX_RESPONSE_LENGTH);
	write(1, buffer, n);
	close(sockfd);

	return 0;
}

/** @} */