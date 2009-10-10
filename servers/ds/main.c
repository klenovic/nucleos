/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Data Store Server. 
 * This service implements a little publish/subscribe data store that is 
 * crucial for the system's fault tolerance. Components that require state
 * can store it here, for later retrieval, e.g., after a crash and subsequent
 * restart by the reincarnation server. 
 * 
 * Created:
 *   Oct 19, 2005       by Jorrit N. Herder
 */
#include "inc.h"        /* include master header file */
#include <nucleos/kernel.h>
#include <nucleos/endpoint.h>

/* Allocate space for the global variables. */
endpoint_t who_e;       /* caller's proc number */
int callnr;             /* system call number */
int sys_panic;          /* flag to indicate system-wide panic */

extern int errno;       /* error number set by system library */

/* Declare some local functions. */
static void init_server(int argc, char **argv);
static void exit_server(void);
static void sig_handler(void);
static void get_work(message *m_ptr);
static void reply(int whom, message *m_ptr);

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char **argv)
{
/* This is the main routine of this service. The main loop consists of 
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  message m;
  int result;
  sigset_t sigset;

  /* Initialize the server, then go to work. */
  init_server(argc, argv);

  /* Main loop - get work and do it, forever. */
  while (TRUE) {

      /* Wait for incoming message, sets 'callnr' and 'who'. */
      get_work(&m);

      if (is_notify(callnr)) {
	      switch (_ENDPOINT_P(who_e)) {
		      case PM_PROC_NR:
			      sig_handler();
			      break;
		      default:
			      report("DS","warning, got illegal notify from:",
					     			 m.m_source);
			      result = -EINVAL;
			      goto send_reply;
	      }

	      /* done, get a new message */
      }

      switch (callnr) {
      case DS_PUBLISH:
          result = do_publish(&m);
          break;
      case DS_RETRIEVE:
          result = do_retrieve(&m);
          break;
      case DS_SUBSCRIBE:
          result = do_subscribe(&m);
          break;
      case DS_CHECK:
	  result = do_check(&m);
	  break;
      case __NR_getsysinfo:
          result = do_getsysinfo(&m);
          break;
      default: 
          report("DS","warning, got illegal request from:", m.m_source);
          result = -EINVAL;
      }

send_reply:
      /* Finally send reply message, unless disabled. */
      if (result != -EDONTREPLY) {
          m.m_type = result;            /* build reply message */
          reply(who_e, &m);             /* send it away */
      }
  }
  return 0;                           /* shouldn't come here */
}

/*===========================================================================*
 *				 init_server                                 *
 *===========================================================================*/
static void init_server(int argc, char **argv)
{
/* Initialize the data store server. */
  int i, s;
  struct sigaction sigact;

  /* Initialize DS. */
  ds_init();
}

/*===========================================================================*
 *				 sig_handler                                 *
 *===========================================================================*/
static void sig_handler()
{
/* Signal handler. */
  sigset_t sigset;
  int sig;

  /* Try to obtain signal set from PM. */
  if (getsigset(&sigset) != 0) return;

  /* Check for known signals. */
  if (sigismember(&sigset, SIGTERM)) {
      exit_server();
  }
}

/*===========================================================================*
 *				exit_server                                  *
 *===========================================================================*/
static void exit_server()
{
/* Shut down the information service. */

  /* Done. Now exit. */
  exit(0);
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
static void get_work(m_ptr)
message *m_ptr;				/* message buffer */
{
    int status = 0;
    status = kipc_receive(ANY, m_ptr);   /* this blocks until message arrives */

    if (status != 0)
        panic("DS","failed to receive message!", status);

    who_e = m_ptr->m_source;        /* message arrived! set sender */
    callnr = m_ptr->m_type;       /* set function call number */
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(who_e, m_ptr)
int who_e;                           	/* destination */
message *m_ptr;				/* message buffer */
{
    int s;
    s = kipc_send(who_e, m_ptr);    /* send the message */

    if (s != 0)
        panic("DS", "unable to send reply!", s);
}

