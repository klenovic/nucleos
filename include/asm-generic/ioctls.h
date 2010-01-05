/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioctl.h - All ioctl() command codes.	Author: Kees J. Bot
 *								23 Nov 2002
 *
 * This header file includes all other ioctl command code headers.
 */

#ifndef __ASM_GENERIC_IOCTLS_H
#define __ASM_GENERIC_IOCTLS_H

/* A driver that uses ioctls claims a character for its series of commands.
 * For instance:  #define TCGETS  _IOR('T',  8, struct termios)
 * This is a terminal ioctl that uses the character 'T'.  The character(s)
 * used in each header file are shown in the comment following.
 */
#include <asm-generic/ioctls-tty.h>	/* 'T' 't' 'k'		*/
#include <asm-generic/ioctls-net.h>	/* 'n'			*/
#include <asm-generic/ioctls-disk.h>	/* 'd'			*/
#include <asm-generic/ioctls-file.h>	/* 'f'			*/
#include <asm-generic/ioctls-memory.h>	/* 'm'			*/
#include <asm-generic/ioctls-cmos.h>	/* 'c'			*/
#include <asm-generic/ioctls-tape.h>	/* 'M'			*/
#include <asm-generic/ioctls-scsi.h>	/* 'S'			*/
#include <asm-generic/ioctls-sound.h>	/* 's'			*/
#include <asm-generic/ioctls-svrctl.h>	/* 's'			*/

#endif /* __ASM_GENERIC_IOCTLS_H */
