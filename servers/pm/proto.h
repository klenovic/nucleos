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

/* devio.c */
int do_dev_io(void);
int do_dev_io(void);

/* dma.c */
int do_adddma(void);
int do_deldma(void);
int do_getdma(void);
void release_dma(endpoint_t proc_e, phys_clicks base, phys_clicks size);

/* dmp.c */
int do_fkey_pressed(void);

/* exec.c */
int do_exec(void);
int exec_newmem(void);
int do_execrestart(void);
void exec_restart(struct mproc *rmp, int result);

/* forkexit.c */
int do_fork(void);
int do_fork_nb(void);
int do_exit(void);
int do_waitpid(void);
void exit_proc(struct mproc *rmp, int exit_status, int dump_core);
void exit_restart(struct mproc *rmp, int dump_core);

/* getset.c */
int do_getset(void);

/* kputc.c */
void diag_repl(void);

/* main.c */
int main(void);

/* misc.c */
int do_reboot(void);
int do_procstat(void);
int do_sysuname(void);
int do_getsysinfo(void);
int do_getsysinfo_up(void);
int do_getprocnr(void);
int do_getpuid(void);
int do_svrctl(void);
int do_allocmem(void);
int do_freemem(void);
int do_getsetpriority(void);

void setreply(int proc_nr, int result);

/* profile.c */
int do_sprofile(void);
int do_cprofile(void);

/* signal.c */
int do_kill(void);
int ksig_pending(void);
int do_pause(void);
int check_sig(pid_t proc_id, int signo);
void sig_proc(struct mproc *rmp, int sig_nr);
int do_sigaction(void);
int do_sigpending(void);
int do_sigprocmask(void);
int do_sigreturn(void);
int do_sigsuspend(void);
void check_pending(struct mproc *rmp);
int vm_notify_sig_wrapper(endpoint_t ep); 

/* time.c */
int do_stime(void);
int do_time(void);
int do_times(void);
int do_gettimeofday(void);

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
void panic(char *who, char *mess, int num);
char *find_param(const char *key);
int proc_from_pid(pid_t p);
int pm_isokendpt(int ep, int *proc);

#endif /*  __SERVERS_PM_PROTO_H */
