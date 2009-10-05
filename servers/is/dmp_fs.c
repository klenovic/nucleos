/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains procedures to dump to FS_PROC_NR' data structures.
 *
 * The entry points into this file are
 *   dtab_dump:   	display device <-> driver mappings	  
 *   fproc_dump:   	display FS_PROC_NR process table	  
 *
 * Created:
 *   Oct 01, 2004:	by Jorrit N. Herder
 */

#include "inc.h"
#include "../mfs/const.h"
#include "../vfs/const.h"
#include "../vfs/fproc.h"
#include "../vfs/dmap.h"
#include <nucleos/dmap.h>

struct fproc fproc[NR_PROCS];
struct dmap dmap[NR_DEVICES];

/*===========================================================================*
 *				fproc_dmp				     *
 *===========================================================================*/
void fproc_dmp()
{
  struct fproc *fp;
  int i, n=0;
  static int prev_i;

  getsysinfo(FS_PROC_NR, SI_PROC_TAB, fproc);

  printf("File System (FS_PROC_NR) process table dump\n");
  printf("-nr- -pid- -tty- -umask- --uid-- --gid-- -ldr- -sus-rev-proc-\n");
  for (i=prev_i; i<NR_PROCS; i++) {
  	fp = &fproc[i];
  	if (fp->fp_pid <= 0) continue;
  	if (++n > 22) break;
  	printf("%3d  %4d  %2d/%d  0x%05x %2d (%d)  %2d (%d)  %3d   %3d %3d ",
  		i, fp->fp_pid, 
  		((fp->fp_tty>>MAJOR)&BYTE), ((fp->fp_tty>>MINOR)&BYTE), 
  		fp->fp_umask,
  		fp->fp_realuid, fp->fp_effuid, fp->fp_realgid, fp->fp_effgid,
  		fp->fp_sesldr,
  		fp->fp_blocked_on, fp->fp_revived
  	);
	if (fp->fp_blocked_on == FP_BLOCKED_ON_OTHER)
		printf("%4d\n", fp->fp_task);
	else
		printf(" nil\n");
  }
  if (i >= NR_PROCS) i = 0;
  else printf("--more--\r");
  prev_i = i;
}

/*===========================================================================*
 *				dmap_flags				     *
 *===========================================================================*/
static char * dmap_flags(int flags)
{
	static char fl[10];
	strcpy(fl, "---");
	if(flags & DMAP_MUTABLE) fl[0] = 'M';
	if(flags & DMAP_BUSY)    fl[1] = 'S';
	if(flags & DMAP_BABY)    fl[2] = 'B';
	return fl;
}

/*===========================================================================*
 *				dtab_dmp				     *
 *===========================================================================*/
void dtab_dmp()
{
    int i;

    getsysinfo(FS_PROC_NR, SI_DMAP_TAB, dmap);
    
    printf("File System (FS_PROC_NR) device <-> driver mappings\n");
    printf("Major  Driver ept  Flags\n");
    printf("-----  ----------  -----\n");
    for (i=0; i<NR_DEVICES; i++) {
        if (dmap[i].dmap_driver == NONE) continue;
        printf("%5d  %10d  %s\n",
		i, dmap[i].dmap_driver, dmap_flags(dmap[i].dmap_flags));
    }
}

