/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Virtual mount table related routines.
 *
 */

#include "fs.h"
#include "vmnt.h"

struct vmnt vmnt[NR_MNTS];

/*===========================================================================*
 *                             get_free_vmnt				     *
 *===========================================================================*/
struct vmnt *get_free_vmnt(short *index)
{
  struct vmnt *vp;
  *index = 0;
  for (vp = &vmnt[0]; vp < &vmnt[NR_MNTS]; ++vp, ++(*index)) 
      if (vp->m_dev == NO_DEV) return(vp);

  return(NIL_VMNT);
}


/*===========================================================================*
 *                             find_vmnt				     *
 *===========================================================================*/
struct vmnt *find_vmnt(int fs_e) 
{
  struct vmnt *vp;
  for (vp = &vmnt[0]; vp < &vmnt[NR_MNTS]; ++vp) 
      if (vp->m_fs_e == fs_e) return(vp);

  return(NIL_VMNT);
}


