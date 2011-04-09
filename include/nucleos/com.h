/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file defines constants for use in message communication (mostly)
 * between system processes.
 *
 * A number of protocol message request and response types are defined. For
 * debugging purposes, each protocol is assigned its own unique number range.
 * The following such message type ranges have been allocated:
 *
 *        1 -   0xFF	POSIX requests (see callnr.h)
 *    0x200 -  0x2FF	Data link layer requests and responses
 *    0x300 -  0x3FF	Bus controller requests and responses
 *    0x400 -  0x4FF	Block and character device requests
 *    0x500 -  0x5FF	Block and character device responses
 *    0x600 -  0x6FF	Kernel calls to SYSTEM task
 *    0x700 -  0x7FF	Reincarnation Server (RS) requests
 *    0x800 -  0x8FF	Data Store (DS) requests
 *    0x900 -  0x9FF	Requests from PM to VFS, and responses
 *   (0xA00 -  0xAFF	old TTY and LOG requests, being phased out)
 *    0xA00 -  0xAFF	Requests from VFS to file systems (see vfsif.h)
 *    0xB00 -  0xBFF	Requests from VM to VFS
 *    0xC00 -  0xCFF	Virtual Memory (VM) requests
 *    0xD00 -  0xDFF	IPC server requests
 *   0x1000 - 0x10FF	Notify messages
 *
 * Zero and negative values are widely used for OK and error responses.
 */

#ifndef __NUCLEOS_COM_H
#define __NUCLEOS_COM_H

#ifndef __ASSEMBLY__

/* Magic process numbers, these may not be any valid endpoint
 * (see <nucleos/endpoint.h>).
 */
#define ENDPT_ANY		0x7ace	/* used to indicate 'any process' */
#define ENDPT_NONE		0x6ace	/* used to indicate 'no process at all' */
#define ENDPT_SELF		0x8ace	/* used to indicate 'own process' */
#define _MAX_MAGIC_PROC		(ENDPT_SELF)	/* used by <nucleos/endpoint.h>
						   to determine generation size */

/*===========================================================================*
 *            	Process numbers of processes in the system image	     *
 *===========================================================================*/

/* The values of several task numbers depend on whether they or other tasks
 * are enabled. They are defined as (PREVIOUS_TASK - ENABLE_TASK) in general.
 * ENABLE_TASK is either 0 or 1, so a task either gets a new number, or gets
 * the same number as the previous task and is further unused. Note that the
 * order should correspond to the order in the task table defined in table.c. 
 */

/* Kernel tasks. These all run in the same address space. */
#define IDLE		-4	/* runs when no one else can run */
#define CLOCK		-3	/* alarms and other clock functions */
#define SYSTEM		-2	/* request system functionality */
#define KERNEL		-1	/* pseudo-process for IPC and scheduling */
#define HARDWARE	KERNEL	/* for hardware interrupt handlers */

/* Number of tasks. */
#define MAX_NR_TASKS	1023
#define NR_TASKS	4

/* Number of slots in the process table for non-kernel processes. The number
 * of system processes defines how many processes with special privileges
 * there can be. User processes share the same properties and count for one.
 */
#define NR_SYS_PROCS	CONFIG_NR_SYS_PROCS
#define NR_PROCS	CONFIG_NR_PROCS

/* Number of controller tasks (/dev/cN device classes). */
#define NR_CTRLRS	CONFIG_NR_CTRLRS

/* User-space processes, that is, device drivers, servers, and INIT. */
#define PM_PROC_NR	0	/* process manager */
#define VFS_PROC_NR	1	/* Virtual filesystem */
#define RS_PROC_NR 	2	/* reincarnation server */
#define MEM_PROC_NR	3	/* memory driver (RAM disk, null, etc.) */
#define LOG_PROC_NR	4	/* log device driver */
#define TTY_PROC_NR	5	/* terminal (TTY) driver */
#define DS_PROC_NR	6	/* data store server */
#define MFS_PROC_NR	7	/* nucleos root filesystem */
#define VM_PROC_NR	8	/* memory server */
#define PFS_PROC_NR	9	/* pipe filesystem */
#define EXT2_PROC_NR	10	/* ext2 filesystem */
#define INIT_PROC_NR	11	/* init -- goes multiuser */

/* Root system process and root user process. */
#define ROOT_SYS_PROC_NR	RS_PROC_NR
#define ROOT_USR_PROC_NR	INIT_PROC_NR

/* Number of processes contained in the system image. */
#define NR_BOOT_PROCS	(NR_TASKS + INIT_PROC_NR + 1)

/* Which processes should receive diagnostics from the kernel and system?
 * Directly sending it to TTY only displays the output. Sending it to the
 * log driver will cause the diagnostics to be buffered and displayed.
 * Messages are sent by src/lib/sysutil/kputc.c to these processes, in
 * the order of this array, which must be terminated by ENDPT_NONE. This is used
 * by drivers and servers that printk().
 * The kernel does this for its own printk() in kernel/utility.c, also using
 * this array, but a slightly different mechanism.
 */
#define OUTPUT_PROCS_ARRAY	{ TTY_PROC_NR, LOG_PROC_NR, ENDPT_NONE }

/*===========================================================================*
 *                	   Kernel notification types                         *
 *===========================================================================*/

/* Kernel notification types. In principle, these can be sent to any process,
 * so make sure that these types do not interfere with other message types.
 * Notifications are prioritized because of the way they are unhold() and
 * blocking notifications are delivered. The lowest numbers go first. The
 * offset are used for the per-process notification bit maps. 
 */
#define NOTIFY_MESSAGE		  0x1000
/* FIXME will be is_notify(a)		((a) == NOTIFY_MESSAGE) */
#define is_notify(a)		((a) & NOTIFY_MESSAGE)
#define NOTIFY_FROM(p_nr)	 (NOTIFY_MESSAGE | ((p_nr) + NR_TASKS)) 

/* Shorthands for message parameters passed with notifications. */
#define NOTIFY_ARG		m_data4
#define NOTIFY_TIMESTAMP	m_data5

/*===========================================================================*
 *                Messages for BUS controller drivers 			     *
 *===========================================================================*/
#define BUSC_RQ_BASE	0x300	/* base for request types */
#define BUSC_RS_BASE	0x380	/* base for response types */

#define BUSC_PCI_INIT		(BUSC_RQ_BASE + 0)	/* First message to
							 * PCI driver
							 */
#define BUSC_PCI_FIRST_DEV	(BUSC_RQ_BASE + 1)	/* Get index (and
							 * vid/did) of the
							 * first PCI device
							 */
#define BUSC_PCI_NEXT_DEV	(BUSC_RQ_BASE + 2)	/* Get index (and
							 * vid/did) of the
							 * next PCI device
							 */
#define BUSC_PCI_FIND_DEV	(BUSC_RQ_BASE + 3)	/* Get index of a
							 * PCI device based on
							 * bus/dev/function
							 */
#define BUSC_PCI_IDS		(BUSC_RQ_BASE + 4)	/* Get vid/did from an
							 * index
							 */
#define BUSC_PCI_DEV_NAME	(BUSC_RQ_BASE + 5)	/* Get the name of a
							 * PCI device
							 */
#define BUSC_PCI_SLOT_NAME	(BUSC_RQ_BASE + 6)	/* Get the name of a
							 * PCI slot
							 */
#define BUSC_PCI_RESERVE	(BUSC_RQ_BASE + 7)	/* Reserve a PCI dev */
#define BUSC_PCI_ATTR_R8	(BUSC_RQ_BASE + 8)	/* Read 8-bit
							 * attribute value
							 */
#define BUSC_PCI_ATTR_R16	(BUSC_RQ_BASE + 9)	/* Read 16-bit
							 * attribute value
							 */
#define BUSC_PCI_ATTR_R32	(BUSC_RQ_BASE + 10)	/* Read 32-bit
							 * attribute value
							 */
#define BUSC_PCI_ATTR_W8	(BUSC_RQ_BASE + 11)	/* Write 8-bit
							 * attribute value
							 */
#define BUSC_PCI_ATTR_W16	(BUSC_RQ_BASE + 12)	/* Write 16-bit
							 * attribute value
							 */
#define BUSC_PCI_ATTR_W32	(BUSC_RQ_BASE + 13)	/* Write 32-bit
							 * attribute value
							 */
#define BUSC_PCI_RESCAN		(BUSC_RQ_BASE + 14)	/* Rescan bus */
#define BUSC_PCI_DEV_NAME_S	(BUSC_RQ_BASE + 15)	/* Get the name of a
							 * PCI device
							 * (safecopy)
							 */
#define BUSC_PCI_SLOT_NAME_S	(BUSC_RQ_BASE + 16)	/* Get the name of a
							 * PCI slot (safecopy)
							 */
#define BUSC_PCI_SET_ACL	(BUSC_RQ_BASE + 17)	/* Set the ACL for a
							 * driver (safecopy)
							 */
#define BUSC_PCI_DEL_ACL	(BUSC_RQ_BASE + 18)	/* Delete the ACL of a
							 * driver 
							 */
#define IOMMU_MAP		(BUSC_RQ_BASE + 32)	/* Ask IOMMU to map
							 * a segment of memory
							 */



/*===========================================================================*
 *                Messages for BLOCK and CHARACTER device drivers	     *
 *===========================================================================*/

/* Message types for device drivers. */
#define DEV_RQ_BASE   0x400	/* base for device request types */
#define DEV_RS_BASE   0x500	/* base for device response types */

#define CANCEL       	(DEV_RQ_BASE +  0) /* force a task to cancel */
#define DEV_OPEN     	(DEV_RQ_BASE +  6) /* open a minor device */
#define DEV_CLOSE    	(DEV_RQ_BASE +  7) /* close a minor device */
#define TTY_SETPGRP 	(DEV_RQ_BASE + 10) /* set process group */
#define TTY_EXIT	(DEV_RQ_BASE + 11) /* process group leader exited */	
#define DEV_SELECT	(DEV_RQ_BASE + 12) /* request select() attention */
#define DEV_STATUS   	(DEV_RQ_BASE + 13) /* request driver status */
#define DEV_REOPEN     	(DEV_RQ_BASE + 14) /* reopen a minor device */

#define DEV_READ_S	(DEV_RQ_BASE + 20) /* (safecopy) read from minor */
#define DEV_WRITE_S   	(DEV_RQ_BASE + 21) /* (safecopy) write to minor */
#define DEV_SCATTER_S  	(DEV_RQ_BASE + 22) /* (safecopy) write from a vector */
#define DEV_GATHER_S   	(DEV_RQ_BASE + 23) /* (safecopy) read into a vector */
#define DEV_IOCTL_S    	(DEV_RQ_BASE + 24) /* (safecopy) I/O control code */
#define DEV_MMAP_S     	(DEV_RQ_BASE + 25) /* (safecopy) mmap interface */

#define DEV_REPLY       (DEV_RS_BASE + 0) /* general task reply */
#define DEV_CLONED      (DEV_RS_BASE + 1) /* return cloned minor */
#define DEV_REVIVE      (DEV_RS_BASE + 2) /* driver revives process */
#define DEV_IO_READY    (DEV_RS_BASE + 3) /* selected device ready */
#define DEV_NO_STATUS   (DEV_RS_BASE + 4) /* empty status reply */
#define DEV_REOPEN_REPL (DEV_RS_BASE + 5) /* reply to DEV_REOPEN */
#define DEV_CLOSE_REPL	(DEV_RS_BASE + 6) /* reply to DEV_CLOSE */
#define DEV_SEL_REPL1	(DEV_RS_BASE + 7) /* first reply to DEV_SELECT */
#define DEV_SEL_REPL2	(DEV_RS_BASE + 8) /* (opt) second reply to DEV_SELECT */

/* Field names for messages to block and character device drivers. */
#define DEVICE    	m_data1	/* major-minor device */
#define IO_ENDPT	m_data2	/* which (proc/endpoint) wants I/O? */
#define COUNT   	m_data3	/* how many bytes to transfer */
#define REQUEST 	m_data3	/* ioctl request code */
#define POSITION	m_data4	/* file offset (low 4 bytes) */
#define HIGHPOS		m_data5	/* file offset (high 4 bytes) */
#define ADDRESS 	m_data6	/* core buffer address */
#define IO_GRANT 	m_data6	/* grant id (for DEV_*_S variants) */

/* Field names for DEV_SELECT messages to device drivers. */
#define DEV_MINOR	m_data1	/* minor device */
#define DEV_SEL_OPS	m_data2	/* which select operations are requested */

/* Field names used in reply messages from tasks. */
#define REP_ENDPT	m_data1	/* # of proc on whose behalf I/O was done */
#define REP_STATUS	m_data2	/* bytes transferred or error number */
#define REP_IO_GRANT	m_data3	/* DEV_REVIVE: grant by which I/O was done */
#  define SUSPEND 	 -998 	/* status to suspend caller, reply later */

/* Field names for messages to TTY driver. */
#define TTY_LINE	DEVICE	/* message parameter: terminal line */
#define TTY_REQUEST	COUNT	/* message parameter: ioctl request code */
#define TTY_SPEK	POSITION/* message parameter: ioctl speed, erasing */
#define TTY_PGRP 	m_data3	/* message parameter: process group */	

/* Field names for the QIC 02 status reply from tape driver */
#define TAPE_STAT0	m_data4
#define TAPE_STAT1	m_data5

/*===========================================================================*
 *                  	   Messages for networking layer		     *
 *===========================================================================*/

/* Message types for network layer requests. This layer acts like a driver. */
#define NW_OPEN		DEV_OPEN
#define NW_CLOSE	DEV_CLOSE
#define NW_READ		DEV_READ
#define NW_WRITE	DEV_WRITE
#define NW_IOCTL	DEV_IOCTL
#define NW_CANCEL	CANCEL

/* Base type for data link layer requests and responses. */
#define DL_RQ_BASE	0x200		
#define DL_RS_BASE	0x280		

/* Message types for data link layer requests. */
#define DL_WRITE	(DL_RQ_BASE + 3)
#define DL_WRITEV	(DL_RQ_BASE + 4)
#define DL_READ		(DL_RQ_BASE + 5)
#define DL_READV	(DL_RQ_BASE + 6)
#define DL_CONF		(DL_RQ_BASE + 7)
#define DL_STOP		(DL_RQ_BASE + 8)
#define DL_GETSTAT	(DL_RQ_BASE + 9)
#define DL_GETNAME	(DL_RQ_BASE +10)
#define DL_WRITEV_S	(DL_RQ_BASE +11)
#define DL_READV_S	(DL_RQ_BASE +12)
#define DL_GETSTAT_S	(DL_RQ_BASE +13)

/* Message type for data link layer replies. */
#define DL_CONF_REPLY	(DL_RS_BASE + 20)
#define DL_TASK_REPLY	(DL_RS_BASE + 21)
#define DL_NAME_REPLY	(DL_RS_BASE + 22)
#define DL_STAT_REPLY	(DL_RS_BASE + 23)

/* Field names for data link layer messages. */
#define DL_PORT		m_data1
#define DL_PROC		m_data2	/* endpoint */
#define DL_COUNT	m_data3
#define DL_MODE		m_data4
#define DL_CLCK		m_data5
#define DL_ADDR		m_data6
#define DL_STAT		m_data4
#define DL_GRANT	m_data5
#define DL_NAME		m_data1

/* Bits in 'DL_STAT' field of DL replies. */
#  define DL_PACK_SEND		0x01
#  define DL_PACK_RECV		0x02
#  define DL_READ_IP		0x04

/* Bits in 'DL_MODE' field of DL requests. */
#  define DL_NOMODE		0x0
#  define DL_PROMISC_REQ	0x2
#  define DL_MULTI_REQ		0x4
#  define DL_BROAD_REQ		0x8

/*===========================================================================*
 *                  SYSTASK request types and field names                    *
 *===========================================================================*/

/* System library calls are dispatched via a call vector, so be careful when 
 * modifying the system call numbers. The numbers here determine which call
 * is made from the call vector.
 */ 
#define KERNEL_CALL	0x600	/* base for kernel calls to SYSTEM */ 

#  define SYS_FORK       (KERNEL_CALL + 0)	/* sys_fork() */
#  define SYS_EXEC       (KERNEL_CALL + 1)	/* sys_exec() */
#  define SYS_EXIT	 (KERNEL_CALL + 2)	/* sys_exit() */
#  define SYS_NICE       (KERNEL_CALL + 3)	/* sys_nice() */
#  define SYS_PRIVCTL    (KERNEL_CALL + 4)	/* sys_privctl() */
#  define SYS_TRACE      (KERNEL_CALL + 5)	/* sys_trace() */
#  define SYS_KILL       (KERNEL_CALL + 6)	/* sys_kill() */

#  define SYS_GETKSIG    (KERNEL_CALL + 7)	/* sys_getsig() */
#  define SYS_ENDKSIG    (KERNEL_CALL + 8)	/* sys_endsig() */
#  define SYS_SIGSEND    (KERNEL_CALL + 9)	/* sys_sigsend() */
#  define SYS_SIGRETURN  (KERNEL_CALL + 10)	/* sys_sigreturn() */

#  define SYS_NEWMAP     (KERNEL_CALL + 11)	/* sys_newmap() */
#  define SYS_SEGCTL     (KERNEL_CALL + 12)	/* sys_segctl() */
#  define SYS_MEMSET     (KERNEL_CALL + 13)	/* sys_memset() */

#  define SYS_UMAP       (KERNEL_CALL + 14)	/* sys_umap() */
#  define SYS_VIRCOPY    (KERNEL_CALL + 15)	/* sys_vircopy() */
#  define SYS_PHYSCOPY   (KERNEL_CALL + 16) 	/* sys_physcopy() */
#  define SYS_VIRVCOPY   (KERNEL_CALL + 17)	/* sys_virvcopy() */
#  define SYS_PHYSVCOPY  (KERNEL_CALL + 18)	/* sys_physvcopy() */

#  define SYS_IRQCTL     (KERNEL_CALL + 19)	/* sys_irqctl() */
#  define SYS_INT86      (KERNEL_CALL + 20)	/* sys_int86() */
#  define SYS_DEVIO      (KERNEL_CALL + 21)	/* sys_devio() */
#  define SYS_SDEVIO     (KERNEL_CALL + 22)	/* sys_sdevio() */
#  define SYS_VDEVIO     (KERNEL_CALL + 23)	/* sys_vdevio() */

#  define SYS_SETALARM	 (KERNEL_CALL + 24)	/* sys_setalarm() */
#  define SYS_TIMES	 (KERNEL_CALL + 25)	/* sys_times() */
#  define SYS_GETINFO    (KERNEL_CALL + 26) 	/* sys_getinfo() */
#  define SYS_ABORT      (KERNEL_CALL + 27)	/* sys_abort() */
#  define SYS_IOPENABLE  (KERNEL_CALL + 28)	/* sys_enable_iop() */
#  define SYS_SAFECOPYFROM (KERNEL_CALL + 31)	/* sys_safecopyfrom() */
#  define SYS_SAFECOPYTO   (KERNEL_CALL + 32)	/* sys_safecopyto() */
#  define SYS_VSAFECOPY  (KERNEL_CALL + 33)	/* sys_vsafecopy() */
#  define SYS_SETGRANT   (KERNEL_CALL + 34)	/* sys_setgrant() */
#  define SYS_READBIOS   (KERNEL_CALL + 35)	/* sys_readbios() */

#  define SYS_SPROF      (KERNEL_CALL + 36)     /* sys_sprof() */ 
#  define SYS_CPROF      (KERNEL_CALL + 37)     /* sys_cprof() */
#  define SYS_PROFBUF    (KERNEL_CALL + 38)     /* sys_profbuf() */

#  define SYS_STIME	(KERNEL_CALL + 39)	/* sys_stime() */

#  define SYS_MAPDMA	(KERNEL_CALL + 42)	/* sys_mapdma() */
#  define SYS_VMCTL	(KERNEL_CALL + 43)	/* sys_vmctl() */
#  define SYS_SYSCTL	(KERNEL_CALL + 44)	/* sys_sysctl() */

#  define SYS_VTIMER     (KERNEL_CALL + 45)	/* sys_vtimer() */
#  define SYS_RUNCTL     (KERNEL_CALL + 46)	/* sys_runctl() */

#  define SYS_STRNLEN	(KERNEL_CALL + 47)	/* sys_strnlen() */

#define NR_SYS_CALLS	48	/* number of system calls */ 

/* Pseudo call for use in kernel/table.c. */
#define SYS_ALL_CALLS (NR_SYS_CALLS)

/* Subfunctions for SYS_PRIVCTL */
#define SYS_PRIV_ALLOW		1	/* Allow process to run */
#define SYS_PRIV_DISALLOW	2	/* Disallow process to run */
#define SYS_PRIV_SET_SYS	3	/* Set a system privilege structure */
#define SYS_PRIV_SET_USER	4	/* Set a user privilege structure */
#define SYS_PRIV_ADD_IO 	5	/* Add I/O range (struct io_range) */
#define SYS_PRIV_ADD_MEM	6	/* Add memory range (struct mem_range)
					 */
#define SYS_PRIV_ADD_IRQ	7	/* Add IRQ */
#define SYS_PRIV_QUERY_MEM	8	/* Verify memory privilege. */

/* Subfunctions for SYS_SETGRANT */
#define SYS_PARAM_SET_GRANT	1	/* Set address and size of grant table */

/* Field names for SYS_MEMSET, SYS_SEGCTL. */
#define MEM_PTR		m_data6	/* base */
#define MEM_COUNT	m_data4	/* count */
#define MEM_PATTERN	m_data5   /* pattern to write */
#define MEM_CHUNK_BASE	m_data1	/* physical base address */
#define MEM_CHUNK_SIZE	m_data2	/* size of mem chunk */
#define MEM_TOT_SIZE	m_data3	/* total memory size */
#define MEM_CHUNK_TAG	m_data4	/* tag to identify chunk of mem */

/* Field names for SYS_STRNLEN */
#define STRNLEN_PROC_E	m_data1	/* endpoint */
#define STRNLEN_STR	m_data4	/* string (user) */
#define STRNLEN_MAXLEN	m_data2   /* maxlen */

/* Field names for SYS_DEVIO, SYS_VDEVIO, SYS_SDEVIO. */
#define DIO_REQUEST	m_data3	/* device in or output */
#   define _DIO_INPUT		0x001
#   define _DIO_OUTPUT		0x002
#   define _DIO_DIRMASK		0x00f
#   define _DIO_BYTE		0x010
#   define _DIO_WORD		0x020
#   define _DIO_LONG		0x030
#   define _DIO_TYPEMASK	0x0f0
#   define _DIO_SAFE		0x100
#   define _DIO_SAFEMASK	0xf00
#   define DIO_INPUT_BYTE	    (_DIO_INPUT|_DIO_BYTE)
#   define DIO_INPUT_WORD	    (_DIO_INPUT|_DIO_WORD)
#   define DIO_OUTPUT_BYTE	    (_DIO_OUTPUT|_DIO_BYTE)
#   define DIO_OUTPUT_WORD	    (_DIO_OUTPUT|_DIO_WORD)
#   define DIO_SAFE_INPUT_BYTE      (_DIO_INPUT|_DIO_BYTE|_DIO_SAFE)
#   define DIO_SAFE_INPUT_WORD      (_DIO_INPUT|_DIO_WORD|_DIO_SAFE)
#   define DIO_SAFE_OUTPUT_BYTE     (_DIO_OUTPUT|_DIO_BYTE|_DIO_SAFE)
#   define DIO_SAFE_OUTPUT_WORD     (_DIO_OUTPUT|_DIO_WORD|_DIO_SAFE)
#define DIO_PORT	m_data4	/* single port address */
#define DIO_VALUE	m_data5	/* single I/O value */
#define DIO_VEC_ADDR	m_data6   /* address of buffer or (p,v)-pairs */
#define DIO_VEC_SIZE	m_data5   /* number of elements in vector */
#define DIO_VEC_ENDPT	m_data2   /* number of process where vector is */
#define DIO_OFFSET	m_data1	/* offset from grant */

/* Field names for SYS_SIGNARLM, SYS_FLAGARLM, SYS_SYNCALRM. */
#define ALRM_EXP_TIME   m_data4	/* expire time for the alarm call */
#define ALRM_ABS_TIME   m_data2	/* set to 1 to use absolute alarm time */
#define ALRM_TIME_LEFT  m_data4	/* how many ticks were remaining */
#define ALRM_ENDPT      m_data1	/* which process wants the alarm? */
#define ALRM_FLAG_PTR	m_data6   /* virtual address of timeout flag */ 	

/* Field names for SYS_IRQCTL. */
#define IRQ_REQUEST     m_data1	/* what to do? */
#  define IRQ_SETPOLICY     1	/* manage a slot of the IRQ table */
#  define IRQ_RMPOLICY      2	/* remove a slot of the IRQ table */
#  define IRQ_ENABLE        3	/* enable interrupts */
#  define IRQ_DISABLE       4	/* disable interrupts */
#define IRQ_VECTOR	m_data2   /* irq vector */
#define IRQ_POLICY	m_data3   /* options for IRQCTL request */
#  define IRQ_REENABLE  0x001	/* reenable IRQ line after interrupt */
#  define IRQ_BYTE      0x100	/* byte values */      
#  define IRQ_WORD      0x200	/* word values */
#  define IRQ_LONG      0x400	/* long values */
#define IRQ_HOOK_ID	m_data7   /* id of irq hook at kernel */

/* Field names for SYS_SEGCTL. */
#define SEG_SELECT	m_data1   /* segment selector returned */ 
#define SEG_OFFSET	m_data2	/* offset in segment returned */
#define SEG_PHYS	m_data3	/* physical address of segment */
#define SEG_SIZE	m_data4	/* segment size */
#define SEG_INDEX	m_data5	/* segment index in remote map */

/* Field names for SYS_VIDCOPY. */
#define VID_REQUEST	m_data1	/* what to do? */
#  define VID_VID_COPY	   1	/* request vid_vid_copy() */
#  define MEM_VID_COPY     2	/* request mem_vid_copy() */
#define VID_SRC_ADDR	m_data2	/* virtual address in memory */
#define VID_SRC_OFFSET	m_data3	/* offset in video memory */
#define VID_DST_OFFSET	m_data4	/* offset in video memory */
#define VID_CP_COUNT	m_data5	/* number of words to be copied */

/* Field names for SYS_ABORT. */
#define ABRT_HOW	m_data1	/* RBT_REBOOT, RBT_HALT, etc. */
#define ABRT_MON_ENDPT  m_data2	/* process where monitor params are */
#define ABRT_MON_LEN	m_data3	/* length of monitor params */
#define ABRT_MON_ADDR   m_data4	/* virtual address of monitor params */

/* Field names for _UMAP, _VIRCOPY, _PHYSCOPY. */
#define CP_SRC_SPACE 	m_data1	/* T or D space (stack is also D) */
#define CP_SRC_ENDPT	m_data3	/* process to copy from */
#define CP_SRC_ADDR	m_data5	/* address where data come from */
#define CP_DST_SPACE	m_data2	/* T or D space (stack is also D) */
#define CP_DST_ENDPT	m_data4	/* process to copy to */
#define CP_DST_ADDR	m_data6	/* address where data go to */
#define CP_NR_BYTES	m_data7	/* number of bytes to copy */

/* Field names for SYS_VCOPY and SYS_VVIRCOPY. */
#define VCP_NR_OK	m_data2	/* number of successfull copies */
#define VCP_VEC_SIZE	m_data3	/* size of copy vector */
#define VCP_VEC_ADDR	m_data4	/* pointer to copy vector */

/* Field names for SYS_GETINFO. */
#define I_REQUEST      m_data3	/* what info to get */
#   define GET_KINFO	   0	/* get kernel information structure */
#   define GET_IMAGE	   1	/* get system image table */
#   define GET_PROCTAB	   2	/* get kernel process table */
#   define GET_RANDOMNESS  3	/* get randomness buffer */
#   define GET_MONPARAMS   4	/* get monitor parameters */
#   define GET_KENV	   5	/* get kernel environment string */
#   define GET_IRQHOOKS	   6	/* get the IRQ table */
#   define GET_KMESSAGES   7	/* get kernel messages */
#   define GET_PRIVTAB	   8	/* get kernel privileges table */
#   define GET_KADDRESSES  9	/* get various kernel addresses */
#   define GET_SCHEDINFO  10	/* get scheduling queues */
#   define GET_PROC 	  11	/* get process slot if given process */
#   define GET_MACHINE 	  12	/* get machine information */
#   define GET_LOCKTIMING 13	/* get lock()/unlock() latency timing */
#   define GET_BIOSBUFFER 14	/* get a buffer for BIOS calls */
#   define GET_LOADINFO   15	/* get load average information */
#   define GET_IRQACTIDS  16	/* get the IRQ masks */
#   define GET_PRIV	  17	/* get privilege structure */
#   define GET_HZ	  18	/* get HZ value */
#   define GET_RANDOMNESS_BIN 19 /* get one randomness bin */
#   define GET_IDLETSC	  20	/* get cumulative idle time stamp counter */
#   define GET_AOUTHEADER 21	/* get a.out headers from the boot image */
#   define GET_BOOTPARAM  22	/* get boot params */
#define I_ENDPT      m_data4	/* calling process */
#define I_VAL_PTR      m_data5	/* virtual address at caller */ 
#define I_VAL_LEN      m_data1	/* max length of value */
#define I_VAL_PTR2     m_data6	/* second virtual address */ 
#define I_VAL_LEN2_E   m_data2	/* second length, or proc nr */

/* Field names for SYS_TIMES. */
#define T_ENDPT      m_data1	/* process to request time info for */
#define T_USER_TIME    m_data1	/* user time consumed by process */
#define T_SYSTEM_TIME  m_data2	/* system time consumed by process */
#define T_BOOTTIME	m_data3	/* Boottime in seconds (also for SYS_STIME) */
#define T_BOOT_TICKS   m_data5	/* number of clock ticks since boot time */

/* vm_map */
#define VM_MAP_ENDPT		m_data1
#define VM_MAP_MAPUNMAP		m_data2
#define VM_MAP_BASE		m_data3
#define VM_MAP_SIZE		m_data4
#define VM_MAP_ADDR		m_data5

/* Field names for SYS_TRACE, SYS_PRIVCTL. */
#define CTL_ENDPT    m_data1	/* process number of the caller */
#define CTL_REQUEST    m_data2	/* server control request */
#define CTL_ARG_PTR    m_data6	/* pointer to argument */
#define CTL_ADDRESS    m_data4	/* address at traced process' space */
#define CTL_DATA       m_data5	/* data field for tracing */

/* SYS_PRIVCTL with CTL_REQUEST == SYS_PRIV_QUERY_MEM */
#define CTL_PHYSSTART  m_data4	/* physical memory start in bytes*/
#define CTL_PHYSLEN    m_data5	/* length in bytes */

/* Field names for SYS_SETGRANT */
#define SG_ADDR		m_data6	/* address */
#define SG_SIZE		m_data2	/* no. of entries */

/* Field names for SYS_KILL, SYS_SIGCTL */
#define SIG_REQUEST    m_data5	/* PM signal control request */
#define S_GETSIG 	   0	/* get pending kernel signal */
#define S_ENDSIG 	   1	/* finish a kernel signal */
#define S_SENDSIG   	   2	/* POSIX style signal handling */
#define S_SIGRETURN	   3 	/* return from POSIX handling */
#define S_KILL		   4 	/* servers kills process with signal */
#define SIG_ENDPT       m_data1	/* process number for inform */
#define SIG_NUMBER     m_data2	/* signal number to send */
#define SIG_FLAGS      m_data3	/* signal flags field */
#define SIG_MAP        m_data4	/* used by kernel to pass signal bit map */
#define SIG_CTXT_PTR   m_data6	/* pointer to info to restore signal context */

/* Field names for SYS_FORK, _EXEC, _EXIT, _NEWMAP. */
#define PR_ENDPT       m_data1	/* indicates a process */
#define PR_PRIORITY    m_data2	/* process priority */
#define PR_SLOT        m_data2	/* indicates a process slot */
#define PR_PID	       m_data3	/* process id at process manager */
#define PR_STACK_PTR   m_data4	/* used for stack ptr in sys_exec, sys_getsp */
#define PR_TRACING     m_data3	/* flag to indicate tracing is on/ off */
#define PR_NAME_PTR    m_data5	/* tells where program name is for dmp */
#define PR_IP_PTR      m_data6	/* initial value for ip after exec */
#define PR_MEM_PTR     m_data4	/* tells where memory map is for sys_newmap
				 * and sys_fork
				 */
#define PR_FORK_FLAGS	m_data3
#define PR_FORK_MSGADDR m_data4

/* Field names for SYS_INT86 */
#define INT86_REG86    m_data4	/* pointer to registers */

/* Flags for PR_FORK_FLAGS. */
#define PFF_VMINHIBIT	0x01	/* Don't schedule until release by VM. */

/* Field names for SYS_SAFECOPY* */
#define SCP_FROM_TO	m_data1	/* from/to whom? */
#define SCP_INFO	m_data2	/* byte: DDDDSSSS Dest and Src seg */
#define SCP_GID		m_data3	/* grant id */
#define SCP_OFFSET	m_data4	/* offset within grant */
#define	SCP_ADDRESS	m_data6	/* my own address */
#define	SCP_BYTES	m_data5	/* bytes from offset */

/* Field names for SYS_VSAFECOPY* */
#define VSCP_VEC_ADDR	m_data6	/* start of vector */
#define VSCP_VEC_SIZE	m_data5	/* elements in vector */

/* For the SCP_INFO field: encoding and decoding. */
#define SCP_MAKEINFO(seg)  ((seg) & 0xffff)
#define SCP_INFO2SEG(info) ((info) & 0xffff)

/* Field names for SELECT (VFS_PROC_NR). */
#define SEL_NFDS       m_data1
#define SEL_READFDS    m_data3
#define SEL_WRITEFDS   m_data4
#define SEL_ERRORFDS   m_data5
#define SEL_TIMEOUT    m_data6

/* Field names for SYS_SPROF, _CPROF, _PROFBUF. */
#define PROF_ACTION    m_data1    /* start/stop/reset/get */
#define PROF_MEM_SIZE  m_data2    /* available memory for data */ 
#define PROF_FREQ      m_data3    /* sample frequency */
#define PROF_ENDPT     m_data4    /* endpoint of caller */
#define PROF_CTL_PTR   m_data5    /* location of info struct */
#define PROF_MEM_PTR   m_data6    /* location of profiling data */

/* Field names for GETSYSINFO_UP (PM). */
#define SIU_WHAT	m_data1
#define SIU_LEN		m_data2
#define SIU_WHERE	m_data6

/* Message for SYS_READBIOS */
#define RDB_SIZE	m_data1
#define RDB_ADDR	m_data4
#define RDB_BUF		m_data6

/* Field names for SYS_VMCTL. */
#define SVMCTL_WHO	m_data1
#define SVMCTL_PARAM	m_data2	/* All SYS_VMCTL requests. */
#define SVMCTL_VALUE	m_data3
#define SVMCTL_PF_WHO		m_data1	/* GET_PAGEFAULT reply: process ep */
#define SVMCTL_PF_I386_CR2	m_data2	/* GET_PAGEFAULT reply: CR2 */
#define SVMCTL_PF_I386_ERR	m_data3	/* GET_PAGEFAULT reply: error code */
#define SVMCTL_MRG_ADDR		m_data4	/* MEMREQ_GET reply: address */
#define SVMCTL_MRG_LEN		m_data1	/* MEMREQ_GET reply: length */
#define SVMCTL_MRG_WRITE	m_data2	/* MEMREQ_GET reply: writeflag */
#define SVMCTL_MRG_EP		m_data3	/* MEMREQ_GET reply: process */
#define SVMCTL_MRG_REQUESTOR	m_data5	/* MEMREQ_GET reply: requestor */
#define SVMCTL_MAP_VIR_ADDR	m_data4

/* Reply message for VMCTL_KERN_PHYSMAP */
#define SVMCTL_MAP_FLAGS	m_data1	/* VMMF_* */
#define SVMCTL_MAP_PHYS_ADDR	m_data4
#define SVMCTL_MAP_PHYS_LEN	m_data5

#define VMMF_UNCACHED		(1L << 0)

/* Codes and field names for SYS_SYSCTL. */
#define SYSCTL_CODE		m_data1	/* SYSCTL_CODE_* below */
#define SYSCTL_ARG1		m_data4
#define SYSCTL_ARG2		m_data2
#define SYSCTL_CODE_DIAG	1	/* Print diagnostics. */
#define SYSCTL_CODE_STACKTRACE	2	/* Print process stack. */
#define DIAG_BUFSIZE	(80*25)

/* Values for SVMCTL_PARAM. */
#define VMCTL_I386_SETCR3	10
#define VMCTL_GET_PAGEFAULT	11
#define VMCTL_CLEAR_PAGEFAULT	12
#define VMCTL_I386_GETCR3	13
#define VMCTL_MEMREQ_GET	14
#define VMCTL_MEMREQ_REPLY	15
#define VMCTL_INCSP		16
#define VMCTL_NOPAGEZERO	18
#define VMCTL_I386_KERNELLIMIT	19
#define VMCTL_I386_PAGEDIRS	20
#define VMCTL_I386_FREEPDE	23
#define VMCTL_ENABLE_PAGING	24
#define VMCTL_I386_INVLPG	25
#define VMCTL_FLUSHTLB		26
#define VMCTL_KERN_PHYSMAP	27
#define VMCTL_KERN_MAP_REPLY	28

/* Field names for SYS_VTIMER. */
#define VT_WHICH	m_data1	/* which timer to set/retrieve */
#  define VT_VIRTUAL        1	/* the ITIMER_VIRTUAL timer */
#  define VT_PROF           2	/* the ITIMER_PROF timer */
#define VT_SET		m_data2	/* 1 for setting a timer, 0 retrieval only */
#define VT_VALUE	m_data4	/* new/previous value of the timer */
#define VT_ENDPT	m_data5	/* process to set/retrieve the timer for */

/* Field names for SYS_RUNCTL. */
#define RC_ENDPT	m_data1	/* which process to stop or resume */
#define RC_ACTION	m_data2	/* set or clear stop flag */
#  define RC_STOP           0	/* stop the process */
#  define RC_RESUME         1	/* clear the stop flag */
#define RC_FLAGS	m_data3	/* request flags */
#  define RC_DELAY          1	/* delay stop if process is sending */

/*===========================================================================*
 *                Messages for the Reincarnation Server 		     *
 *===========================================================================*/

#define RS_RQ_BASE		0x700

#define RS_UP		(RS_RQ_BASE + 0)	/* start system service */
#define RS_DOWN		(RS_RQ_BASE + 1)	/* stop system service */
#define RS_REFRESH	(RS_RQ_BASE + 2)	/* refresh system service */
#define RS_RESTART	(RS_RQ_BASE + 3)	/* restart system service */
#define RS_SHUTDOWN	(RS_RQ_BASE + 4)	/* alert about shutdown */

#define RS_LOOKUP	(RS_RQ_BASE + 8)	/* lookup server name */

#  define RS_CMD_ADDR		m_data4		/* command string */
#  define RS_CMD_LEN		m_data1		/* length of command */
#  define RS_PERIOD 	        m_data2		/* heartbeat period */
#  define RS_DEV_MAJOR          m_data3           /* major device number */

#  define RS_ENDPOINT		m_data1		/* endpoint number in reply */

#  define RS_NAME		m_data4		/* name */
#  define RS_NAME_LEN		m_data1		/* namelen */

/*===========================================================================*
 *                Messages for the Data Store Server			     *
 *===========================================================================*/

#define DS_RQ_BASE		0x800

#define DS_PUBLISH	(DS_RQ_BASE + 0)	/* publish information */
#define DS_SUBSCRIBE	(DS_RQ_BASE + 1)	/* subscribe to information */
#define DS_RETRIEVE	(DS_RQ_BASE + 2)	/* retrieve information by name */
#define DS_CHECK	(DS_RQ_BASE + 3)	/* retrieve updated information */

/* DS field names: DS_SUBSCRIBE, DS_PUBLISH, DS_RETRIEVE */
#  define DS_KEY_GRANT		m_data6		/* key for the information */
#  define DS_KEY_LEN		m_data1		/* length of key incl. '\0' */
#  define DS_FLAGS		m_data2		/* flags provided by caller */

/* DS_PUBLISH, DS_RETRIEVE */
#  define DS_VAL		m_data4		/* data (u32, char *, etc.) */
#  define DS_VAL_LEN		m_data5		/* data length */

/*===========================================================================*
 *                Miscellaneous messages used by TTY			     *
 *===========================================================================*/

/* Miscellaneous request types and field names, e.g. used by IS server. */
#define FKEY_CONTROL 		98  	/* control a function key at the TTY */
#  define FKEY_REQUEST	     m_data1	/* request to perform at TTY */
#  define    FKEY_MAP		10	/* observe function key */
#  define    FKEY_UNMAP		11	/* stop observing function key */
#  define    FKEY_EVENTS	12	/* request open key presses */
#  define FKEY_FKEYS	      m_data4	/* F1-F12 keys pressed */
#  define FKEY_SFKEYS	      m_data5	/* Shift-F1-F12 keys pressed */
#define DIAG_BASE	0xa00
#define DIAGNOSTICS_OLD 	(DIAG_BASE+1) 	/* output a string without FS in between */
#define DIAGNOSTICS_S_OLD 	(DIAG_BASE+2) 	/* grant-based version of DIAGNOSTICS */
#  define DIAG_PRINT_BUF_G    m_data4
#  define DIAG_BUF_COUNT      m_data1
#define GET_KMESS	(DIAG_BASE+3)	/* get kmess from TTY */
#  define GETKM_PTR	      m_data4
#define GET_KMESS_S	(DIAG_BASE+4)	/* get kmess from TTY */
#  define GETKM_GRANT	      m_data1
#define ASYN_DIAGNOSTICS_OLD (DIAG_BASE+5) 	/* grant-based, replyless DIAGNOSTICS */

#define DIAG_REPL_OLD 	(DIAG_BASE+0x80+0) 	/* reply to DIAGNOSTICS(_S) */

/*===========================================================================*
 *                Messages used between PM and VFS			     *
 *===========================================================================*/

#define PM_RQ_BASE	0x900
#define PM_RS_BASE	0x980

/* Requests from PM to VFS */
#define PM_SETUID	(PM_RQ_BASE + 1)	/* Set new user ID */
#define PM_SETGID	(PM_RQ_BASE + 2)	/* Set group ID */
#define PM_SETSID	(PM_RQ_BASE + 3)	/* Set session leader */
#define PM_EXIT		(PM_RQ_BASE + 4)	/* Process exits */
#define PM_DUMPCORE	(PM_RQ_BASE + 5)	/* Process is to dump core */
#define PM_EXEC		(PM_RQ_BASE + 6)	/* Forwarded exec call */
#define PM_FORK		(PM_RQ_BASE + 7)	/* Newly forked process */
#define PM_FORK_NB	(PM_RQ_BASE + 8)	/* Non-blocking fork */
#define PM_UNPAUSE	(PM_RQ_BASE + 9)	/* Interrupt process call */
#define PM_REBOOT	(PM_RQ_BASE + 10)	/* System reboot */
#define PM_SETGROUPS	(PM_RQ_BASE + 11)	/* Tell VFS about setgroups */

/* Replies from VFS to PM */
#define PM_SETUID_REPLY	(PM_RS_BASE + 21)
#define PM_SETGID_REPLY	(PM_RS_BASE + 22)
#define PM_SETSID_REPLY	(PM_RS_BASE + 23)
#define PM_EXIT_REPLY	(PM_RS_BASE + 24)
#define PM_CORE_REPLY	(PM_RS_BASE + 25)
#define PM_EXEC_REPLY	(PM_RS_BASE + 26)
#define PM_FORK_REPLY	(PM_RS_BASE + 27)
#define PM_FORK_NB_REPLY	(PM_RS_BASE + 28)
#define PM_UNPAUSE_REPLY	(PM_RS_BASE + 29)
#define PM_REBOOT_REPLY	(PM_RS_BASE + 30)
#define PM_SETGROUPS_REPLY	(PM_RS_BASE + 31)

/* Standard parameters for all requests and replies, except PM_REBOOT */
#  define PM_PROC		m_data1	/* process */

/* Additional parameters for PM_SETUID and PM_SETGID */
#  define PM_EID		m_data2	/* effective user/group id */
#  define PM_RID		m_data3	/* real user/group id */

/* Additional parameter for PM_SETGROUPS */
#define PM_GROUP_NO		m_data2	/* number of groups */
#define PM_GROUP_ADDR		m_data4	/* struct holding group data */


/* Additional parameters for PM_EXEC */
#  define PM_PATH		m_data4	/* executable */
#  define PM_PATH_LEN		m_data2	/* length of path including
					 * terminating nul
					 */
#  define PM_FRAME		m_data5	/* arguments and environment */
#  define PM_FRAME_LEN		m_data3	/* size of frame */

/* Additional parameters for PM_EXEC_REPLY and PM_CORE_REPLY */
#  define PM_STATUS		m_data2	/* OK or failure */

/* Additional parameters for PM_FORK and PM_FORK_NB */
#  define PM_PPROC		m_data2	/* parent process */
#  define PM_CPID		m_data3	/* child pid */

/* Parameters for the EXEC_NEWMEM call */
#define EXC_NM_PROC	m_data1		/* process that needs new map */
#define EXC_NM_PTR	m_data4		/* parameters in struct exec_newmem */
/* Results:
 * the status will be in m_type.
 * the top of the stack will be in m_data1.
 * the following flags will be in m_data2:
 */
#define EXC_NM_RF_LOAD_TEXT	1	/* Load text segment (otherwise the
					 * text segment is already present)
					 */
#define EXC_NM_RF_ALLOW_SETUID	2	/* Setuid execution is allowed (tells
					 * FS to update its uid and gid 
					 * fields.
					 */
#define EXC_NM_RF_FULLVM	4	

/* Parameters for the EXEC_RESTART call */
#define EXC_RS_PROC	m_data1		/* process that needs to be restarted */
#define EXC_RS_RESULT	m_data2		/* result of the exec */

/*===========================================================================*
 *                Messages used from VFS to file servers		     *
 *===========================================================================*/

#define VFS_BASE	0xA00		/* Requests sent by VFS to filesystem
					 * implementations. See <nucleos/vfsif.h>
					 */

/*===========================================================================*
 *                Messages used from VM to VFS				     *
 *===========================================================================*/

/* Requests sent by VM to VFS, done on behalf of a user process. */
#define VM_VFS_BASE	0xB00		
#define VM_VFS_OPEN	(VM_VFS_BASE+0) /* open() on behalf of user process. */
#	define VMVO_NAME_GRANT		m_data1	/* 0-terminated */
#	define VMVO_NAME_LENGTH		m_data2	/* name length including 0 */
#	define VMVO_FLAGS		m_data3
#	define VMVO_MODE		m_data4
#	define VMVO_ENDPOINT		m_data5
#define VM_VFS_MMAP	(VM_VFS_BASE+1) /* mmap() */
#define VM_VFS_CLOSE	(VM_VFS_BASE+2) /* close() */
#	define VMVC_FD			m_data1
#	define VMVC_ENDPOINT		m_data2

/* PM field names */
/* BRK */
#define PMBRK_ADDR				m_data4

/* TRACE */
#define PMTRACE_ADDR				m_data4

#define PM_ENDPT				m_data1
#define PM_PENDPT				m_data2

/*===========================================================================*
 *                Messages for VM server				     *
 *===========================================================================*/
#define VM_RQ_BASE		0xC00

/* Calls from PM */
#define VM_EXIT			(VM_RQ_BASE+0)
#	define VME_ENDPOINT		m_data1
#define VM_FORK			(VM_RQ_BASE+1)
#	define VMF_ENDPOINT		m_data1
#	define VMF_SLOTNO		m_data2
#	define VMF_CHILD_ENDPOINT	m_data3	/* result */
#define VM_BRK			(VM_RQ_BASE+2)
#	define VMB_ENDPOINT		m_data1
#	define VMB_ADDR			m_data4
#	define VMB_RETADDR		m_data5	/* result */
#define VM_EXEC_NEWMEM		(VM_RQ_BASE+3)
#	define VMEN_ENDPOINT		m_data1
#	define VMEN_ARGSPTR		m_data4
#	define VMEN_ARGSSIZE		m_data2
#	define VMEN_FLAGS		m_data3	/* result */
#	define VMEN_STACK_TOP		m_data5	/* result */
#define VM_PUSH_SIG		(VM_RQ_BASE+4)
#	define VMPS_ENDPOINT		m_data1
#	define VMPS_OLD_SP		m_data4	/* result */
#define VM_WILLEXIT		(VM_RQ_BASE+5)
#	define VMWE_ENDPOINT		m_data1

/* General calls. */
#define VM_MMAP			(VM_RQ_BASE+10)
#	define VMM_ADDR			m_data5
#	define VMM_LEN			m_data6
#	define VMM_PROT			m_data1
#	define VMM_FLAGS		m_data2
#	define VMM_FD			m_data3
#	define VMM_OFFSET		m_data4
#	define VMM_RETADDR		m_data5	/* result */
#define VM_UMAP			(VM_RQ_BASE+11)
#	define VMU_SEG			m_data1
#	define VMU_OFFSET		m_data4
#	define VMU_LENGTH		m_data5
#	define VMU_RETADDR		m_data6

/* to VM: inform VM about a region of memory that is used for
 * bus-master DMA
 */
#define VM_ADDDMA	(VM_RQ_BASE+12)
#	define VMAD_REQ			m_data2
#	define VMAD_EP			m_data1
#	define VMAD_START		m_data4
#	define VMAD_SIZE		m_data5

/* to VM: inform VM that a region of memory that is no longer
 * used for bus-master DMA
 */
#define VM_DELDMA       (VM_RQ_BASE+13)
#	define VMDD_REQ			m_data2
#	define VMDD_EP			m_data1
#	define VMDD_START		m_data4
#	define VMDD_SIZE		m_data5

/* to VM: ask VM for a region of memory that should not
 * be used for bus-master DMA any longer
 */
#define VM_GETDMA       (VM_RQ_BASE+14)
#	define VMGD_REQ			m_data2
#	define VMGD_PROCP		m_data1
#	define VMGD_BASEP		m_data4
#	define VMGD_SIZEP		m_data5

#define VM_MAP_PHYS		(VM_RQ_BASE+15)
#	define VMMP_EP			m_data1
#	define VMMP_PHADDR		m_data5
#	define VMMP_LEN			m_data2
#	define VMMP_VADDR_REPLY		m_data6

#define VM_UNMAP_PHYS		(VM_RQ_BASE+16)
#	define VMUP_EP			m_data1
#	define VMUP_VADDR		m_data4

#define VM_MUNMAP		(VM_RQ_BASE+17)
#	define VMUM_ADDR		m_data4
#	define VMUM_LEN			m_data1

#define VM_ALLOCMEM		(VM_RQ_BASE+18)
#	define VMAM_BYTES		m_data4
#	define VMAM_MEMBASE		m_data1

#define VM_MUNMAP_TEXT		(VM_RQ_BASE+19)

/* Calls from VFS. */
#	define VMV_ENDPOINT		m_data1	/* for all VM_VFS_REPLY_* */
#define VM_VFS_REPLY_OPEN	(VM_RQ_BASE+30)
#	define VMVRO_FD			m_data2
#define VM_VFS_REPLY_MMAP	(VM_RQ_BASE+31)
#define VM_VFS_REPLY_CLOSE	(VM_RQ_BASE+32)

#define VM_REMAP		(VM_RQ_BASE+33)
#	define VMRE_D			m_data1
#	define VMRE_S			m_data2
#	define VMRE_DA			m_data4
#	define VMRE_SA			m_data5
#	define VMRE_RETA		m_data6
#	define VMRE_SIZE		m_data3

#define VM_SHM_UNMAP		(VM_RQ_BASE+34)
#	define VMUN_ENDPT		m_data1
#	define VMUN_ADDR		m_data4

#define VM_GETPHYS		(VM_RQ_BASE+35)
#	define VMPHYS_ENDPT		m_data1
#	define VMPHYS_ADDR		m_data4
#	define VMPHYS_RETA		m_data5

#define VM_GETREF		(VM_RQ_BASE+36)
#	define VMREFCNT_ENDPT		m_data1
#	define VMREFCNT_ADDR		m_data4
#	define VMREFCNT_RETC		m_data2

#define VM_RS_SET_PRIV		(VM_RQ_BASE+37)
#	define VM_RS_NR			m_data1
#	define VM_RS_BUF		m_data4

#define VM_QUERY_EXIT		(VM_RQ_BASE+38)
#	define VM_QUERY_RET_PT	m_data1
#	define VM_QUERY_IS_MORE	m_data2

#define VM_NOTIFY_SIG		(VM_RQ_BASE+39)
#	define VM_NOTIFY_SIG_ENDPOINT	m_data1
#	define VM_NOTIFY_SIG_IPC	m_data2

#define VM_CTL			(VM_RQ_BASE+40)
#define VCTL_WHAT			m_data1
#define VCTL_PARAM			m_data2

/* General calls. */
#define NNR_VM_MMAP		(VM_RQ_BASE+41)
#	define VMM_ADDR			m_data5
#	define VMM_LEN			m_data6
#	define VMM_PROT			m_data1
#	define VMM_FLAGS		m_data2
#	define VMM_FD			m_data3
#	define VMM_OFFSET		m_data4
#	define VMM_RETADDR		m_data5	/* result */

#define NNR_VM_MUNMAP		(VM_RQ_BASE+42)
#	define VMUM_ADDR		m_data4
#	define VMUM_LEN			m_data1

#define NNR_VM_MUNMAP_TEXT	(VM_RQ_BASE+43)

/* VCTL_PARAMs */
#define VCTLP_STATS_MEM			1
#define VCTLP_STATS_EP			2

/* Total. */
#define VM_NCALLS			44

/*===========================================================================*
 *                Messages for IPC server				     *
 *===========================================================================*/
#define IPC_BASE	0xD00

/* Shared Memory */
#define IPC_SHMGET	(IPC_BASE+1)
#	define SHMGET_KEY	m_data4
#	define SHMGET_SIZE	m_data5
#	define SHMGET_FLAG	m_data1
#	define SHMGET_RETID	m_data2
#define IPC_SHMAT	(IPC_BASE+2)
#	define SHMAT_ID		m_data1
#	define SHMAT_ADDR	m_data4
#	define SHMAT_FLAG	m_data2
#	define SHMAT_RETADDR	m_data5
#define IPC_SHMDT	(IPC_BASE+3)
#	define SHMDT_ADDR	m_data4
#define IPC_SHMCTL	(IPC_BASE+4)
#	define SHMCTL_ID	m_data1
#	define SHMCTL_CMD	m_data2
#	define SHMCTL_BUF	m_data4
#	define SHMCTL_RET	m_data3

/* Semaphore */
#define IPC_SEMGET	(IPC_BASE+5)
#	define SEMGET_KEY	m_data4
#	define SEMGET_NR	m_data1
#	define SEMGET_FLAG	m_data2
#	define SEMGET_RETID	m_data3
#define IPC_SEMCTL	(IPC_BASE+6)
#	define SEMCTL_ID	m_data1
#	define SEMCTL_NUM	m_data2
#	define SEMCTL_CMD	m_data3
#	define SEMCTL_OPT	m_data4
#define IPC_SEMOP	(IPC_BASE+7)
#	define SEMOP_ID		m_data1
#	define SEMOP_OPS	m_data4
#	define SEMOP_SIZE	m_data2

#endif /* __ASSEMBLY__ */

#endif /* __NUCLEOS_COM_H */ 
