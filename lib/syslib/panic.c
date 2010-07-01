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
#include <nucleos/signal.h>
#include <nucleos/unistd.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <kernel/proc.h>

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
  kipc_msg_t m;
  struct proc proc;
  endpoint_t me;
  char *name=0;
  char *name_unknown = "unknown";
  void (*suicide)(void);
  if(panicing) return;
  panicing= 1;

  if(sys_getproc(&proc, ENDPT_SELF) != 0) {
	name = name_unknown;
	me = ENDPT_NONE;
  } else {
	name = proc.p_name;
	me = proc.p_endpoint;
  }

  printf("%s(%d): ", name, me);

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

