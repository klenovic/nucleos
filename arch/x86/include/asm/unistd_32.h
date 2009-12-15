/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_UNISTD_32_H
#define __ASM_X86_UNISTD_32_H

#define __NR_exit		1
#define __NR_fork		2
#define __NR_read		3
#define __NR_write		4
#define __NR_open		5
#define __NR_close		6
#define __NR_wait		7
#define __NR_creat		8
#define __NR_link		9
#define __NR_unlink		10
#define __NR_waitpid		11
#define __NR_chdir		12
#define __NR_time		13
#define __NR_mknod		14
#define __NR_chmod		15
#define __NR_chown		16
#define __NR_brk		17
#define __NR_stat		18
#define __NR_lseek		19
#define __NR_getpid		20
#define __NR_mount		21
#define __NR_umount		22
#define __NR_setuid		23
#define __NR_getuid		24
#define __NR_stime		25
#define __NR_ptrace		26
#define __NR_alarm		27
#define __NR_fstat		28
#define __NR_pause		29
#define __NR_utime		30
#define __NR_access		33
#define __NR_sync		36
#define __NR_kill		37
#define __NR_rename		38
#define __NR_mkdir		39
#define __NR_rmdir		40
#define __NR_dup		41
#define __NR_pipe		42
#define __NR_times		43
#define __NR_symlink		45
#define __NR_setgid		46
#define __NR_getgid		47
#define __NR_signal		48
#define __NR_readlink		49
#define __NR_lstat		50
#define __NR_ioctl		54
#define __NR_fcntl		55
#define __NR_exec		59
#define __NR_umask		60
#define __NR_chroot		61
#define __NR_setsid		62
#define __NR_getpgrp		63
#define __NR_itimer		64
/* Posix signal handling. */
#define __NR_sigaction		71
#define __NR_sigsuspend		72
#define __NR_sigpending		73
#define __NR_sigprocmask	74
#define __NR_sigreturn		75
#define __NR_reboot		76
#define __NR_svrctl		77
#define __NR_sysuname		78
#define __NR_getsysinfo		79	/* to PM or FS */
#define __NR_getdents		80	/* to FS */
#define __NR_llseek		81	/* to FS */
#define __NR_fstatfs		82	/* to FS */
#define __NR_select		85	/* to FS */
#define __NR_fchdir		86	/* to FS */
#define __NR_fsync		87	/* to FS */
#define __NR_getpriority	88	/* to PM */
#define __NR_setpriority	89	/* to PM */
#define __NR_gettimeofday	90	/* to PM */
#define __NR_seteuid		91	/* to PM */
#define __NR_setegid		92	/* to PM */
#define __NR_truncate		93	/* to FS */
#define __NR_ftruncate		94	/* to FS */
#define __NR_fchmod		95	/* to FS */
#define __NR_fchown		96	/* to FS */
#define __NR_getsysinfo_up	97	/* to PM or FS */
#define __NR_sprof		98	/* to PM */
#define __NR_cprof		99	/* to PM */

#if defined(__KERNEL__) || defined(__UKERNEL__)

#include <nucleos/types.h>
#include <nucleos/type.h>
#include <nucleos/signal.h>

#define __syscall_offset	130
#define NR_syscalls		256	/* number of system calls allowed */

#define __NNR_exit		(1 + __syscall_offset)
#define __NNR_fork		(2 + __syscall_offset)
#define __NNR_read		(3 + __syscall_offset)
#define __NNR_write		(4 + __syscall_offset)
#define __NNR_open		(5 + __syscall_offset)
#define __NNR_close		(6 + __syscall_offset)
#define __NNR_wait		(7 + __syscall_offset)
#define __NNR_creat		(8 + __syscall_offset)
#define __NNR_link		(9 + __syscall_offset)
#define __NNR_unlink		(10 + __syscall_offset)

#define __NNR_waitpid		(11 + __syscall_offset)
#define __NNR_chdir		(12 + __syscall_offset)
#define __NNR_time		(13 + __syscall_offset)
#define __NNR_mknod		(14 + __syscall_offset)
#define __NNR_chmod		(15 + __syscall_offset)
#define __NNR_chown		(16 + __syscall_offset)
#define __NNR_brk		(17 + __syscall_offset)
#define __NNR_stat		(18 + __syscall_offset)
#define __NNR_lseek		(19 + __syscall_offset)
#define __NNR_getpid		(20 + __syscall_offset)

#define __NNR_mount		(21 + __syscall_offset)
#define __NNR_umount		(22 + __syscall_offset)
#define __NNR_setuid		(23 + __syscall_offset)
#define __NNR_getuid		(24 + __syscall_offset)
#define __NNR_stime		(25 + __syscall_offset)
#define __NNR_ptrace		(26 + __syscall_offset)
#define __NNR_alarm		(27 + __syscall_offset)
#define __NNR_fstat		(28 + __syscall_offset)
#define __NNR_pause		(29 + __syscall_offset)
#define __NNR_utime		(30 + __syscall_offset)

#define __NNR_access		(31 + __syscall_offset)
#define __NNR_sync		(32 + __syscall_offset)
#define __NNR_kill		(33 + __syscall_offset)
#define __NNR_rename		(34 + __syscall_offset)
#define __NNR_mkdir		(35 + __syscall_offset)
#define __NNR_rmdir		(36 + __syscall_offset)
#define __NNR_dup		(37 + __syscall_offset)
#define __NNR_pipe		(38 + __syscall_offset)
#define __NNR_times		(39 + __syscall_offset)
#define __NNR_symlink		(40 + __syscall_offset)

#define __NNR_setgid		(41 + __syscall_offset)
#define __NNR_getgid		(42 + __syscall_offset)
#define __NNR_signal		(43 + __syscall_offset)
#define __NNR_readlink		(44 + __syscall_offset)
#define __NNR_lstat		(45 + __syscall_offset)
#define __NNR_ioctl		(46 + __syscall_offset)
#define __NNR_fcntl		(47 + __syscall_offset)
#define __NNR_exec		(48 + __syscall_offset)
#define __NNR_umask		(49 + __syscall_offset)
#define __NNR_chroot		(50 + __syscall_offset)

#define __NNR_setsid		(51 + __syscall_offset)
#define __NNR_getpgrp		(52 + __syscall_offset)
#define __NNR_getitimer		(53 + __syscall_offset)
#define __NNR_setitimer		(54 + __syscall_offset)
/* Posix signal handling. */
#define __NNR_sigaction		(55 + __syscall_offset)
#define __NNR_sigsuspend	(56 + __syscall_offset)
#define __NNR_sigpending	(57 + __syscall_offset)
#define __NNR_sigprocmask	(58 + __syscall_offset)
#define __NNR_sigreturn		(59 + __syscall_offset)
#define __NNR_reboot		(60 + __syscall_offset)

#define __NNR_getdents		(61 + __syscall_offset)
#define __NNR_llseek		(62 + __syscall_offset)
#define __NNR_fstatfs		(63 + __syscall_offset)
#define __NNR_select		(64 + __syscall_offset)
#define __NNR_fchdir		(65 + __syscall_offset)
#define __NNR_fsync		(66 + __syscall_offset)
#define __NNR_getpriority	(67 + __syscall_offset)
#define __NNR_setpriority	(68 + __syscall_offset)
#define __NNR_gettimeofday	(69 + __syscall_offset)
#define __NNR_seteuid		(70 + __syscall_offset)

#define __NNR_setegid		(71 + __syscall_offset)
#define __NNR_truncate		(72 + __syscall_offset)
#define __NNR_ftruncate		(73 + __syscall_offset)
#define __NNR_fchmod		(74 + __syscall_offset)
#define __NNR_fchown		(75 + __syscall_offset)
#define __NNR_sprof		(76 + __syscall_offset)
#define __NNR_cprof		(77 + __syscall_offset)
#define __NNR_getegid		(78 + __syscall_offset)
#define __NNR_getppid		(79 + __syscall_offset)
#define __NNR_dup2		(80 + __syscall_offset)

#define __NNR_uname		(81 + __syscall_offset)
#define __NNR_mmap		(82 + __syscall_offset)
#define __NNR_munmap		(83 + __syscall_offset)
#define __NNR_munmap_text	(84 + __syscall_offset)

/* Kernel message numbers. These are used only by u/kernel and
 * never by C library.
 */
#define KCNR_FS_READY		57
#define KCNR_EXEC_NEWMEM	100	/* from FS or RS to PM: new memory map for
					 * exec
					 */
#define KCNR_FORK_NB		101	/* to PM: special fork call for RS */
#define KCNR_EXEC_RESTART	102	/* to PM: final part of exec for RS */
#define KCNR_PROCSTAT		103	/* to PM */
#define KCNR_GETPROCNR		104	/* to PM */
#define KCNR_ALLOCMEM		105	/* to PM */
#define KCNR_GETEPINFO		107	/* to PM: get pid/gid/uid of an endpoint */
#define KCNR_ADDDMA		108	/* to PM: inform PM about a region of memory
					 * that is used for bus-master DMA
					 */
#define KCNR_DELDMA		109	/* to PM: inform PM that a region of memory
					 * that is no longer used for bus-master DMA
					 */
#define KCNR_GETDMA		110	/* to PM: ask PM for a region of memory
					 * that should not be used for bus-master DMA
					 * any longer
					 */
#define KCNR_DEVCTL		120	/* to FS, map or unmap a device */
#define KCNR_TASK_REPLY		121	/* to FS: reply code from drivers, not
					 * really a standalone call.
					 */
#define KCNR_MAPDRIVER		122	/* to FS, map a device */

/* Values used by access().  POSIX Table 2-8. */
#define F_OK               0	/* test if file exists */
#define X_OK               1	/* test if file is executable */
#define W_OK               2	/* test if file is writable */
#define R_OK               4	/* test if file is readable */

/* Values used for whence in lseek(fd, offset, whence).  POSIX Table 2-9. */
#define SEEK_SET           0	/* offset is absolute  */
#define SEEK_CUR           1	/* offset is relative to current position */
#define SEEK_END           2	/* offset is relative to end of file */

/* This value is required by POSIX Table 2-10. */
#define _POSIX_VERSION 199009L	/* which standard is being conformed to */

/* These three definitions are required by POSIX Sec. 8.2.1.2. */
#define STDIN_FILENO       0	/* file descriptor for stdin */
#define STDOUT_FILENO      1	/* file descriptor for stdout */
#define STDERR_FILENO      2	/* file descriptor for stderr */

/* How to exit the system or stop a server process. */
#define RBT_HALT	   0	/* shutdown and return to monitor */
#define RBT_REBOOT	   1	/* reboot the system through the monitor */
#define RBT_PANIC	   2	/* a server panics */
#define RBT_MONITOR	   3	/* let the monitor do this */
#define RBT_RESET	   4	/* hard reset the system */
#define RBT_INVALID	   5	/* first invalid reboot flag */
#define _PM_SEG_FLAG (1L << 30)	/* for read() and write() to FS_PROC_NR by PM */

/* What system info to retrieve with sysgetinfo(). */
#define SI_KINFO	   0	/* get kernel info via PM */
#define SI_PROC_ADDR	   1	/* address of process table */
#define SI_PROC_TAB	   2	/* copy of entire process table */
#define SI_DMAP_TAB	   3	/* get device <-> driver mappings */
#define SI_MEM_ALLOC	   4	/* get memory allocation data */
#define SI_DATA_STORE	   5	/* get copy of data store */
#define SI_LOADINFO	   6	/* get copy of load average structure */
#define SI_KPROC_TAB	   7	/* copy of kernel process table */
#define SI_CALL_STATS	   8	/* system call statistics */
#define SI_PCI_INFO	   9	/* get kernel info via PM */

/* NULL must be defined in <nucleos/unistd.h> according to POSIX Sec. 2.7.1. */
#define NULL    ((void *)0)

/* The following relate to configurable system variables. POSIX Table 4-2. */
#define _SC_ARG_MAX	   1
#define _SC_CHILD_MAX	   2
#define _SC_CLOCKS_PER_SEC 3
#define _SC_CLK_TCK	   3
#define _SC_NGROUPS_MAX	   4
#define _SC_OPEN_MAX	   5
#define _SC_JOB_CONTROL	   6
#define _SC_SAVED_IDS	   7
#define _SC_VERSION	   8
#define _SC_STREAM_MAX	   9
#define _SC_TZNAME_MAX    10
#define _SC_PAGESIZE	  11
#define _SC_PAGE_SIZE	  _SC_PAGESIZE

/* The following relate to configurable pathname variables. POSIX Table 5-2. */
#define _PC_LINK_MAX	   1	/* link count */
#define _PC_MAX_CANON	   2	/* size of the canonical input queue */
#define _PC_MAX_INPUT	   3	/* type-ahead buffer size */
#define _PC_NAME_MAX	   4	/* file name size */
#define _PC_PATH_MAX	   5	/* pathname size */
#define _PC_PIPE_BUF	   6	/* pipe size */
#define _PC_NO_TRUNC	   7	/* treatment of long name components */
#define _PC_VDISABLE	   8	/* tty disable */
#define _PC_CHOWN_RESTRICTED 9	/* chown restricted or not */

/* POSIX defines several options that may be implemented or not, at the
 * implementer's whim.  This implementer has made the following choices:
 *
 * _POSIX_JOB_CONTROL	    not defined:	no job control
 * _POSIX_SAVED_IDS 	    not defined:	no saved uid/gid
 * _POSIX_NO_TRUNC	    defined as -1:	long path names are truncated
 * _POSIX_CHOWN_RESTRICTED  defined:		you can't give away files
 * _POSIX_VDISABLE	    defined:		tty functions can be disabled
 */
#define _POSIX_NO_TRUNC       (-1)
#define _POSIX_CHOWN_RESTRICTED  1

/* Function Prototypes. */
void _exit(int _status);
int access(const char *_path, int _amode);
unsigned int alarm(unsigned int _seconds);
int chdir(const char *_path);
int fchdir(int fd);
int chown(const char *_path, uid_t _owner, gid_t _group);
int fchown(int fd, uid_t _owner, gid_t _group);
int close(int _fd);
char *ctermid(char *_s);
char *cuserid(char *_s);
int dup(int _fd);
int dup2(int _fd, int _fd2);
int execl(const char *_path, const char *_arg, ...);
int execle(const char *_path, const char *_arg, ...);
int execlp(const char *_file, const char *arg, ...);
int execv(const char *_path, char *const _argv[]);
int execve(const char *_path, char *const _argv[], char *const _envp[]);
int execvp(const char *_file, char *const _argv[]);
pid_t fork(void);
long fpathconf(int _fd, int _name);
char *getcwd(char *_buf, size_t _size);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
int getgroups(int _gidsetsize, gid_t _grouplist[]);
char *getlogin(void);
pid_t getpgrp(void);
pid_t getpid(void);

pid_t getppid(void);
uid_t getuid(void);
int isatty(int _fd);
int link(const char *_existing, const char *_new);
off_t lseek(int _fd, off_t _offset, int _whence);
long pathconf(const char *_path, int _name);
int pause(void);
int pipe(int _fildes[2]);
ssize_t read(int _fd, void *_buf, size_t _n);
int rmdir(const char *_path);
int setgid(gid_t _gid);
int setegid(gid_t _gid);
int setpgid(pid_t _pid, pid_t _pgid);
pid_t setsid(void);
int setuid(uid_t _uid);
int seteuid(uid_t _uid);
unsigned int sleep(unsigned int _seconds);
long sysconf(int _name);
pid_t tcgetpgrp(int _fd);
int tcsetpgrp(int _fd, pid_t _pgrp_id);
char *ttyname(int _fd);
int unlink(const char *_path);
ssize_t write(int _fd, const void *_buf, size_t _n);
int truncate(const char *_path, off_t _length);
int ftruncate(int _fd, off_t _length);
int nice(int _incr);

/* Open Group Base Specifications Issue 6 (not complete) */
int symlink(const char *path1, const char *path2);
int readlink(const char *, char *, size_t);
int getopt(int _argc, char * const _argv[], char const *_opts);

extern char *optarg;
extern int optind, opterr, optopt;

int usleep(useconds_t _useconds);

extern int optreset;	/* Reset getopt state */

int brk(void *addr);
int chroot(const char *_name);
loff_t lseek64(int fd, loff_t offset, int whence);
int mknod(const char *_name, mode_t _mode, dev_t _addr);
int mknod4(const char *_name, mode_t _mode, dev_t _addr, long _size);
char *mktemp(char *_template);
long ptrace(int _req, pid_t _pid, long _addr, long _data);
char *sbrk(int _incr);
int sync(void);
int fsync(int fd);
int reboot(int _how, ...);
int gethostname(char *_hostname, size_t _len);
int getdomainname(char *_domain, size_t _len);
int ttyslot(void);
int fttyslot(int _fd);
char *crypt(const char *_key, const char *_salt);
int getsysinfo(endpoint_t who, int what, void *where);
int getsigset(sigset_t *sigset);
int getprocnr(void);
int getnprocnr(pid_t pid);
int getpprocnr(void);
int _pm_findproc(char *proc_name, int *proc_nr);
int allocmem(phys_bytes size, phys_bytes *base);
int freemem(phys_bytes size, phys_bytes base);

#define DEV_MAP 1
#define DEV_UNMAP 2
#define mapdriver(driver, device, style, force) \
	devctl(DEV_MAP, driver, device, style, force)
#define unmapdriver(device) devctl(DEV_UNMAP, 0, device, 0)

int devctl(int ctl_req, int driver, int device, int style, int force);
int mapdriver5(char *label, size_t len, int major, int style, int force);
int adddma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
int deldma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
int getdma(endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep);

pid_t getnpid(endpoint_t proc_nr);
uid_t getnuid(endpoint_t proc_nr);
gid_t getngid(endpoint_t proc_nr);

/* For compatibility with other Unix systems */
size_t getpagesize(void);
int setgroups(int ngroups, const gid_t *gidset);
int initgroups(const char *name, gid_t basegid);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __ASM_X86_UNISTD_32_H */
