/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_PM_PROTO_H
#define __SERVERS_PM_PROTO_H

/* Function prototypes. */
struct mproc;
struct stat;
struct mem_map;
struct memory;

#include <nucleos/timer.h>

/* alarm.c */
int do_alarm(void);
int do_itimer(void);
void set_alarm(struct mproc *rmp, clock_t ticks);
void check_vtimer(int proc_nr, int sig);

/* break.c */
int do_brk(void);

/* dma.c */
int do_adddma(void);
int do_deldma(void);
int do_getdma(void);

/* exec.c */
int do_exec(void);
int exec_newmem(void);
int do_execrestart(void);
void exec_restart(struct mproc *rmp, int result);

/* forkexit.c */
int do_fork(void);
int do_fork_nb(void);
int do_exit(void);
void exit_proc(struct mproc *rmp, int exit_status, int dump_core);
void exit_restart(struct mproc *rmp, int dump_core);
int do_waitpid(void);
int wait_test(struct mproc *rmp, struct mproc *child);

/* getset.c */
int do_get(void);
int do_set(void);

/* main.c */
int main(void);
void setreply(int proc_nr, int result);

/* misc.c */
int do_reboot(void);
int do_procstat(void);
int do_sysuname(void);
int do_getsysinfo(void);
int do_getsysinfo_up(void);
int do_getprocnr(void);
int do_getepinfo(void);
int do_svrctl(void);
int do_allocmem(void);
int do_freemem(void);
int do_getsetpriority(void);

/* profile.c */
int do_sprofile(void);
int do_cprofile(void);

/* signal.c */
int do_kill(void);
int ksig_pending(void);
int do_pause(void);
int check_sig(pid_t proc_id, int signo);
void sig_proc(struct mproc *rmp, int signo, int trace);
int do_sigaction(void);
int do_sigpending(void);
int do_sigprocmask(void);
int do_sigreturn(void);
int do_sigsuspend(void);
void check_pending(struct mproc *rmp);
void restart_sigs(struct mproc *rmp);
void vm_notify_sig_wrapper(endpoint_t ep); 

/* time.c */
int do_stime(void);
int do_time(void);
int do_times(void);

/* timers.c */
void pm_set_timer(timer_t *tp, int delta, tmr_func_t watchdog, int arg);
void pm_expire_timers(clock_t now);
void pm_cancel_timer(timer_t *tp);

/* trace.c */
int do_trace(void);
void stop_proc(struct mproc *rmp, int sig_nr);

/* utility.c */
pid_t get_free_pid(void);
int no_sys(void);
char *find_param(const char *key);
struct mproc *find_proc(pid_t lpid);
int pm_isokendpt(int ep, int *proc);
void tell_fs(struct mproc *rmp, message *m_ptr);

#endif /*  __SERVERS_PM_PROTO_H */
