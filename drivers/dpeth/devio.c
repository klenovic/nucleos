/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
**  File:	devio.c		Jun. 11, 2005
**
**  Author:	Giovanni Falzoni <gfalzoni@inwind.it>
**
**  This file contains the routines for readind/writing
**  from/to the device registers.
**
**  $Id: devio.c 2361 2006-07-10 12:43:38Z philip $
*/

#include <nucleos/drivers.h>
#include <net/ether.h>
#include <net/eth_io.h>
#include "dp.h"

#if (USE_IOPL == 0)

static void warning(const char *type, int err)
{

  printf("Warning: eth#0 sys_%s failed (%d)\n", type, err);
  return;
}

/*
**  Name:	unsigned int inb(unsigned short int port);
**  Function:	Reads a byte from specified i/o port.
*/
unsigned int inb(unsigned short port)
{
  unsigned long value;
  int rc;

  if ((rc = sys_inb(port, &value)) != 0) warning("inb", rc);
  return value;
}

/*
**  Name:	unsigned int inw(unsigned short int port);
**  Function:	Reads a word from specified i/o port.
*/
unsigned int inw(unsigned short port)
{
  unsigned long value;
  int rc;

  if ((rc = sys_inw(port, &value)) != 0) warning("inw", rc);
  return value;
}

/*
**  Name:	unsigned int insb(unsigned short int port, int proc_nr, void *buffer, int count);
**  Function:	Reads a sequence of bytes from specified i/o port to user space buffer.
*/
void insb(unsigned short int port, int proc_nr, void *buffer, int count)
{
  int rc;

  if ((rc = sys_insb(port, proc_nr, buffer, count)) != 0)
	warning("insb", rc);
  return;
}

/*
**  Name:	unsigned int insw(unsigned short int port, int proc_nr, void *buffer, int count);
**  Function:	Reads a sequence of words from specified i/o port to user space buffer.
*/
void insw(unsigned short int port, int proc_nr, void *buffer, int count)
{
  int rc;

  if ((rc = sys_insw(port, proc_nr, buffer, count)) != 0)
	warning("insw", rc);
  return;
}

/*
**  Name:	void outb(unsigned short int port, unsigned long value);
**  Function:	Writes a byte to specified i/o port.
*/
void outb(unsigned short port, unsigned long value)
{
  int rc;

  if ((rc = sys_outb(port, value)) != 0) warning("outb", rc);
  return;
}

/*
**  Name:	void outw(unsigned short int port, unsigned long value);
**  Function:	Writes a word to specified i/o port.
*/
void outw(unsigned short port, unsigned long value)
{
  int rc;

  if ((rc = sys_outw(port, value)) != 0) warning("outw", rc);
  return;
}

/*
**  Name:	void outsb(unsigned short int port, int proc_nr, void *buffer, int count);
**  Function:	Writes a sequence of bytes from user space to specified i/o port.
*/
void outsb(unsigned short port, int proc_nr, void *buffer, int count)
{
  int rc;

  if ((rc = sys_outsb(port, proc_nr, buffer, count)) != 0)
	warning("outsb", rc);
  return;
}

/*
**  Name:	void outsw(unsigned short int port, int proc_nr, void *buffer, int count);
**  Function:	Writes a sequence of bytes from user space to specified i/o port.
*/
void outsw(unsigned short port, int proc_nr, void *buffer, int count)
{
  int rc;

  if ((rc = sys_outsw(port, proc_nr, buffer, count)) != 0)
	warning("outsw", rc);
  return;
}

#else
#error To be implemented
#endif				/* USE_IOPL */
/**  devio.c  **/
