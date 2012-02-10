/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Function prototypes. */

#ifndef __KERNEL_PROTO_H
#define __KERNEL_PROTO_H

#include <nucleos/timer.h>
#include <nucleos/safecopies.h>
#include <nucleos/a.out.h>
#include <nucleos/kipc.h>
#include <asm/kernel/types.h>
#include <asm/kernel/proto.h>

#ifdef __KERNEL__

/* Struct declarations. */
struct proc;
struct timer;

/* clock.c */
void clock_task(void);
clock_t get_uptime(void);
void set_timer(struct timer *tp, clock_t t, tmr_func_t f);
void reset_timer(struct timer *tp);
void ser_dump_proc(void);

/* main.c */
void main(void);
void prepare_shutdown(int how);
void nucleos_shutdown(struct timer *tp);

/* utility.c */
int printk(const char *fmt, ...);
void kernel_panic(char *s, int n);

/* proc.c */
int kipc_call(u8 call_type, u32 flags, endpoint_t endpt, void *msg);
int lock_notify(int src, int dst);
int mini_notify(struct proc *src, endpoint_t dst);
int lock_send(int dst, kipc_msg_t *m_ptr);
void enqueue(struct proc *rp);
void dequeue(struct proc *rp);
void balance_queues(struct timer *tp);
struct proc* schedcheck(void);
struct proc* arch_finish_schedcheck(void);
struct proc *endpoint_lookup(endpoint_t ep);

#ifdef CONFIG_DEBUG_KERNEL_IPC_WARNINGS
int isokendpt_f(char *file, int line, endpoint_t e, int *p, int f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#else
int isokendpt_f(endpoint_t e, int *p, int f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif

/* system.c */
int get_priv(register struct proc *rc, int proc_type);
void set_sendto_bit(struct proc *rc, int id);
void unset_sendto_bit(struct proc *rc, int id);
void send_sig(int proc_nr, int sig_nr);
void cause_sig(int proc_nr, int sig_nr);
void sig_delay_done(struct proc *rp);
void sys_task(void);

#define numap_local(proc_nr, vir_addr, bytes) \
	umap_local(proc_addr(proc_nr), D, (vir_addr), (bytes))

phys_bytes umap_grant(struct proc *, cp_grant_id_t, vir_bytes);
void clear_endpoint(struct proc *rc);
phys_bytes umap_bios(vir_bytes vir_addr, vir_bytes bytes);

/* system/do_newmap.c */
int newmap(struct proc *rp, struct mem_map *map_ptr);

/* system/do_vtimer.c */
void vtimer_check(struct proc *rp);

/* interrupt.c */
void put_irq_handler(irq_hook_t *hook, int irq, irq_handler_t handler);
void rm_irq_handler(irq_hook_t *hook);
void enable_irq(irq_hook_t *hook);
int disable_irq(irq_hook_t *hook);
void cons_setc(int pos, int c);
void cons_seth(int pos, int n);

/* debug.c */
#ifdef CONFIG_DEBUG_KERNEL_SCHED_CHECK
#define CHECK_RUNQUEUES check_runqueues_f(__FILE__, __LINE__)
void check_runqueues_f(char *file, int line);
#endif
char *rtsflagstr(int flags);
char *miscflagstr(int flags);

/* system/do_safecopy.c */
int verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, int, vir_bytes, vir_bytes *,
		 endpoint_t *);

/* system/do_sysctl.c */
int do_sysctl(kipc_msg_t *m);

#ifdef CONFIG_DEBUG_KERNEL_STATS_PROFILE
/* profile.c */
void init_profile_clock(u32_t);
void stop_profile_clock(void);
#endif

/* functions defined in architecture-dependent files. */
void prot_init(void);
phys_bytes phys_copy(phys_bytes dest, phys_bytes source, phys_bytes count);
void phys_copy_fault(void);
void phys_copy_fault_in_kernel(void);

#define virtual_copy(src, dst, bytes) virtual_copy_f(src, dst, bytes, 0)
#define virtual_copy_vmcheck(src, dst, bytes) virtual_copy_f(src, dst, bytes, 1)

int virtual_copy_f(struct vir_addr *src, struct vir_addr *dst, vir_bytes bytes, int vmcheck);
int data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to, vir_bytes to_addr,
	      size_t bytes);
int data_copy_vmcheck(endpoint_t from, vir_bytes from_addr, endpoint_t to, vir_bytes to_addr,
		      size_t bytes);

#define data_copy_to(d, p, v, n) data_copy(SYSTEM, (d), (p), (v), (n));
#define data_copy_from(d, p, v, n) data_copy((p), (v), SYSTEM, (d), (n));

void alloc_segments(struct proc *rp);
void vm_init(struct proc *first);
int vm_copy(vir_bytes src, struct proc *srcproc, vir_bytes dst, struct proc *dstproc,
	    phys_bytes bytes);
phys_bytes umap_local(register struct proc *rp, int seg, vir_bytes vir_addr, vir_bytes bytes);
void cp_mess(int src,phys_clicks src_clicks, vir_bytes src_offset, phys_clicks dst_clicks,
	     vir_bytes dst_offset);
phys_bytes umap_remote(struct proc* rp, int seg, vir_bytes vir_addr, vir_bytes bytes);
phys_bytes umap_virtual(struct proc* rp, int seg, vir_bytes vir_addr, vir_bytes bytes);
phys_bytes seg2phys(u16);
int vm_phys_memset(phys_bytes source, u8_t pattern, phys_bytes count);
vir_bytes alloc_remote_segment(u32_t *, segframe_t *, int, phys_bytes, vir_bytes, int);
int intr_init(int, int);
int intr_disabled(void);
void halt_cpu(void);
void arch_init(void);
void ser_putc(char);
void arch_shutdown(int);
struct exec *arch_get_aout_header(int i);
void arch_copy_aout_headers(void);
void restart(void);
void read_tsc(u32_t *high, u32_t *low);
int arch_init_profile_clock(u32_t freq);
void arch_stop_profile_clock(void);
void arch_ack_profile_clock(void);
void do_ser_debug(void);
int arch_get_params(char *parm, int max);
int arch_set_params(char *parm, int max);
int arch_pre_exec(struct proc *pr, u32_t, u32_t);
int arch_umap(struct proc *pr, vir_bytes, vir_bytes, int, phys_bytes *);
int arch_do_vmctl(kipc_msg_t *m_ptr, struct proc *p);
int vm_contiguous(struct proc *targetproc, u32_t vir_buf, size_t count);
void proc_stacktrace(struct proc *proc);
int vm_lookup(struct proc *proc, vir_bytes virtual, vir_bytes *result, u32_t *ptent);
int vm_suspend(struct proc *caller, struct proc *target,
	phys_bytes lin, phys_bytes size, int wrflag, int type);
int delivermsg(struct proc *target);
void arch_do_syscall(struct proc *proc);
int arch_phys_map(int index, phys_bytes *addr,
	phys_bytes *len, int *flags);
int arch_phys_map_reply(int index, vir_bytes addr);
int arch_enable_paging(void);

#endif /* __KERNEL__ */
#endif /* __KERNEL_PROTO_H */
