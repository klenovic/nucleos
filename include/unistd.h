/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <unistd.h> header contains a few miscellaneous manifest constants. */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <nucleos/types.h>
#include <nucleos/type.h>

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
#define _PM_SEG_FLAG (1L << 30)	/* for read() and write() to FS by PM */

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

/* NULL must be defined in <unistd.h> according to POSIX Sec. 2.7.1. */
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
int chown(const char *_path, _mnx_Uid_t _owner, _mnx_Gid_t _group);
int fchown(int fd, _mnx_Uid_t _owner, _mnx_Gid_t _group);
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
pid_t getnpid(int proc_nr);
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
int setgid(_mnx_Gid_t _gid);
int setegid(_mnx_Gid_t _gid);
int setpgid(pid_t _pid, pid_t _pgid);
pid_t setsid(void);
int setuid(_mnx_Uid_t _uid);
int seteuid(_mnx_Uid_t _uid);
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

int brk(char *_addr);
int chroot(const char *_name);
int lseek64(int _fd, u64_t _offset, int _whence, u64_t *_newpos);
int mknod(const char *_name, _mnx_Mode_t _mode, Dev_t _addr);
int mknod4(const char *_name, _mnx_Mode_t _mode, Dev_t _addr, long _size);
char *mktemp(char *_template);
int mount(char *_spec, char *_name, int _flag, char *type, char *args);
long ptrace(int _req, pid_t _pid, long _addr, long _data);
char *sbrk(int _incr);
int sync(void);
int fsync(int fd);
int umount(const char *_name);
int reboot(int _how, ...);
int gethostname(char *_hostname, size_t _len);
int getdomainname(char *_domain, size_t _len);
int ttyslot(void);
int fttyslot(int _fd);
char *crypt(const char *_key, const char *_salt);
int getsysinfo(int who, int what, void *where);
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
uid_t getpeuid(endpoint_t ep);
int adddma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
int deldma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
int getdma(endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep);

/* For compatibility with other Unix systems */
int getpagesize(void);
int setgroups(int ngroups, const gid_t *gidset);
int initgroups(const char *name, gid_t basegid);

#endif /* _UNISTD_H */
