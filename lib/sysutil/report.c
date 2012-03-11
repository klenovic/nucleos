/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h" 

/*===========================================================================*
 *				    report					     *
 *===========================================================================*/
void report(who, mess, num)
char *who;				/* server identification */
char *mess;				/* message format to print */
int num;				/* number to go with the message */
{
/* Display a message for a server. */ 

  if (num != NO_NUM) {
      printk("%s: %s %d\n", who, mess, num);
  } else {
      printk("%s: %s\n", who, mess);
  }
}


