/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_PROTO_H
#define __SERVERS_VFS_PROTO_H

/* Function prototypes. */
#include <nucleos/timer.h>
#include "request.h"

/* Structs used in prototypes must be declared as such first. */
struct filp;
struct fproc;
struct vmnt;
struct vnode;

/* device.c */
int dev_open(dev_t dev, int proc, int flags);
int dev_reopen(dev_t dev, int filp_no, int flags);
int dev_close(dev_t dev, int filp_no);
int dev_io(int op, dev_t dev, int proc, void *buf, u64_t pos, int bytes, int flags,
	   int suspend_reopen);
int gen_opcl(int op, dev_t dev, int proc, int flags);
int gen_io(int task_nr, kipc_msg_t *mess_ptr);
int asyn_io(int task_nr, kipc_msg_t *mess_ptr);
int no_dev(int op, dev_t dev, int proc, int flags);
int no_dev_io(int, kipc_msg_t *);
int tty_opcl(int op, dev_t dev, int proc, int flags);
int ctty_opcl(int op, dev_t dev, int proc, int flags);
int clone_opcl(int op, dev_t dev, int proc, int flags);
int ctty_io(int task_nr, kipc_msg_t *mess_ptr);
int do_ioctl(void);
void pm_setsid(int proc_e);
void dev_status(kipc_msg_t *);
void dev_up(int major);
endpoint_t suspended_ep(endpoint_t driver, cp_grant_id_t g);
void reopen_reply(void);

/* dmap.c */
int do_devctl(void);
int fs_devctl(int req, int dev, int proc_nr_e, int style, int force);
int do_mapdriver(void);
void build_dmap(void);
int map_driver(int major, int proc_nr, int dev_style, int force);
int dmap_driver_match(endpoint_t proc, int major);
void dmap_unmap_by_endpt(int proc_nr);
void dmap_endpt_up(int proc_nr);

/* exec.c */
int pm_exec(int proc_e, char *path, vir_bytes path_len, char *frame, vir_bytes frame_len);

/* filedes.c */
struct filp *find_filp(struct vnode *vp, mode_t bits);
int get_fd(int start, mode_t bits, int *k, struct filp **fpt);
struct filp *get_filp(int fild);
struct filp *get_filp2(struct fproc *rfp, int fild);
int invalidate(struct filp *);

/* fscall.c */
void nested_fs_call(kipc_msg_t *m);

/* link.c */
int do_link(void);
int do_unlink(void);
int scall_rmdir(void);
int do_rename(void);
int do_truncate(void);
int do_ftruncate(void);
int truncate_vnode(struct vnode *vp, off_t newsize);

/* lock.c */
int lock_op(struct filp *f, int req);
void lock_revive(void);

/* main.c */
int main(void);
void reply(int whom, int result);

/* misc.c */
int do_dup(void);
int do_dup2(void);
void pm_exit(int proc);
int do_fcntl(void);
void pm_fork(int pproc, int cproc, int cpid);
void pm_setgid(int proc_e, int egid, int rgid);
void pm_setuid(int proc_e, int euid, int ruid);
void pm_setgroups(int proc_e, int ngroups, gid_t *addr);
int do_sync(void);
int do_fsync(void);
void pm_reboot(void);
int do_svrctl(void);
int do_getsysinfo(void);
int pm_dumpcore(int proc_e, struct mem_map *seg_ptr);

/* mmap.c */
int do_vm_mmap(void);

/* mount.c */
int do_fsready(void);
int do_mount(void);
int do_umount(void);
int unmount(dev_t dev);

/* open.c */
int do_close(void);
int close_fd(struct fproc *rfp, int fd_nr);
void close_filp(struct filp *fp);
void close_reply(void);
int do_creat(void);
int scall_lseek(void);
int scall_llseek(void);
int do_mknod(void);
int do_mkdir(void);
int scall_open(void);
int scall_symlink(void);
int do_vm_open(void);
int do_vm_close(void);

/* path.c */
struct vnode *advance(struct vnode *dirp, int flags);
struct vnode *eat_path(int flags);
struct vnode *last_dir(void);

/* pipe.c */
int map_vnode(struct vnode *vp);
int scall_pipe(void);
void unpause(int proc_nr_e);
int pipe_check(struct vnode *vp, int rw_flag, int oflags, int bytes, u64_t position, int notouch);
void release(struct vnode *vp, int call_nr, int count);
void revive(int proc_nr, int bytes);
void suspend(int task);
void pipe_suspend(int rw_flag, int fd_nr, char *buf, size_t size);
int select_request_pipe(struct filp *f, int *ops, int bl);
int select_match_pipe(struct filp *f);
void unsuspend_by_endpt(endpoint_t);
void select_reply1(void);
void select_reply2(void);
void wait_for(endpoint_t);
#if DO_SANITYCHECKS
int check_pipe(void);
#endif

/* protect.c */
int do_access(void);
int do_chmod(void);
int do_fchmod(void);
int do_chown(void);
int do_fchown(void);
int do_umask(void);
int forbidden(struct vnode *vp, mode_t access_desired);
int read_only(struct vnode *vp);

/* read.c */
int do_read(void);
int do_getdents(void);
int read_write(int rw_flag);
int rw_pipe(int rw_flag, endpoint_t usr, int fd_nr, struct filp *f, char *buf, size_t req_size);

/* request.c */
int req_breadwrite(endpoint_t fs_e, endpoint_t user_e, dev_t dev, u64_t pos,
		   unsigned int num_of_bytes, char *user_addr, int rw_flag,
		   u64_t *new_posp, unsigned int *cum_iop);
int req_chmod(int fs_e, ino_t inode_nr, mode_t rmode, mode_t *new_modep);
int req_chown(endpoint_t fs_e, ino_t inode_nr, uid_t newuid, gid_t newgid,
	      mode_t *new_modep);
int req_create(int fs_e, ino_t inode_nr, int omode, int uid, int gid, char *path,
	       node_details_t *res);
int req_flush(endpoint_t fs_e, dev_t dev);
int req_fstatfs(int fs_e, int who_e, char *buf);
int req_ftrunc(endpoint_t fs_e, ino_t inode_nr, off_t start, off_t end);
int req_getdents(endpoint_t fs_e, ino_t inode_nr, u64_t pos,
		 char *buf, size_t size, u64_t *new_pos);
int req_inhibread(endpoint_t fs_e, ino_t inode_nr);
int req_link(endpoint_t fs_e, ino_t link_parent, char *lastc, ino_t linked_file);
int req_lookup(endpoint_t fs_e, ino_t dir_ino, ino_t root_ino,
	       uid_t uid, gid_t gid, int flags,
	       lookup_res_t *res);

int req_mkdir(endpoint_t fs_e, ino_t inode_nr, char *lastc, uid_t uid, gid_t gid,
	      mode_t dmode);
int req_mknod(endpoint_t fs_e, ino_t inode_nr, char *lastc, uid_t uid, gid_t gid,
	      mode_t dmode, dev_t dev);
int req_mountpoint(endpoint_t fs_e, ino_t inode_nr);
int req_newnode(endpoint_t fs_e, uid_t uid, gid_t gid, mode_t dmode,
		dev_t dev, struct node_details *res);
int req_putnode(int fs_e, ino_t inode_nr, int count);
int req_rdlink(endpoint_t fs_e, ino_t inode_nr, endpoint_t who_e, char *buf, size_t len);
int req_readsuper(endpoint_t fs_e, char *driver_name, dev_t dev, int readonly, int isroot,
		  struct node_details *res_nodep);
int req_readwrite(endpoint_t fs_e, ino_t inode_nr, u64_t pos, int rw_flag,
		  endpoint_t user_e, char *user_addr, unsigned int num_of_bytes, u64_t *new_posp,
		  unsigned int *cum_iop); 
int req_rename(endpoint_t fs_e, ino_t old_dir, char *old_name, ino_t new_dir, char *new_name);
int req_rmdir(endpoint_t fs_e, ino_t inode_nr, char *lastc);
int req_slink(endpoint_t fs_e, ino_t inode_nr, char *lastc, endpoint_t who_e, char *path_addr,
	      int path_length, uid_t uid, gid_t gid);
int req_stat(int fs_e, ino_t inode_nr, int who_e, char *buf, int pos);
int req_sync(endpoint_t fs_e);
int req_unlink(endpoint_t fs_e, ino_t inode_nr, char *lastc);
int req_unmount(endpoint_t fs_e);
int req_utime(endpoint_t fs_e, ino_t inode_nr, time_t actime, time_t modtime);
int req_newdriver(endpoint_t fs_e, dev_t dev, endpoint_t driver_e);

/* stadir.c */
int do_chdir(void);
int do_fchdir(void);
int do_chroot(void);
int do_fstat(void);
int do_stat(void);
int do_fstatfs(void);
int do_readlink(void);
int do_lstat(void);

/* time.c */
int scall_utime(void);

/* utility.c */
time_t clock_time(void);
unsigned conv2(int norm, int w);
long conv4(int norm, long x);
int fetch_name(char *buff, const int pathmax, char __user *path);
int no_sys(void);
int isokendpt_f(char *f, int l, int e, int *p, int ft);
void panic(char *who, char *mess, int num);

#define okendpt(e, p) isokendpt_f(__FILE__, __LINE__, (e), (p), 1)
#define isokendpt(e, p) isokendpt_f(__FILE__, __LINE__, (e), (p), 0)

/* vmnt.c */
struct vmnt *get_free_vmnt(short *index);
struct vmnt *find_vmnt(int fs_e);

/* vnode.c */
struct vnode *get_free_vnode(void);
struct vnode *find_vnode(int fs_e, int numb);
void dup_vnode(struct vnode *vp);
void put_vnode(struct vnode *vp);
void vnode_clean_refs(struct vnode *vp);
#if DO_SANITYCHECKS
int check_vrefs(void);
#endif

/* write.c */
int do_write(void);

/* select.c */
int do_select(void);
int select_callback(struct filp *, int ops);
void select_forget(int fproc);
void select_timeout_check(timer_t *);
void init_select(void);
void select_unsuspend_by_endpt(endpoint_t proc);
int select_notified(int major, int minor, int ops);

/* timers.c */
void fs_set_timer(timer_t *tp, int delta, tmr_func_t watchdog, int arg);
void fs_expire_timers(clock_t now);
void fs_cancel_timer(timer_t *tp);
void fs_init_timer(timer_t *tp);
#endif
