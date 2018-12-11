// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file xroe-app.c
* Main file for sample application.
*
*
******************************************************************************/

/** 
* @addtogroup sample_app
* @{
*
*  A sample application to demonstrate the RoE Framer software modules.
*
* @details
* 
* The sample application demonstrates how the software stack operates. It is
* not a complete implementation, and is not intended to be used in a production
* system. The application runs in the foreground or as a daemon, providing 
* interfaces for serial, socket and filesystem FIFO communications. The sample 
* application is composed of the following modules and libraries.
* 
* - @link framer_driver_api C Library API @endlink
*
* This library is ready for reuse in an unmodified form.
*
* - @link command_parser Message Parsing Module @endlink
*
* This module demonstrates how a human-readable message protocol can be used to
* configure and control the RoE Framer. It provides interfaces to the hardware
* API library and any protocol modules.
*
* - @link comms_lib Communications Module @endlink
*
* This module provides a VFS interface for command line operation and TCP/IP
* and UDP/IP socket interfaces for remote operation.
*
* - @link protocol_ecpri eCPRI Protocol Module @endlink
*
* This module provides a sample implementation of the eCPRI user plane over IP
* message layer, including one-way delay measurements and remote memory access
* to a remote node.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <errno.h>
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
#include <arpa/inet.h>
#include <syslog.h>

#include <client.h>
#include <comms.h>
#include <xroefram_str.h>
#include <parser.h>


/**
 * PID_FILE The full name of the file containing the application process id.
 */
#define PID_FILE "/var/run/xroe.pid"

char pid_path[30];

/*****************************************************************************/
/**
*
* Daemonises the application if requested at startup.
*
* @param [in]	port   Port to open (if not default)
******************************************************************************/
static void skeleton_daemon(int port)
{
    pid_t pid, sid;
	int fd;
	char pid_str[20];
	
    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}
	
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("xroe-appd", LOG_PID, LOG_DAEMON);
	
	pid = getpid();
	if(port)
	{
		sprintf(pid_path, "/var/run/xroe%d.pid", port);
	}
	else
	{
		sprintf(pid_path, PID_FILE);
	}
	/* Try and create the pidfile */
	if((fd = open(pid_path, O_CREAT | O_EXCL | O_CLOEXEC | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH)) < 0)
	{
		syslog(LOG_ERR, "Failed to create pidfile @ /var/run/xroe.pid, is there another daemon running?\n");
		exit(EXIT_FAILURE);
	}

	/* Write the pid to the pidfile and close it */
	sprintf(pid_str, "%d", pid);
	write(fd, pid_str, strlen(pid_str));
	close(fd);
}

/*****************************************************************************/
/**
*
* Entry point into sample application
* Parses optional arguments, opens connections and log, contains the message
* handling loop. 
* argc less than 2 (i.e. just calling the program with no command-line 
* arguments), or the first command line argument being "help" will return the
* help text.
* Other valid options are:
* - d: daemonise the application
* - s: soft mode, do not open UNIX socket or UDP socket for eCPRI messages
* - n: connect to remote application at given IP address (requires -c)
* - p: connect to given remote port (-c) or listen on the given port
* - c: send command to listening application (UNIX socket by default)
*
* @param [in]	argc   Number of command-line arguments (including program name)
* @param [in]	argv   Array of strings containg command-line arguments
*
* @return
*		- EXIT_SUCCESS on clean exit
*		- EXIT_FAILURE on start-up error
*
******************************************************************************/
int main(int argc, char **argv)
{
	char response[MAX_RESPONSE_LENGTH];
	char command[MAX_RESPONSE_LENGTH];
	int quit = 0;
	int nohw = 0;
	int daemonise = 0;
	int send_command = 0;
	in_addr_t in_addr = 0;
	int port = 0;
	int opt;
	int msg_to_parse = 0;
	
	if((argc < 2) || (strcmp(argv[1], "help")==0))
	{
		printf(XROE_USAGE_STR);
		exit(EXIT_FAILURE);
	}
	
    while ((opt = getopt(argc, argv, "dsn:p:c:")) != -1) 
	{
        switch (opt) 
		{
        case 'd':
            daemonise = 1;
            break;
        case 's':
            nohw = 1;
            break;
        case 'n':
            in_addr = inet_addr(optarg);
            break;
        case 'p':
			port = atoi(optarg);
			break;
        case 'c':
            send_command = 1;
			bzero(command, sizeof(command));
			strncpy(command, optarg, MAX_RESPONSE_LENGTH-1);
            break;
        default: /* '?' */
			printf(XROE_USAGE_STR);
            exit(EXIT_FAILURE);
        }
    }
		 
	/* Command line client used to talk to daemon */
	if(send_command)
	{
		client_send_message(in_addr, port, command);
		exit(0);
	}

	if(daemonise)
	{
		struct stat stat_struct;
		if(-1 != stat(PID_FILE, &stat_struct))
		{
			printf("Pidfile exists, is there another daemon running?\n");
		}
		else if(errno != ENOENT)
		{
			perror("Error checking Pidfile");
		}

		skeleton_daemon(port);
		syslog(LOG_NOTICE, "xroe-app daemon started.");
	}
	else if(nohw)
	{
		openlog ("xroe-appd", LOG_PID, LOG_USER);
	}
 
	if(open_connections(nohw, port)<0)
	{
		syslog(LOG_ERR, "Exiting on connection error\n");
		exit(EXIT_FAILURE);
	}
	
	while(!quit)
	{
		bzero(command, sizeof(command));
		bzero(response, sizeof(response));
		
		msg_to_parse = get_message(nohw, command);
		if(msg_to_parse == 0)
		{
			/* Message already dealt with internally (eCPRI) */
			continue;
		}
		else if(msg_to_parse < 0)
		{
			/* Deal with error? */
			quit = 1;
		}
		else if(parse_command(nohw, command, response) < 0)
		{
			quit = 1;
		}
		else
		{
			send_response(response);
		}
	}
 
	close_connections(nohw);
    syslog(LOG_NOTICE, "xroe-app terminated.");
    closelog();
	unlink(pid_path);
    return EXIT_SUCCESS;
}
/** @} */
