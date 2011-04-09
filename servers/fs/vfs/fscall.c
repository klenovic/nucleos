/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file handles nested counter-request calls to VFS sent by file system
 * (VFS_PROC_NR) servers in response to VFS requests.
 *
 * The entry points into this file are
 *   nested_fs_call	perform a nested call from a file system server
 */

#include "fs.h"
#include <servers/vfs/fproc.h>
#include <nucleos/string.h>
#include <assert.h>
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>

/* maximum nested call stack depth */
#define MAX_DEPTH 1

/* global variables stack */
static struct {
  struct fproc *g_fp;			/* pointer to caller process */
  kipc_msg_t g_m_in;			/* request message */
  kipc_msg_t g_m_out;			/* reply message */
  int g_who_e;				/* endpoint of caller process */
  int g_who_p;				/* slot number of caller process */
  int g_call_nr;			/* call number */
  int g_super_user;			/* is the caller root? */
  short g_cum_path_processed;		/* how many path chars processed? */
  char g_user_fullpath[PATH_MAX+1];	/* path to look up */
} globals[MAX_DEPTH];

static int depth = 0;			/* current globals stack level */

#ifdef CONFIG_DEBUG_SERVERS_SYSCALL_STATS
extern unsigned long calls_stats[NR_syscalls];
#endif

static int push_globals(void);
static void pop_globals(void);
static void set_globals(kipc_msg_t *m);

/*===========================================================================*
 *				push_globals				     *
 *===========================================================================*/
static int push_globals()
{
/* Save the global variables of the current call onto the globals stack.
 */

  if (depth == MAX_DEPTH)
	return -EPERM;

  globals[depth].g_fp = fp;
  globals[depth].g_m_in = m_in;
  globals[depth].g_m_out = m_out;
  globals[depth].g_who_e = who_e;
  globals[depth].g_who_p = who_p;
  globals[depth].g_call_nr = call_nr;
  globals[depth].g_super_user = super_user;
  globals[depth].g_cum_path_processed = cum_path_processed;

  /* XXX is it safe to strcpy this? */
  assert(sizeof(globals[0].g_user_fullpath) == sizeof(user_fullpath));
  memcpy(globals[depth].g_user_fullpath, user_fullpath, sizeof(user_fullpath));

  /* err_code is not used across blocking calls */

  depth++;

  return 0;
}

/*===========================================================================*
 *				pop_globals				     *
 *===========================================================================*/
static void pop_globals()
{
/* Restore the global variables of a call from the globals stack.
 */

  if (depth == 0)
	panic("VFS", "Popping from empty globals stack!", NO_NUM);

  depth--;

  fp = globals[depth].g_fp;
  m_in = globals[depth].g_m_in;
  m_out = globals[depth].g_m_out;
  who_e = globals[depth].g_who_e;
  who_p = globals[depth].g_who_p;
  call_nr = globals[depth].g_call_nr;
  super_user = globals[depth].g_super_user;
  cum_path_processed = globals[depth].g_cum_path_processed;

  memcpy(user_fullpath, globals[depth].g_user_fullpath, sizeof(user_fullpath));
}

/*===========================================================================*
 *				set_globals				     *
 *===========================================================================*/
static void set_globals(m)
kipc_msg_t *m;				/* request message */
{
/* Initialize global variables based on a request message.
 */

  m_in = *m;
  who_e = m_in.m_source;
  who_p = _ENDPOINT_P(who_e);
  call_nr = m_in.m_type;
  fp = &fproc[who_p];
  super_user = (fp->fp_effuid == SU_UID ? TRUE : FALSE);
  /* the rest need not be initialized */
}

/*===========================================================================*
 *				nested_fs_call				     *
 *===========================================================================*/
void nested_fs_call(m)
kipc_msg_t *m;				/* request/reply message pointer */
{
/* Handle a nested call from a file system server.
 */
  int r;

  /* Save global variables of the current call */
  if ((r = push_globals()) != 0) {
	printk("VFS: error saving global variables in call %d from VFS_PROC_NR %d\n",
		m->m_type, m->m_source);
  } else {
	/* Initialize global variables for the nested call */
	set_globals(m);

	/* Perform the nested call */
	if (call_nr < 0 || call_nr >= NR_syscalls) {
		printk("VFS: invalid nested call %d from VFS_PROC_NR %d\n", call_nr,
			who_e);

		r = -ENOSYS;
	} else {
#ifdef CONFIG_DEBUG_SERVERS_SYSCALL_STATS
		calls_stats[call_nr]++;
#endif

		r = (*call_vec[call_nr])();
	}

	/* Store the result, and restore original global variables */
	*m = m_out;

	pop_globals();
  }

  m->m_type = r;
}
