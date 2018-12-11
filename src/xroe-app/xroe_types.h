// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 
 
typedef int (*CommandFunc)(int argc, char **argv, char *resp);

typedef struct _commands_t
{
	const char *cmd;
	const char *helptxt;
	CommandFunc func;
} commands_t;
