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

/* main.c */
int main(int argc, char **argv);

/* dmp.c */
void map_unmap_fkeys(int map);
int do_fkey_pressed(kipc_msg_t *m);
void mapping_dmp(void);
void vm_dmp(void);

/* dmp_kernel.c */
void proctab_dmp(void);
void procstack_dmp(void);
void memmap_dmp(void);
void privileges_dmp(void);
void image_dmp(void);
void irqtab_dmp(void);
void kmessages_dmp(void);
void monparams_dmp(void);
void kenv_dmp(void);
void timing_dmp(void);

/* dmp_pm.c */
void mproc_dmp(void);
void sigaction_dmp(void);

/* dmp_fs.c */
void dtab_dmp(void);
void fproc_dmp(void);

/* dmp_rs.c */
void rproc_dmp(void);

/* dmp_ds.c */
void data_store_dmp(void);
