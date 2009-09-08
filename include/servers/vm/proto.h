/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VM_PROTO_H
#define __SERVERS_VM_PROTO_H

/* Function prototypes. */

struct vmproc;
struct stat;
struct mem_map;
struct memory;

#include <nucleos/kipc.h>
#include <nucleos/endpoint.h>
#include <nucleos/safecopies.h>
#include <nucleos/timer.h>
#include <stdio.h>
#include <asm/pagetable.h>
#include <servers/vm/vmproc.h>
#include <servers/vm/vm.h>

/* alloc.c */
phys_clicks alloc_mem_f(phys_clicks clicks, u32_t flags);
int do_adddma(message *msg);
int do_deldma(message *msg);
int do_getdma(message *msg);
int do_allocmem(message *msg);
void release_dma(struct vmproc *vmp);

void free_mem_f(phys_clicks base, phys_clicks clicks);

#define ALLOC_MEM(clicks, flags) alloc_mem_f(clicks, flags)
#define FREE_MEM(base, clicks) free_mem_f(base, clicks)

void mem_init(struct memory *chunks);
void memstats(void);

/* utility.c */
int get_mem_map(int proc_nr, struct mem_map *mem_map);
void get_mem_chunks(struct memory *mem_chunks);
void reserve_proc_mem(struct memory *mem_chunks, struct mem_map *map_ptr);
int reserve_initrd_mem(struct memory *mem_chunks);
int vm_isokendpt(endpoint_t ep, int *proc);
int get_stack_ptr(int proc_nr, vir_bytes *sp);

/* exit.c */
void clear_proc(struct vmproc *vmp);
int do_exit(message *msg);
int do_willexit(message *msg);
void free_proc(struct vmproc *vmp);

/* fork.c */
int do_fork(message *msg);

/* exec.c */
struct vmproc *find_share(struct vmproc *vmp_ign, ino_t ino, dev_t dev, time_t ctime);
int do_exec_newmem(message *msg);
int proc_new(struct vmproc *vmp, phys_bytes start, phys_bytes text, phys_bytes data,
	     phys_bytes stack, phys_bytes gap, phys_bytes text_here, phys_bytes data_here,
	     vir_bytes stacktop);
phys_bytes find_kernel_top(void);

/* break.c */
int do_brk(message *msg);
int adjust(struct vmproc *rmp, vir_clicks data_clicks, vir_bytes sp);
int real_brk(struct vmproc *vmp, vir_bytes v);

/* signal.c */
int do_push_sig(message *msg);

/* vfs.c */
int do_vfs_reply(message *msg);
int vfs_open(struct vmproc *for_who, callback_t callback, cp_grant_id_t filename_gid,
	     int filename_len, int flags, int mode);
int vfs_close(struct vmproc *for_who, callback_t callback, int fd);

/* mmap.c */
int do_mmap(message *msg);
int do_map_phys(message *msg);
int do_unmap_phys(message *msg);

/* pagefaults.c */
void do_pagefaults(void);
void do_memory(void);
char *pf_errstr(u32_t err);

/* $(ARCH)/pagetable.c */
void pt_init(void);
int pt_new(pt_t *pt);
void pt_free(pt_t *pt);
void pt_freerange(pt_t *pt, vir_bytes lo, vir_bytes hi);
int pt_writemap(pt_t *pt, vir_bytes v, phys_bytes physaddr, size_t bytes, u32_t flags,
		u32_t writemapflags);
int pt_bind(pt_t *pt, struct vmproc *who);
void *vm_allocpages(phys_bytes *p, int pages, int cat);
void pt_cycle(void);
int pt_mapkernel(pt_t *pt);
void phys_readaddr(phys_bytes addr, phys_bytes *v1, phys_bytes *v2);
void phys_writeaddr(phys_bytes addr, phys_bytes v1, phys_bytes v2);

#if SANITYCHECKS
void pt_sanitycheck(pt_t *pt, char *file, int line);
#endif

/* $(ARCH)/pagefaults.c */
int arch_get_pagefault(endpoint_t *who, vir_bytes *addr, u32_t *err);

/* slaballoc.c */
void *slaballoc(int bytes);
void slabfree(void *mem, int bytes);
void slabstats(void);

#define SLABALLOC(var) (var = slaballoc(sizeof(*var)))
#define SLABFREE(ptr) slabfree(ptr, sizeof(*(ptr)))
#if SANITYCHECKS
int slabsane(void *mem, int bytes);
#define SLABSANE(ptr) { \
       if(!slabsane(ptr, sizeof(*(ptr)))) { \
               printf("VM:%s:%d: SLABSANE(%s)\n", __FILE__, __LINE__, #ptr); \
               vm_panic("SLABSANE failed", NO_NUM);    \
       } \
}
#else
#define SLABSANE(ptr)
#endif
/* region.c */
struct vir_region * map_page_region(struct vmproc *vmp, vir_bytes min, vir_bytes max,
				    vir_bytes length, vir_bytes what, u32_t flags, int mapflags);
struct vir_region * map_proc_kernel(struct vmproc *dst);
int map_region_extend(struct vmproc *vmp, struct vir_region *vr, vir_bytes delta);
int map_region_shrink(struct vir_region *vr, vir_bytes delta);
int map_unmap_region(struct vmproc *vmp, struct vir_region *vr);
int map_free_proc(struct vmproc *vmp);
int map_proc_copy(struct vmproc *dst, struct vmproc *src);
struct vir_region *map_lookup(struct vmproc *vmp, vir_bytes addr);
int map_pf(struct vmproc *vmp, struct vir_region *region, vir_bytes offset, int write);
int map_handle_memory(struct vmproc *vmp, struct vir_region *region, vir_bytes offset,
		       vir_bytes len, int write);

struct vir_region * map_region_lookup_tag(struct vmproc *vmp, u32_t tag);
void map_region_set_tag(struct vir_region *vr, u32_t tag);
u32_t map_region_get_tag(struct vir_region *vr);


#if SANITYCHECKS
void map_sanitycheck(char *file, int line);
#endif

/* $(ARCH)/vm.c */
void arch_init_vm(struct memory mem_chunks[NR_MEMS]);
vir_bytes arch_map2vir(struct vmproc *vmp, vir_bytes addr);
vir_bytes arch_vir2map(struct vmproc *vmp, vir_bytes addr);

#endif /* __SERVERS_VM_PROTO_H */
