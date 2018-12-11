// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file comms.c
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
#include <linux/net_tstamp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <poll.h>
#include <errno.h>
#include <linux/sockios.h>
#include <syslog.h>

#include <comms.h>
#include <ecpri_proto.h>

/** @name Communications Variables
 *
 * Variables used to manage communications between applications.
 * @{
 */
int sock_fd; /**< File descriptor number for UNIX file socket */
int sock_tcp; /**< File descriptor number for TCP/IP socket */
int sock_ip; /**< File descriptor number for UDP/IP socket */
int port_ip; /**< Port number for TCP/IP and UDP/IP socket */
int newsockfd; /**< File descriptor number for new incoming socket */
struct pollfd fds[NUM_LISTEN_SOCKETS]; /**< Socket file descriptor array */
/**@}*/

/*****************************************************************************/
/**
*
* Opens listening sockets.
* 
*
* @param [in]	nohw   soft mode, do not open UNIX socket or UDP socket
* @param [in]	port   port to bind TCP/IP and UDP/IP sockets to
*
* @return
*		- 1 on success
*		- -1 on error
*
******************************************************************************/
int open_connections(int nohw, int port)
{
	int servlen;
	struct sockaddr_un fd_serv_addr;
	struct sockaddr_in in_serv_addr;
	int reuse = 1;
	int flags;
	struct ifreq ifreq;
	struct hwtstamp_config cfg;
	int err;

	memset(&ifreq, 0, sizeof(ifreq));
	memset(&cfg, 0, sizeof(cfg));

	if(!nohw)
	{
		/* Create Unix socket for filesystem-based comms */
		if ((sock_fd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
		{
		   syslog(LOG_ERR, "Error creating UNIX socket\n");
		   return(-1);
		}
		
		if(0 == access(XROE_SOCKET_FILE, F_OK))
		{
			unlink(XROE_SOCKET_FILE);
		}

		bzero((char *) &fd_serv_addr, sizeof(fd_serv_addr));
		fd_serv_addr.sun_family = AF_UNIX;
		strcpy(fd_serv_addr.sun_path, XROE_SOCKET_FILE);
		servlen = strlen(fd_serv_addr.sun_path) + sizeof(fd_serv_addr.sun_family);
		if(bind(sock_fd, (struct sockaddr *)&fd_serv_addr, servlen) < 0)
		{
		   syslog(LOG_ERR, "Error binding socket\n");
		   return(-1);
		}
		listen(sock_fd, 5);

		/* Set up poll structure */
		fds[0].fd = sock_fd; 
		fds[0].events = POLLIN;
	}
	else
	{
		sock_fd = -1;
	}
	
	/* Create TCP socket for network comms */
	if ((sock_tcp = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
	   syslog(LOG_ERR, "Error creating TCP socket\n");
	   return(-1);
	}
	
	if(port)
	{
		port_ip = htons(port);
	}
	else
	{
		port_ip = htons(XROE_COMM_PORT);		
	}

	if (setsockopt(sock_tcp, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
	{
	   syslog(LOG_ERR, "setsockopt(SO_REUSEADDR) failed\n");
	   return(-1);
	}

#ifdef SO_REUSEPORT
	if (setsockopt(sock_tcp, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
	{
	   syslog(LOG_ERR, "setsockopt(SO_REUSEPORT) failed\n");
	   return(-1);
	}
#endif		
	
	bzero((char *)&in_serv_addr, sizeof(in_serv_addr));
	in_serv_addr.sin_family = AF_INET;
	in_serv_addr.sin_addr.s_addr = INADDR_ANY;
	in_serv_addr.sin_port = port_ip;
	if (bind(sock_tcp, (struct sockaddr *)&in_serv_addr, sizeof(in_serv_addr)) < 0) 
 	{
	   syslog(LOG_ERR, "Error binding TCP socket\n");
	   return(-1);
	}
	listen(sock_tcp, 5);
	
	/* Set up poll structure */
	fds[1].fd = sock_tcp; 
	fds[1].events = POLLIN;
 
	/* Open IP4 datagram socket for receiving eCPRI messages */
	if((sock_ip = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
	   syslog(LOG_ERR, "Error creating UDP eCPRI socket\n");
	   return(-1);
	}

	if (setsockopt(sock_ip, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
	{
	   syslog(LOG_ERR, "setsockopt(SO_REUSEADDR) failed\n");
	   return(-1);
	}

#ifdef SO_REUSEPORT
	if (setsockopt(sock_ip, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
	{
	   syslog(LOG_ERR, "setsockopt(SO_REUSEPORT) failed\n");
	   return(-1);
	}
#endif		
	
	flags = SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RX_SOFTWARE;
	flags |= SOF_TIMESTAMPING_RAW_HARDWARE | SOF_TIMESTAMPING_SOFTWARE;
#ifdef SOF_TIMESTAMPING_OPT_TX_SWHW
	flags |= SOF_TIMESTAMPING_OPT_TX_SWHW;
#endif
#ifdef 	SOF_TIMESTAMPING_OPT_ID
	flags |= SOF_TIMESTAMPING_OPT_ID;
#endif
#ifdef 	SOF_TIMESTAMPING_OPT_TSONLY
	flags |= SOF_TIMESTAMPING_OPT_TSONLY;
#endif
#ifdef 	SOF_TIMESTAMPING_OPT_CMSG
	flags |= SOF_TIMESTAMPING_OPT_CMSG;
#endif
	
	err = setsockopt(sock_ip, SOL_SOCKET, SO_TIMESTAMPING, &flags, sizeof(flags));
	if ( err < 0) 
	{
		syslog(LOG_ERR, "ioctl SO_TIMESTAMPING failed: %x\n", err);
		return (-1);
	}

	strncpy(ifreq.ifr_name, "eth1", sizeof(ifreq.ifr_name) - 1);

	ifreq.ifr_data = (void *) &cfg;

	cfg.tx_type = HWTSTAMP_TX_ON;
	cfg.rx_filter = HWTSTAMP_FILTER_ALL;

	err = ioctl(sock_ip, SIOCSHWTSTAMP, &ifreq);
	if (err < 0) {
		err = errno;
		syslog(LOG_ERR, "SIOCSHWTSTAMP failed %x\n", err);
		if (err == ERANGE)
			syslog(LOG_ERR, "The requested time stamping mode is not supported by the hardware.\n");
	} else {
		syslog(LOG_ERR, "new settings:\ntx_type %d\nrx_filter %d\n", cfg.tx_type, cfg.rx_filter);
	}

	if(!nohw)
	{
		memset(&ifreq, 0, sizeof(ifreq));
		snprintf(ifreq.ifr_name, sizeof(ifreq.ifr_name), "eth1");
		if (setsockopt(sock_ip, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifreq, sizeof(ifreq)) < 0) 
		{
			syslog(LOG_ERR, "ioctl SO_BINDTODEVICE failed: %x\n", errno);
			return (-1);
		}
	}

	bzero((char *)&in_serv_addr, sizeof(in_serv_addr));
	in_serv_addr.sin_family = AF_INET;
	in_serv_addr.sin_addr.s_addr = INADDR_ANY;
	in_serv_addr.sin_port = port_ip;
	if (bind(sock_ip, (struct sockaddr *)&in_serv_addr, sizeof(in_serv_addr)) < 0) 
	{
	   syslog(LOG_ERR, "Error %x binding UDP socket\n", errno);
	   return(-1);
	}

	/* Set up poll structure */
	fds[2].fd = sock_ip; 
	fds[2].events = POLLIN;	

	return 1;
}


/*****************************************************************************/
/**
*
* Gets any pending incoming messages.
* 
*
* @param [in]	nohw     soft mode, do not check UNIX socket
* @param [out]	command  pointer to string to copy incoming command into 
*
* @return
*		- length of command string on success
*		- -1 on error
*
******************************************************************************/
int get_message(int nohw, char *command)
{
	unsigned int clilen, in_len = 0;
	int ret;

	if ((ret = poll(fds, NUM_LISTEN_SOCKETS, -1)) < 0) 
	{
		syslog(LOG_ERR, "Error polling for connections\n");
		return -1;
	}

	if((!nohw) && (fds[0].revents & POLLIN))
	{
		struct sockaddr_un cli_addr;

		clilen = sizeof(cli_addr);
		newsockfd = accept(fds[0].fd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		{
			syslog(LOG_ERR, "Error accepting connection\n");
			return -1;
		}
		in_len = read(newsockfd, command, MAX_RESPONSE_LENGTH);

		if(in_len > 0)
		{
			command[in_len] = 0;
		}
	}
	else if(fds[1].revents & POLLIN)
	{
		struct sockaddr_in cli_addr;

		clilen = sizeof(cli_addr);
		newsockfd = accept(fds[1].fd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		{
			syslog(LOG_ERR, "Error accepting connection\n");
			return -1;
		}
		in_len = read(newsockfd, command, MAX_RESPONSE_LENGTH);

		if(in_len > 0)
		{
			command[in_len] = 0;
		}
	}
	else if(fds[2].revents & (POLLIN|POLLERR))
	{
		/* in_len will get set to 0 if handled internally, or -1 on error */
		in_len = proto_ecpri_handle_incoming_msg(fds[2].fd, fds[2].revents, command);
	}
	
	return(in_len);
}


/*****************************************************************************/
/**
*
* Sends response message.
* 
*
* @param [in]	response     pointer to the response string to send.
*
******************************************************************************/
void send_response(char *response)
{	
	write(newsockfd, response, strlen(response));
	close(newsockfd);
}

/*****************************************************************************/
/**
*
* Closes the open sockets.
* 
*
* @param [in]	nohw     soft mode, do not close UNIX or UDP/IP sockets.
*
******************************************************************************/
void close_connections(int nohw)
{
	close(sock_tcp);
	if(!nohw)
	{
		close(sock_fd);
		unlink(XROE_SOCKET_FILE); /* Deletes the file */
		close(sock_ip);
	}
}
/** @} */