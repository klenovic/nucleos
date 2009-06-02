/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __KERNEL_IPC_H
#define __KERNEL_IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#include <nucleos/com.h>

/* Masks and flags for system calls. */
#define NON_BLOCKING    0x0080  /* do not block if target not ready */

/* System call numbers that are passed when trapping to the kernel. */
#define SEND		   1	/* blocking send */
#define RECEIVE		   2	/* blocking receive */
#define SENDREC	 	   3  	/* SEND + RECEIVE */
#define NOTIFY		   4	/* asynchronous notify */
#define SENDNB             5    /* nonblocking send */
#define SENDA		   16	/* asynchronous send */

/* The following bit masks determine what checks that should be done. */
#define CHECK_DEADLOCK  0x03	/* 0000 0011 : check for deadlock */

#define WILLRECEIVE(target, source_ep) \
  ((RTS_ISSET(target, RECEIVING) && !RTS_ISSET(target, SENDING)) &&	\
    (target->p_getfrom_e == ANY || target->p_getfrom_e == source_ep))

#endif /* __KERNEL_IPC_H */
