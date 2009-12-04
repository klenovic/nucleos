/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file handles signals, which are asynchronous events and are generally
 * a messy and unpleasant business.  Signals can be generated by the KILL
 * system call, or from the keyboard (SIGINT) or from the clock (SIGALRM).
 * In all cases control eventually passes to check_sig() to see which processes
 * can be signaled.  The actual signaling is done by sig_proc().
 *
 * The entry points into this file are:
 *   do_sigaction:	perform the SIGACTION system call
 *   do_sigpending:	perform the SIGPENDING system call
 *   do_sigprocmask:	perform the SIGPROCMASK system call
 *   do_sigreturn:	perform the SIGRETURN system call
 *   do_sigsuspend:	perform the SIGSUSPEND system call
 *   do_kill:		perform the KILL system call
 *   do_pause:		perform the PAUSE system call
 *   ksig_pending:	the kernel notified about pending signals
 *   sig_proc:		interrupt or terminate a signaled process
 *   check_sig:		check which processes to signal with sig_proc()
 *   check_pending:	check if a pending signal can now be delivered
 *   restart_sigs: 	restart signal work after finishing a FS call
 */
#include <nucleos/kernel.h>
#include "pm.h"
#include <nucleos/stat.h>
#include <nucleos/ptrace.h>
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>
#include <nucleos/com.h>
#include <nucleos/vm.h>
#include <nucleos/signal.h>
#include <nucleos/resource.h>
#include <asm/sigcontext.h>
#include <nucleos/string.h>
#include "mproc.h"
#include "param.h"

static void unpause(struct mproc *rmp);
static void handle_ksig(int proc_nr, sigset_t sig_map);
static int sig_send(struct mproc *rmp, int signo);

/*===========================================================================*
 *				do_sigaction				     *
 *===========================================================================*/
int do_sigaction()
{
  int r;
  struct sigaction svec;
  struct sigaction *svp;

  if (m_in.sig_nr == SIGKILL) return(0);
  if (m_in.sig_nr < 1 || m_in.sig_nr >= _NSIG) return(-EINVAL);
  svp = &mp->mp_sigact[m_in.sig_nr];
  if ((struct sigaction *) m_in.sig_osa != (struct sigaction *) NULL) {
	r = sys_datacopy(PM_PROC_NR,(vir_bytes) svp,
		who_e, (vir_bytes) m_in.sig_osa, (phys_bytes) sizeof(svec));
	if (r != 0) return(r);
  }

  if ((struct sigaction *) m_in.sig_nsa == (struct sigaction *) NULL) 
  	return 0;

  /* Read in the sigaction structure. */
  r = sys_datacopy(who_e, (vir_bytes) m_in.sig_nsa,
		PM_PROC_NR, (vir_bytes) &svec, (phys_bytes) sizeof(svec));
  if (r != 0) return(r);

  if (svec.sa_handler == SIG_IGN) {
	sigaddset(&mp->mp_ignore, m_in.sig_nr);
	sigdelset(&mp->mp_sigpending, m_in.sig_nr);
	sigdelset(&mp->mp_catch, m_in.sig_nr);
	sigdelset(&mp->mp_sig2mess, m_in.sig_nr);
  } else if (svec.sa_handler == SIG_DFL) {
	sigdelset(&mp->mp_ignore, m_in.sig_nr);
	sigdelset(&mp->mp_catch, m_in.sig_nr);
	sigdelset(&mp->mp_sig2mess, m_in.sig_nr);
  } else if (svec.sa_handler == SIG_MESS) {
	if (! (mp->mp_flags & PRIV_PROC)) return(-EPERM);
	sigdelset(&mp->mp_ignore, m_in.sig_nr);
	sigaddset(&mp->mp_sig2mess, m_in.sig_nr);
	sigdelset(&mp->mp_catch, m_in.sig_nr);
  } else {
	sigdelset(&mp->mp_ignore, m_in.sig_nr);
	sigaddset(&mp->mp_catch, m_in.sig_nr);
	sigdelset(&mp->mp_sig2mess, m_in.sig_nr);
  }

  mp->mp_sigact[m_in.sig_nr].sa_handler = svec.sa_handler;

  sigdelset(&svec.sa_mask, SIGKILL);
  sigdelset(&svec.sa_mask, SIGSTOP);

  mp->mp_sigact[m_in.sig_nr].sa_mask = svec.sa_mask;
  mp->mp_sigact[m_in.sig_nr].sa_flags = svec.sa_flags;
  mp->mp_sigreturn = (vir_bytes) m_in.sig_ret;
  return 0;
}

/*===========================================================================*
 *				do_sigpending                                *
 *===========================================================================*/
int do_sigpending()
{
  mp->mp_reply.reply_mask = (long) mp->mp_sigpending;
  return 0;
}

/*===========================================================================*
 *				do_sigprocmask                               *
 *===========================================================================*/
int do_sigprocmask()
{
/* Note that the library interface passes the actual mask in sigmask_set,
 * not a pointer to the mask, in order to save a copy.  Similarly,
 * the old mask is placed in the return message which the library
 * interface copies (if requested) to the user specified address.
 *
 * The library interface must set SIG_INQUIRE if the 'act' argument
 * is NULL.
 *
 * KILL and STOP can't be masked.
 */

  int i;

  mp->mp_reply.reply_mask = (long) mp->mp_sigmask;

  switch (m_in.sig_how) {
      case SIG_BLOCK:
	sigdelset((sigset_t *)&m_in.sig_set, SIGKILL);
	sigdelset((sigset_t *)&m_in.sig_set, SIGSTOP);
	for (i = 1; i < _NSIG; i++) {
		if (sigismember((sigset_t *)&m_in.sig_set, i))
			sigaddset(&mp->mp_sigmask, i);
	}
	break;

      case SIG_UNBLOCK:
	for (i = 1; i < _NSIG; i++) {
		if (sigismember((sigset_t *)&m_in.sig_set, i))
			sigdelset(&mp->mp_sigmask, i);
	}
	check_pending(mp);
	break;

      case SIG_SETMASK:
	sigdelset((sigset_t *) &m_in.sig_set, SIGKILL);
	sigdelset((sigset_t *) &m_in.sig_set, SIGSTOP);
	mp->mp_sigmask = (sigset_t) m_in.sig_set;
	check_pending(mp);
	break;

      case SIG_INQUIRE:
	break;

      default:
	return(-EINVAL);
	break;
  }
  return 0;
}

/*===========================================================================*
 *				do_sigsuspend                                *
 *===========================================================================*/
int do_sigsuspend()
{
  mp->mp_sigmask2 = mp->mp_sigmask;	/* save the old mask */
  mp->mp_sigmask = (sigset_t) m_in.sig_set;
  sigdelset(&mp->mp_sigmask, SIGKILL);
  sigdelset(&mp->mp_sigmask, SIGSTOP);
  mp->mp_flags |= SIGSUSPENDED;
  check_pending(mp);
  return(SUSPEND);
}

/*===========================================================================*
 *				do_sigreturn				     *
 *===========================================================================*/
int do_sigreturn()
{
/* A user signal handler is done.  Restore context and check for
 * pending unblocked signals.
 */

  int r;

  mp->mp_sigmask = (sigset_t) m_in.sig_set;
  sigdelset(&mp->mp_sigmask, SIGKILL);
  sigdelset(&mp->mp_sigmask, SIGSTOP);

  r = sys_sigreturn(who_e, (struct sigmsg *) m_in.sig_context);
  check_pending(mp);
  return(r);
}

/*===========================================================================*
 *				do_kill					     *
 *===========================================================================*/
int do_kill()
{
/* Perform the kill(pid, signo) system call. */

  return check_sig(m_in.pid, m_in.sig_nr);
}

/*===========================================================================*
 *				ksig_pending				     *
 *===========================================================================*/
int ksig_pending()
{
/* Certain signals, such as segmentation violations originate in the kernel.
 * When the kernel detects such signals, it notifies the PM to take further 
 * action. The PM requests the kernel to send messages with the process
 * slot and bit map for all signaled processes. The File System, for example,
 * uses this mechanism to signal writing on broken pipes (SIGPIPE). 
 *
 * The kernel has notified the PM about pending signals. Request pending
 * signals until all signals are handled. If there are no more signals,
 * NONE is returned in the process number field.
 */ 
 endpoint_t proc_nr_e;
 sigset_t sig_map;

 while (TRUE) {
   int r;
   /* get an arbitrary pending signal */
   if((r=sys_getksig(&proc_nr_e, &sig_map)) != 0)
  	panic(__FILE__,"sys_getksig failed", r);
   if (NONE == proc_nr_e) {		/* stop if no more pending signals */
 	break;
   } else {
 	int proc_nr_p;
 	if(pm_isokendpt(proc_nr_e, &proc_nr_p) != 0)
  		panic(__FILE__,"sys_getksig strange process", proc_nr_e);
   	handle_ksig(proc_nr_e, sig_map);	/* handle the received signal */
	/* If the process still exists to the kernel after the signal
	 * has been handled ...
	 */
        if ((mproc[proc_nr_p].mp_flags & (IN_USE | EXITING)) == IN_USE)
	{
	   if((r=sys_endksig(proc_nr_e)) != 0)	/* ... tell kernel it's done */
  		panic(__FILE__,"sys_endksig failed", r);
	}
   }
 } 
 return(SUSPEND);			/* prevents sending reply */
}

/*===========================================================================*
 *				handle_ksig				     *
 *===========================================================================*/
static void handle_ksig(proc_nr_e, sig_map)
int proc_nr_e;
sigset_t sig_map;
{
  register struct mproc *rmp;
  int i, proc_nr;
  pid_t proc_id, id;

  if(pm_isokendpt(proc_nr_e, &proc_nr) != 0 || proc_nr < 0) {
	printf("PM: handle_ksig: %d?? not ok\n", proc_nr_e);
	return;
  }
  rmp = &mproc[proc_nr];
  if ((rmp->mp_flags & (IN_USE | EXITING)) != IN_USE) {
#if 0
	printf("PM: handle_ksig: %d?? exiting / not in use\n", proc_nr_e);
#endif
	return;
  }
  proc_id = rmp->mp_pid;
  mp = &mproc[0];			/* pretend signals are from PM */
  mp->mp_procgrp = rmp->mp_procgrp;	/* get process group right */

  /* Check each bit in turn to see if a signal is to be sent.  Unlike
   * kill(), the kernel may collect several unrelated signals for a
   * process and pass them to PM in one blow.  Thus loop on the bit
   * map. For SIGVTALRM and SIGPROF, see if we need to restart a
   * virtual timer. For SIGINT, SIGWINCH and SIGQUIT, use proc_id 0
   * to indicate a broadcast to the recipient's process group.  For
   * SIGKILL, use proc_id -1 to indicate a systemwide broadcast.
   */
  for (i = 1; i < _NSIG; i++) {
	if (!sigismember(&sig_map, i)) continue;
#if 0
	printf("PM: sig %d for %d from kernel\n", 
		i, proc_nr_e);
#endif
	switch (i) {
	    case SIGINT:
	    case SIGQUIT:
	    case SIGWINCH:
		id = 0; break;	/* broadcast to process group */
	    case SIGVTALRM:
	    case SIGPROF:
	    	check_vtimer(proc_nr, i);
	    	/* fall-through */
	    default:
		id = proc_id;
		break;
	}
	check_sig(id, i);
  }

  /* If SIGNDELAY is set, an earlier sys_stop() failed because the process was
   * still sending, and the kernel hereby tells us that the process is now done
   * with that. We can now try to resume what we planned to do in the first
   * place: set up a signal handler. However, the process's message may have
   * been a call to PM, in which case the process may have changed any of its
   * signal settings. The process may also have forked, exited etcetera.
   */
  if (sigismember(&sig_map, SIGNDELAY) && (rmp->mp_flags & DELAY_CALL)) {
	rmp->mp_flags &= ~DELAY_CALL;

	if (rmp->mp_flags & (FS_CALL | PM_SIG_PENDING))
		panic(__FILE__, "handle_ksig: bad process state", NO_NUM);

	/* Process as many normal signals as possible. */
	check_pending(rmp);

	if (rmp->mp_flags & DELAY_CALL)
		panic(__FILE__, "handle_ksig: multiple delay calls?", NO_NUM);
  }
}

/*===========================================================================*
 *				do_pause				     *
 *===========================================================================*/
int do_pause()
{
/* Perform the pause() system call. */

  mp->mp_flags |= PAUSED;
  return(SUSPEND);
}

/*===========================================================================*
 *				sig_proc				     *
 *===========================================================================*/
void sig_proc(rmp, signo, trace)
register struct mproc *rmp;	/* pointer to the process to be signaled */
int signo;			/* signal to send to process (1 to _NSIG-1) */
int trace;			/* pass signal to tracer first? */
{
/* Send a signal to a process.  Check to see if the signal is to be caught,
 * ignored, tranformed into a message (for system processes) or blocked.
 *  - If the signal is to be transformed into a message, request the KERNEL to
 * send the target process a system notification with the pending signal as an
 * argument.
 *  - If the signal is to be caught, request the KERNEL to push a sigcontext
 * structure and a sigframe structure onto the catcher's stack.  Also, KERNEL
 * will reset the program counter and stack pointer, so that when the process
 * next runs, it will be executing the signal handler. When the signal handler
 * returns,  sigreturn(2) will be called.  Then KERNEL will restore the signal
 * context from the sigcontext structure.
 * If there is insufficient stack space, kill the process.
 */
  int r, slot;

  slot = (int) (rmp - mproc);
  if ((rmp->mp_flags & (IN_USE | EXITING)) != IN_USE) {
	printf("PM: signal %d sent to exiting process %d\n", signo, slot);
	panic(__FILE__,"", NO_NUM);
  }

  if (trace == TRUE && rmp->mp_tracer != NO_TRACER && signo != SIGKILL) {
	/* Signal should be passed to the debugger first.
	 * This happens before any checks on block/ignore masks; otherwise,
	 * the process itself could block/ignore debugger signals.
	 */

	sigaddset(&rmp->mp_sigtrace, signo);

	if (!(rmp->mp_flags & STOPPED))
		stop_proc(rmp, signo);	/* a signal causes it to stop */

	return;
  }

  if (rmp->mp_flags & FS_CALL) {
	sigaddset(&rmp->mp_sigpending, signo);

	if (!(rmp->mp_flags & PM_SIG_PENDING)) {
		/* No delay calls: FS_CALL implies the process called us. */
		if ((r = sys_stop(rmp->mp_endpoint)) != 0)
			panic(__FILE__, "sys_stop failed", r);

		rmp->mp_flags |= PM_SIG_PENDING;
	}

	return;
  }

  if (sigismember(&rmp->mp_ignore, signo)) { 
	/* Signal should be ignored. */
	return;
  }
  if (sigismember(&rmp->mp_sigmask, signo)) {
	/* Signal should be blocked. */
	sigaddset(&rmp->mp_sigpending, signo);
	return;
  }
  if (sigismember(&rmp->mp_sig2mess, signo)) {
	/* Mark event pending in process slot and send notification. */
	sigaddset(&rmp->mp_sigpending, signo);
	kipc_notify(rmp->mp_endpoint);
	return;
  }

  if ((rmp->mp_flags & STOPPED) && signo != SIGKILL) {
	/* If the process is stopped for a debugger, do not deliver any signals
	 * (except SIGKILL) in order not to confuse the debugger. The signals
	 * will be delivered using the check_pending() calls in do_trace().
	 */
	sigaddset(&rmp->mp_sigpending, signo);
	return;
  }

  if (sigismember(&rmp->mp_catch, signo)) {
	/* Signal is caught. First interrupt the process's current call, if
	 * applicable. This may involve a roundtrip to FS, in which case we'll
	 * have to check back later.
	 */
	if (!(rmp->mp_flags & UNPAUSED)) {
		unpause(rmp);

		if (!(rmp->mp_flags & UNPAUSED)) {
			/* not yet unpaused; continue later */
			sigaddset(&rmp->mp_sigpending, signo);

			return;
		}
	}

	/* Then send the actual signal to the process, by setting up a signal
	 * handler.
	 */
	if (sig_send(rmp, signo))
		return;

	/* We were unable to spawn a signal handler. Kill the process. */
  }
  else if (sigismember(&ign_sset, signo)) {
	/* Signal defaults to being ignored. */
	return;
  }

  /* Terminate process */
  rmp->mp_sigstatus = (char) signo;
  if (sigismember(&core_sset, signo)) {
	printf("PM: coredump signal %d for %d / %s\n", signo, rmp->mp_pid,
		rmp->mp_name);
	exit_proc(rmp, 0, TRUE /*dump_core*/);
  }
  else {
  	exit_proc(rmp, 0, FALSE /*dump_core*/);
  }
}

/*===========================================================================*
 *				check_sig				     *
 *===========================================================================*/
int check_sig(proc_id, signo)
pid_t proc_id;			/* pid of proc to sig, or 0 or -1, or -pgrp */
int signo;			/* signal to send to process (0 to _NSIG-1) */
{
/* Check to see if it is possible to send a signal.  The signal may have to be
 * sent to a group of processes.  This routine is invoked by the KILL system
 * call, and also when the kernel catches a DEL or other signal.
 */

  register struct mproc *rmp;
  int count;			/* count # of signals sent */
  int error_code;

  if (signo < 0 || signo >= _NSIG) return(-EINVAL);

  /* Return -EINVAL for attempts to send SIGKILL to INIT alone. */
  if (proc_id == INIT_PID && signo == SIGKILL) return(-EINVAL);

  /* Search the proc table for processes to signal.  
   * (See forkexit.c about pid magic.)
   */
  count = 0;
  error_code = -ESRCH;
  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++) {
	if (!(rmp->mp_flags & IN_USE)) continue;

	/* Check for selection. */
	if (proc_id > 0 && proc_id != rmp->mp_pid) continue;
	if (proc_id == 0 && mp->mp_procgrp != rmp->mp_procgrp) continue;
	if (proc_id == -1 && rmp->mp_pid <= INIT_PID) continue;
	if (proc_id < -1 && rmp->mp_procgrp != -proc_id) continue;

	/* Do not kill servers and drivers when broadcasting SIGKILL. */
	if (proc_id == -1 && signo == SIGKILL &&
		(rmp->mp_flags & PRIV_PROC)) continue;

	/* Check for permission. */
	if (mp->mp_effuid != SUPER_USER
	    && mp->mp_realuid != rmp->mp_realuid
	    && mp->mp_effuid != rmp->mp_realuid
	    && mp->mp_realuid != rmp->mp_effuid
	    && mp->mp_effuid != rmp->mp_effuid) {
		error_code = -EPERM;
		continue;
	}

	count++;
	if (signo == 0 || (rmp->mp_flags & EXITING)) continue;

	/* 'sig_proc' will handle the disposition of the signal.  The
	 * signal may be caught, blocked, ignored, or cause process
	 * termination, possibly with core dump.
	 */
	sig_proc(rmp, signo, TRUE /*trace*/);

	if (proc_id > 0) break;	/* only one process being signaled */
  }

  /* If the calling process has killed itself, don't reply. */
  if ((mp->mp_flags & (IN_USE | EXITING)) != IN_USE) return(SUSPEND);
  return(count > 0 ? 0 : error_code);
}

/*===========================================================================*
 *				check_pending				     *
 *===========================================================================*/
void check_pending(rmp)
register struct mproc *rmp;
{
  /* Check to see if any pending signals have been unblocked. Deliver as many
   * of them as we can, until we have to wait for a reply from VFS first.
   *
   * There are several places in this file where the signal mask is
   * changed.  At each such place, check_pending() should be called to
   * check for newly unblocked signals.
   */

  int i;

  for (i = 1; i < _NSIG; i++) {
	if (sigismember(&rmp->mp_sigpending, i) &&
		!sigismember(&rmp->mp_sigmask, i)) {
		sigdelset(&rmp->mp_sigpending, i);
		sig_proc(rmp, i, FALSE /*trace*/);

		if (rmp->mp_flags & FS_CALL)
			break;
	}
  }
}

/*===========================================================================*
 *				restart_sigs				     *
 *===========================================================================*/
void restart_sigs(rmp)
struct mproc *rmp;
{
/* FS has replied to a request from us; do signal-related work.
 */
  int r;

  if (rmp->mp_flags & (FS_CALL | EXITING)) return;

  if (rmp->mp_flags & TRACE_EXIT) {
	/* Tracer requested exit with specific exit value */
	exit_proc(rmp, rmp->mp_exitstatus, FALSE /*dump_core*/);
  }
  else if (rmp->mp_flags & PM_SIG_PENDING) {
	/* We saved signal(s) for after finishing a FS call. Deal with this.
	 * PM_SIG_PENDING remains set to indicate the process is still stopped.
	 */
	check_pending(rmp);

	/* The process may now be FS-blocked again, because a signal exited the
	 * process or was caught. Restart the process only when this is NOT the
	 * case.
	 */
	if (!(rmp->mp_flags & FS_CALL)) {
		rmp->mp_flags &= ~(PM_SIG_PENDING | UNPAUSED);

		if ((r = sys_resume(rmp->mp_endpoint)) != 0)
			panic(__FILE__, "sys_resume failed", r);
	}
  }
}

/*===========================================================================*
 *				unpause					     *
 *===========================================================================*/
static void unpause(rmp)
struct mproc *rmp;		/* which process */
{
/* A signal is to be sent to a process.  If that process is hanging on a
 * system call, the system call must be terminated with -EINTR.  Possible
 * calls are PAUSE, WAIT, READ and WRITE, the latter two for pipes and ttys.
 * First check if the process is hanging on an PM call.  If not, tell FS,
 * so it can check for READs and WRITEs from pipes, ttys and the like.
 */
  message m;
  int r, slot;

  /* If we're already waiting for a delayed call, don't do anything now. */
  if (rmp->mp_flags & DELAY_CALL)
	return;

  /* Check to see if process is hanging on a PAUSE, WAIT or SIGSUSPEND call. */
  if (rmp->mp_flags & (PAUSED | WAITING | SIGSUSPENDED)) {
	/* Stop process from running. No delay calls: it called us. */
	if ((r = sys_stop(rmp->mp_endpoint)) != 0)
		panic(__FILE__, "sys_stop failed", r);

	rmp->mp_flags |= UNPAUSED;

	/* We interrupt the actual call from sig_send() below. */
	return;
  }

  /* Not paused in PM. Let FS try to unpause the process. */
  if (!(rmp->mp_flags & PM_SIG_PENDING)) {
	/* Stop process from running. */
	r = sys_delay_stop(rmp->mp_endpoint);

	/* If the process is still busy sending a message, the kernel will give
	 * us -EBUSY now and send a SIGNDELAY to the process as soon as sending
	 * is done.
	 */
	if (r == -EBUSY) {
		rmp->mp_flags |= DELAY_CALL;

		return;
	}
	else if (r != 0) panic(__FILE__, "sys_stop failed", r);

	rmp->mp_flags |= PM_SIG_PENDING;
  }

  m.m_type = PM_UNPAUSE;
  m.PM_PROC = rmp->mp_endpoint;

  tell_fs(rmp, &m);

  /* Also tell VM. */
  vm_notify_sig_wrapper(rmp->mp_endpoint);
}

/*===========================================================================*
 *				sig_send				     *
 *===========================================================================*/
static int sig_send(rmp, signo)
struct mproc *rmp;		/* what process to spawn a signal handler in */
int signo;			/* signal to send to process (1 to _NSIG-1) */
{
/* The process is supposed to catch this signal. Spawn a signal handler.
 * Return TRUE if this succeeded, FALSE otherwise.
 */
  struct sigmsg sigmsg;
  vir_bytes cur_sp;
  int r, sigflags, slot;

  if (!(rmp->mp_flags & UNPAUSED))
	panic(__FILE__, "sig_send: process not unpaused", NO_NUM);

  sigflags = rmp->mp_sigact[signo].sa_flags;
  slot = (int) (rmp - mproc);

  if (rmp->mp_flags & SIGSUSPENDED)
	sigmsg.sm_mask = rmp->mp_sigmask2;
  else
	sigmsg.sm_mask = rmp->mp_sigmask; 
  sigmsg.sm_signo = signo;
  sigmsg.sm_sighandler =
	(vir_bytes) rmp->mp_sigact[signo].sa_handler;
  sigmsg.sm_sigreturn = rmp->mp_sigreturn;
  rmp->mp_sigmask |= rmp->mp_sigact[signo].sa_mask;

  if (sigflags & SA_NODEFER)
	sigdelset(&rmp->mp_sigmask, signo);
  else
	sigaddset(&rmp->mp_sigmask, signo);

  if (sigflags & SA_RESETHAND) {
	sigdelset(&rmp->mp_catch, signo);
	rmp->mp_sigact[signo].sa_handler = SIG_DFL;
  }
  sigdelset(&rmp->mp_sigpending, signo); 

  if(vm_push_sig(rmp->mp_endpoint, &cur_sp) != 0)
	return(FALSE);

  sigmsg.sm_stkptr = cur_sp;

  /* Ask the kernel to deliver the signal */
  r = sys_sigsend(rmp->mp_endpoint, &sigmsg);
  if (r != 0)
	panic(__FILE__, "sys_sigsend failed", r);

  /* Was the process suspended in PM? Then interrupt the blocking call. */
  if (rmp->mp_flags & (PAUSED | WAITING | SIGSUSPENDED)) {
	rmp->mp_flags &= ~(PAUSED | WAITING | SIGSUSPENDED);

	setreply(slot, -EINTR);
  }

  /* Was the process stopped just for this signal? Then resume it. */
  if ((rmp->mp_flags & (PM_SIG_PENDING | UNPAUSED)) == UNPAUSED) {
	rmp->mp_flags &= ~UNPAUSED;

	if ((r = sys_resume(rmp->mp_endpoint)) != 0)
		panic(__FILE__, "sys_resume failed", r);
  }

  return(TRUE);
}

/*===========================================================================*
 *				vm_notify_sig_wrapper			     *
 *===========================================================================*/
void vm_notify_sig_wrapper(endpoint_t ep)
{
/* get IPC's endpoint,
 * the reason that we directly get the endpoint
 * instead of from DS server is that otherwise
 * it will cause deadlock between PM, VM and DS.
 */
  struct mproc *rmp;
  endpoint_t ipc_ep = 0;

  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++) {
	if (!(rmp->mp_flags & IN_USE))
		continue;
	if (!strcmp(rmp->mp_name, "ipc")) {
		ipc_ep = rmp->mp_endpoint;
		vm_notify_sig(ep, ipc_ep);

		return;
	}
  }
}

#define p_set		m1_p1
#define how_value	m1_i1
#define p_oldset	m1_p2

int sys_sigpending(void)
{
	int err;

	if (!m_in.p_set)
		return -EFAULT;

	err = sys_datacopy(SELF, (vir_bytes)&mp->mp_sigpending,
			  mp->mp_endpoint, (vir_bytes)m_in.p_set,
			  sizeof(sigset_t));

	if (err != 0)
		return -EFAULT;

	return 0;
}

int sys_sigprocmask(void)
{
	/* Note that the library interface passes the actual mask in sigmask_set,
	 * not a pointer to the mask, in order to save a copy.  Similarly,
	 * the old mask is placed in the return message which the library
	 * interface copies (if requested) to the user specified address.
	 *
	 * KILL and STOP can't be masked.
	 */
	int i;
	int err;
	sigset_t set;

	/* If `set' is null then ignore `how' and store the sigmask into `oldset' */
	if (!m_in.p_set) {
		if (m_in.p_oldset) {
			err = sys_datacopy(SELF, (vir_bytes)&mp->mp_sigmask,
					  mp->mp_endpoint, (vir_bytes)m_in.p_oldset,
					  sizeof(sigset_t));

			if (err != 0)
				return -EFAULT;

		} else {
			return -EFAULT;
		}

		return 0;
	}

	/* if `oldset' non-null then make a copy */
	if (m_in.p_oldset) {
		err = sys_datacopy(SELF, (vir_bytes)&mp->mp_sigmask,
				   mp->mp_endpoint, (vir_bytes)m_in.p_oldset,
				   sizeof(sigset_t));

		if (err != 0)
			return -EFAULT;
	}

	/* Get the new `set'. */
	err = sys_datacopy(mp->mp_endpoint, (vir_bytes)m_in.p_set,
			   SELF, (vir_bytes)&set,
			   sizeof(sigset_t));

	if (err != 0)
		return -EFAULT;

	switch (m_in.how_value) {
	case SIG_BLOCK:
		sigdelset((sigset_t *)&set, SIGKILL);
		sigdelset((sigset_t *)&set, SIGSTOP);

		for (i = 1; i < _NSIG; i++) {
			if (sigismember((sigset_t *)&set, i))
				sigaddset(&mp->mp_sigmask, i);
		}
		break;

	case SIG_UNBLOCK:
		for (i = 1; i < _NSIG; i++) {
			if (sigismember((sigset_t *)&set, i))
				sigdelset(&mp->mp_sigmask, i);
		}
		check_pending(mp);
		break;

	case SIG_SETMASK:
		sigdelset((sigset_t *) &set, SIGKILL);
		sigdelset((sigset_t *) &set, SIGSTOP);
		mp->mp_sigmask = (sigset_t)set;
		check_pending(mp);
		break;

	default:
		return -EINVAL;
		break;
	}

	return 0;
}

int sys_sigsuspend(void)
{
	int err;
	sigset_t set;

	/* Get the `set' argument */
	err = sys_datacopy(mp->mp_endpoint, (vir_bytes)m_in.p_set,
			   SELF, (vir_bytes)&set,
			   sizeof(sigset_t));

	if (err != 0)
		return -EFAULT;

	mp->mp_sigmask2 = mp->mp_sigmask;	/* save the old mask */
	mp->mp_sigmask = (sigset_t) set;

	sigdelset(&mp->mp_sigmask, SIGKILL);
	sigdelset(&mp->mp_sigmask, SIGSTOP);

	mp->mp_flags |= SIGSUSPENDED;

	check_pending(mp);

	return SUSPEND;
}
