/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* System Information Service. 
 * This service handles the various debugging dumps, such as the process
 * table, so that these no longer directly touch kernel memory. Instead, the 
 * system task is asked to copy some table in local memory. 
 * 
 * Created:
 *   Apr 29, 2004	by Jorrit N. Herder
 */

#include "inc.h"

/* Allocate space for the global variables. */
message m_in;		/* the input message itself */
message m_out;		/* the output message used for reply */
int who_e;		/* caller's proc number */
int callnr;		/* system call number */

extern int errno;	/* error number set by system library */

/* Declare some local functions. */
static void init_server(int argc, char **argv);
static void get_work(void);
static void reply(int whom, int result);

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char **argv)
{
/* This is the main routine of this service. The main loop consists of 
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  int result;                 
  sigset_t sigset;

  /* Initialize the server, then go to work. */
  init_server(argc, argv);

  /* Main loop - get work and do it, forever. */         
  while (TRUE) {              

      /* Wait for incoming message, sets 'callnr' and 'who'. */
      get_work();

      switch (callnr) {
      case SYS_SIG:
	  printf("got SYS_SIG message\n");
	  sigset = m_in.NOTIFY_ARG;
	  for ( result=0; result< _NSIG; result++) {
	      if (sigismember(&sigset, result))
		  printf("signal %d found\n", result);
	  }
	  continue;
      case PROC_EVENT:
          result = -EDONTREPLY;
      	  break;
      case FKEY_PRESSED:
          result = do_fkey_pressed(&m_in);
          break;
      case DEV_PING:
	  notify(m_in.m_source);
	  continue;
      default: 
          printf("IS: warning, got illegal request %d from %d\n",
          	callnr, m_in.m_source);
          result = -EDONTREPLY;
      }

      /* Finally send reply message, unless disabled. */
      if (result != -EDONTREPLY) {
	  reply(who_e, result);
      }
  }
  return 0;				/* shouldn't come here */
}

/*===========================================================================*
 *				 init_server                                 *
 *===========================================================================*/
static void init_server(int argc, char **argv)
{
/* Initialize the information service. */
  int fkeys, sfkeys;
  int i, s;
  struct sigaction sigact;

  /* Install signal handler. Ask PM to transform signal into message. */
  sigact.sa_handler = SIG_MESS;
  sigact.sa_mask = ~0;			/* block all other signals */
  sigact.sa_flags = 0;			/* default behaviour */
  if (sigaction(SIGTERM, &sigact, NULL) < 0) 
      report("IS","warning, sigaction() failed", errno);

  /* Set key mappings. IS takes all of F1-F12 and Shift+F1-F10. */
  fkeys = sfkeys = 0;
  for (i=1; i<=12; i++) bit_set(fkeys, i);
  for (i=1; i<=10; i++) bit_set(sfkeys, i);
  if ((s=fkey_map(&fkeys, &sfkeys)) != 0)
      report("IS", "warning, fkey_map failed:", s);
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
static void get_work()
{
    int status = 0;
    status = receive(ANY, &m_in);   /* this blocks until message arrives */
    if (status != 0)
        panic("IS","failed to receive message!", status);
    who_e = m_in.m_source;        /* message arrived! set sender */
    callnr = m_in.m_type;       /* set function call number */
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(who, result)
int who;                           	/* destination */
int result;                           	/* report result to replyee */
{
    int send_status;
    m_out.m_type = result;  		/* build reply message */
    send_status = send(who, &m_out);    /* send the message */
    if (send_status != 0)
        panic("IS", "unable to send reply!", send_status);
}

