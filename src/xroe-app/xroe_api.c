// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file xroe_api.c
* @addtogroup framer_driver_api
* @{
*
*  Radio over Ethernet Framer driver API library 
*
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <roe_framer_ctrl.h>

/* Maximum allowed length of sysfs path */
#define XROE_MAX_SYSPATH_LENGTH 1024

/* IOCTL commands */
/* Use 0xF5 as magic number */
#define XROE_FRAMER_MAGIC_NUMBER  0xF5

#define XROE_FRAMER_IOSET		_IOW(XROE_FRAMER_MAGIC_NUMBER,  0, uint32_t)
#define XROE_FRAMER_IOGET 	_IOR(XROE_FRAMER_MAGIC_NUMBER,  1, uint32_t)

struct ioctl_arguments {
         uint32_t *offset;
         uint32_t *value;
 };

/*****************************************************************************/
/**
*
* Reads bytes from anywhere in the framer address space.
* Reads bytes from the address given and writes them into pRead.
*
* @param [in]	addr   Address in framer address space (0 base) to read from
* @param [out]	pRead  Pointer to use to store output
* @param [in]	length Number of bytes to read
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int IP_API_Read(int addr, uint8_t *pRead, int length)
{
	int fd=0;
	int read = 0;
	int ret = 0;
	fd=open("/dev/xroe/ip", O_RDONLY);

	if(fd>0)
	{
		read = pread(fd, pRead, length, addr);
		if(!read)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}
	
	return ret;
}

/*****************************************************************************/
/**
*
* Writes bytes to anywhere in the framer address space.
* Writes length bytes to addr from pWrite.
*
* @param [in]	addr   Address in framer address space (0 base) to write to
* @param [in]	pWrite Pointer to read from
* @param [in]	length Number of bytes to write
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int IP_API_Write(int addr, uint8_t *pWrite, int length)
{
	int fd=0;
	int write = 0;
	int ret = 0;
	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		write = pwrite(fd, pWrite, length, addr);
		if(!write)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Reads 32-bits from anywhere in the framer address space.
* Reads 32-bits from the address given, shifts and masks them
* as per Offset and Mask, and writes them into pRead.
*
* @param [in]  	addr   Address in framer address space (0 base) to read from
* @param [out] 	pRead  Pointer to use to store output
* @param [in]  	Mask   Mask to use to mask output before shifting
* @param [in]  	Offset Number of bits to shift output down by after masking
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int IP_API_Read_Register(int addr, unsigned int *pRead, int Mask, int Offset)
{
	int fd=0;
	unsigned int buf;
	int ret = 0;
	struct ioctl_arguments args;

	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		args.offset = (uint32_t *)&addr;
		args.value = (uint32_t *)&buf;
		
		ret = ioctl(fd, XROE_FRAMER_IOGET, &args);
		if(!ret)
		{
			*pRead = (buf & Mask) >> Offset;
		}

		close(fd);
	}
	return ret;
}


/*****************************************************************************/
/**
*
* Writes 32-bits to anywhere in the framer address space.
* Writes 32-bits from Write to the address given, after
* shifting and masking them as per Offset and Mask.
*
* @param [in]  	addr   Address in framer address space (0 base) to write to
* @param [in] 	Write  Value to write
* @param [in]  	Mask   Mask to use to mask input after shifting
* @param [in]  	Offset Number of bits to shift input up by before masking
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int IP_API_Write_Register(int addr, unsigned int Write, int Mask, int Offset)
{
	int fd = 0;
	int buf;
	int ret = 0;
	struct ioctl_arguments args;

	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		args.offset = (uint32_t *)&addr;
		args.value = (uint32_t *)&buf;

		buf = (Write << Offset) & Mask;
		ret = ioctl(fd, XROE_FRAMER_IOSET, &args);

		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
* Reads a value from the stats sysfs entries.
* Reads 32-bits from sysfs to resp.
*
* @param [in]  name   Path in TrafGen sysfs to read from
* @param [in]  resp   Pointer to use to store output
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- read() return value on read() failure
*
******************************************************************************/
int STATS_SYSFS_API_Read(const char *name, char *resp)
{
	int fd;
    int w;
	char buff[1024];
	char syspath[XROE_MAX_SYSPATH_LENGTH] = "/sys/kernel/xroe/stats/";
	int ret = -1;
	
	strncat(syspath, name, XROE_MAX_SYSPATH_LENGTH-strlen(syspath));
	fd = open(syspath, O_RDONLY);

	if (fd>0)
	{
		w = read(fd, buff, sizeof(buff));
		if (w)
		{
			ret = 0;
			strncpy(resp, buff, w);
		}
		else
		{
			ret = errno;
		}

		close(fd);
	}

	return ret;
}


/*****************************************************************************/
/**
*
* Enables or disables the framer.
* Writes true or false to framer restart sysfs variable to enable.
* or disable the framer
*
* @param [in]	restart Non-zero to disable (assert restart), zero to enable
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int FRAMER_API_Framer_Restart(int restart)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/framer_restart", O_WRONLY);

	if(fd>0)
	{
		if(restart)
		{
			w = write (fd, "true", 1);
		}
		else
		{
			w = write (fd, "false", 1);
		}
		
		if(w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Enables or disables the deframer.
* Writes true or false to deframer restart sysfs variable to 
* enable or disable the deframer.
*
* @param [in]	restart Non-zero to disable (assert restart), zero to enable
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int FRAMER_API_Deframer_Restart(int restart)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/deframer_restart", O_WRONLY);

	if(fd>0)
	{
		if(restart)
		{
			w = write (fd, "true", 1);
		}
		else
		{
			w = write (fd, "false", 1);
		}
		
		if(w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
* Reads a value from the Traffic Generator sysfs entries.
* Reads 32-bits from sysfs to resp.
*
* @param [in]  name   Path in TrafGen sysfs to read from
* @param [in]  resp   Pointer to use to store output
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- read() return value on read() failure
*
******************************************************************************/
int TRAFGEN_SYSFS_API_Read(const char *name, char *resp)
{
	int fd;
    int w;
	char buff[1024];
	// char syspath[XROE_MAX_SYSPATH_LENGTH] = "/sys/devices/platform/amba_pl@0/a0060000.roe_radio_ctrl/";

	char syspath[XROE_MAX_SYSPATH_LENGTH] = "/sys/kernel/traffic/";
	int ret = -1;
	
	strncat(syspath, name, XROE_MAX_SYSPATH_LENGTH - strlen(syspath));

	fd = open(syspath, O_RDONLY);

	if (fd>0)
	{
		w = read(fd, buff, sizeof(buff));
		if (w)
		{
			ret = 0;
			strncpy(resp, buff, w);
		}
		else
		{
			ret = errno;
		}

		close(fd);
	}

	return ret;
}


/*****************************************************************************/
/**
* Writes a value to the Traffic Generator sysfs entries.
* Writes to sysfs from val.
*
* @param [in]  name   Path in TrafGen sysfs to write to
* @param [in]  val    Pointer to read from
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- write() return value on write() failure
*
******************************************************************************/
int TRAFGEN_SYSFS_API_Write(const char *name, char *val)
{
	int fd;
    int w;
	char syspath[XROE_MAX_SYSPATH_LENGTH] = "/sys/kernel/traffic/";
	int ret = -1;
	
	strncat(syspath, name, XROE_MAX_SYSPATH_LENGTH-strlen(syspath));
	fd = open(syspath, O_RDONLY);

	if (fd>0)
	{
		w = write(fd, val, sizeof(val));
		if (w)
		{
			ret = 0;
		}
		else
		{
			ret = errno;
		}

		close(fd);
	}

	return ret;
}


/*****************************************************************************/
/**
*
* Resets the XXV Ethernet interface.
* Asserts then de-asserts the XXV Ethernet reset.
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int XXV_API_Reset(void)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/xxv_reset", O_WRONLY);

	if (fd>0)
	{
		w = write(fd, "true", 1);
		if (w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}

		w = write(fd, "false", 1);
		if (w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}
/** @} */
