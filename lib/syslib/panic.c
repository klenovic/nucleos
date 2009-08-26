/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <nucleos/sysutil.h>

#include "syslib.h"

int panicing= 0;

/*===========================================================================*
 *				panic					     *
 *===========================================================================*/
void panic(who, mess, num)
char *who;			/* server identification */
char *mess;			/* message format string */
int num;			/* number to go with format string */
{
/* Something awful has happened. Panics are caused when an internal
 * inconsistency is detected, e.g., a programming error or illegal 
 * value of a defined constant.
 */
  message m;
  endpoint_t me = NONE;
  char name[20];
  void (*suicide)(void);
  if(panicing) return;
  panicing= 1;

  if(sys_whoami(&me, name, sizeof(name)) == 0 && me != NONE)
	printf("%s(%d): ", name, me);
  else
	printf("(sys_whoami failed): ");
  printf("syslib:panic.c: stacktrace: ");
  util_stacktrace();

  if (NULL != who && NULL != mess) {
      if (num != NO_NUM) {
          printf("Panic in %s: %s: %d\n", who, mess, num); 
      } else {
          printf("Panic in %s: %s\n", who, mess); 
      }
  }

  /* Try exit */
  _exit(1);

  /* Try to signal ourself */
  abort();

  /* If exiting nicely through PM fails for some reason, try to
   * commit suicide. E.g., message to PM might fail due to deadlock.
   */
  suicide = (void (*)(void)) -1;
  suicide();

  /* If committing suicide fails for some reason, hang. */
  for(;;) { }
}

