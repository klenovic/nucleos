/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __RS_PROTO_H
#define __RS_PROTO_H

/* Function prototypes. */

/* Structs used in prototypes must be declared as such first. */
struct rproc;

/* exec.c */
int dev_execve(int proc_e, char *exec, size_t exec_len,  char *argv[], char **env);

/* main.c */
int main(void);

/* manager.c */
int do_up(kipc_msg_t *m);
int do_down(kipc_msg_t *m);
int do_refresh(kipc_msg_t *m);
int do_restart(kipc_msg_t *m);
int do_lookup(kipc_msg_t *m);
int do_shutdown(kipc_msg_t *m);
void do_period(kipc_msg_t *m);
void do_exit(kipc_msg_t *m);
int do_getsysinfo(kipc_msg_t *m);

/* utility.c */
int publish_service(struct rproc *rp);

/* memory.c */
void* rs_startup_sbrk(size_t size);
void* rs_startup_sbrk_synch(size_t size);
int rs_startup_segcopy(endpoint_t src_proc, int src_s, int dst_s,
		       vir_bytes dst_vir, phys_bytes bytes);

#endif /* __RS_PROTO_H */
