/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Function prototypes for the system library.  The prototypes in this file 
 * are undefined to do_unused if the kernel call is not enabled in config.h. 
 * The implementation is contained in src/kernel/system/.  
 *
 * The system library allows to access system services by doing a kernel call.
 * System calls are transformed into request messages to the SYS task that is 
 * responsible for handling the call. By convention, kipc_call() is transformed 
 * into a message with type SYS_CALL that is handled in a function do_call(). 
 *
 * Changes:
 *   Jul 30, 2005   created SYS_INT86 to support BIOS driver  (Philip Homburg) 
 *   Jul 13, 2005   created SYS_PRIVCTL to manage services  (Jorrit N. Herder) 
 *   Jul 09, 2005   updated SYS_KILL to signal services  (Jorrit N. Herder) 
 *   Jun 21, 2005   created SYS_NICE for nice(2) kernel call  (Ben J. Gras)
 *   Jun 21, 2005   created SYS_MEMSET to speed up exec(2)  (Ben J. Gras)
 *   Apr 12, 2005   updated SYS_VCOPY for virtual_copy()  (Jorrit N. Herder)
 *   Jan 20, 2005   updated SYS_COPY for virtual_copy()  (Jorrit N. Herder)
 *   Oct 24, 2004   created SYS_GETKSIG to support PM  (Jorrit N. Herder) 
 *   Oct 10, 2004   created handler for unused calls  (Jorrit N. Herder) 
 *   Sep 09, 2004   updated SYS_EXIT to let services exit  (Jorrit N. Herder) 
 *   Aug 25, 2004   rewrote SYS_SETALARM to clean up code  (Jorrit N. Herder)
 *   Jul 13, 2004   created SYS_SEGCTL to support drivers  (Jorrit N. Herder) 
 *   May 24, 2004   created SYS_SDEVIO to support drivers  (Jorrit N. Herder) 
 *   May 24, 2004   created SYS_GETINFO to retrieve info  (Jorrit N. Herder) 
 *   Apr 18, 2004   created SYS_VDEVIO to support drivers  (Jorrit N. Herder) 
 *   Feb 24, 2004   created SYS_IRQCTL to support drivers  (Jorrit N. Herder) 
 *   Feb 02, 2004   created SYS_DEVIO to support drivers  (Jorrit N. Herder) 
 */

#ifndef __KERNEL_SYSTEM_H
#define __KERNEL_SYSTEM_H

/* Common includes for the system library. */
#include <kernel/kernel.h>
#include <kernel/proto.h>
#include <kernel/proc.h>

#ifdef __KERNEL__

/* Default handler for unused kernel calls. */
int do_unused(kipc_msg_t *m_ptr);

int do_exec(kipc_msg_t *m_ptr);
#if ! USE_EXEC
#define do_exec do_unused
#endif

int do_fork(kipc_msg_t *m_ptr);
#if ! USE_FORK
#define do_fork do_unused
#endif

int do_newmap(kipc_msg_t *m_ptr);
#if ! USE_NEWMAP
#define do_newmap do_unused
#endif

int do_exit(kipc_msg_t *m_ptr);
#if ! USE_EXIT
#define do_exit do_unused
#endif

int do_trace(kipc_msg_t *m_ptr);
#if ! USE_TRACE
#define do_trace do_unused
#endif

int do_nice(kipc_msg_t *m_ptr);
#if ! USE_NICE
#define do_nice do_unused
#endif

int do_runctl(kipc_msg_t *m_ptr);
#if ! USE_RUNCTL
#define do_runctl do_unused
#endif

int do_copy(kipc_msg_t *m_ptr);
#define do_vircopy 	do_copy
#if ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy do_unused
#endif

int do_vcopy(kipc_msg_t *m_ptr);
#define do_virvcopy 	do_vcopy
#if ! (USE_VIRVCOPY || USE_PHYSVCOPY)
#define do_vcopy do_unused
#endif

int do_umap(kipc_msg_t *m_ptr);
#if ! USE_UMAP
#define do_umap do_unused
#endif

int do_memset(kipc_msg_t *m_ptr);
#if ! USE_MEMSET
#define do_memset do_unused
#endif

int do_abort(kipc_msg_t *m_ptr);
#if ! USE_ABORT
#define do_abort do_unused
#endif

int do_getinfo(kipc_msg_t *m_ptr);
#if ! USE_GETINFO
#define do_getinfo do_unused
#endif

int do_privctl(kipc_msg_t *m_ptr);
#if ! USE_PRIVCTL
#define do_privctl do_unused
#endif

int do_segctl(kipc_msg_t *m_ptr);
#if ! USE_SEGCTL
#define do_segctl do_unused
#endif

int do_irqctl(kipc_msg_t *m_ptr);
#if ! USE_IRQCTL
#define do_irqctl do_unused
#endif

int do_devio(kipc_msg_t *m_ptr);
#if ! USE_DEVIO
#define do_devio do_unused
#endif

int do_vdevio(kipc_msg_t *m_ptr);
#if ! USE_VDEVIO
#define do_vdevio do_unused
#endif

int do_int86(kipc_msg_t *m_ptr);

int do_sdevio(kipc_msg_t *m_ptr);
#if ! USE_SDEVIO
#define do_sdevio do_unused
#endif

int do_kill(kipc_msg_t *m_ptr);
#if ! USE_KILL
#define do_kill do_unused
#endif

int do_getksig(kipc_msg_t *m_ptr);
#if ! USE_GETKSIG
#define do_getksig do_unused
#endif

int do_endksig(kipc_msg_t *m_ptr);
#if ! USE_ENDKSIG
#define do_endksig do_unused
#endif

int do_sigsend(kipc_msg_t *m_ptr);
#if ! USE_SIGSEND
#define do_sigsend do_unused
#endif

int do_sigreturn(kipc_msg_t *m_ptr);
#if ! USE_SIGRETURN
#define do_sigreturn do_unused
#endif

int do_times(kipc_msg_t *m_ptr);
#if ! USE_TIMES
#define do_times do_unused
#endif

int do_setalarm(kipc_msg_t *m_ptr);
#if ! USE_SETALARM
#define do_setalarm do_unused
#endif

int do_stime(kipc_msg_t *m_ptr);

int do_vtimer(kipc_msg_t *m_ptr);
#if ! USE_VTIMER
#define do_vtimer do_unused
#endif

int do_safecopy(kipc_msg_t *m_ptr);
int do_vsafecopy(kipc_msg_t *m_ptr);
int do_iopenable(kipc_msg_t *m_ptr);
int do_vmctl(kipc_msg_t *m_ptr);
int do_setgrant(kipc_msg_t *m_ptr);
int do_readbios(kipc_msg_t *m_ptr);
int do_mapdma(kipc_msg_t *m_ptr);

int do_sprofile(kipc_msg_t *m_ptr);
#ifndef CONFIG_DEBUG_KERNEL_STATS_PROFILE
#define do_sprofile do_unused
#endif

int do_cprofile(kipc_msg_t *m_ptr);
int do_profbuf(kipc_msg_t *m_ptr);

int do_mapdma(kipc_msg_t *m_ptr);
long do_strnlen(kipc_msg_t *m_ptr);
#endif /* __KERNEL__ */
#endif	/* __KERNEL_SYSTEM_H */

