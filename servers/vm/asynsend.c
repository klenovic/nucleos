/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#define _SYSTEM 1

#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/config.h>
#include <nucleos/const.h>
#include <server/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/ipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>

#include <errno.h>
#include <string.h>
#include <env.h>
#include <stdio.h>
#include <stdlib.h>

#include "proto.h"
#include "util.h"

#define SENDSLOTS NR_PROCS

PRIVATE asynmsg_t msgtable[SENDSLOTS];
PRIVATE size_t msgtable_n= SENDSLOTS;

PUBLIC int asynsend(dst, mp)
endpoint_t dst;
message *mp;
{
        int i;
        unsigned flags;

        /* Find slot in table */
        for (i= 0; i<msgtable_n; i++)
        {
                flags= msgtable[i].flags;
                if ((flags & (AMF_VALID|AMF_DONE)) == (AMF_VALID|AMF_DONE))
                {
                        if (msgtable[i].result != OK)
                        {
                                printf(
                      "VM: asynsend: found completed entry %d with error %d\n",
                                        i, msgtable[i].result);
                        }
                        break;
                }
                if (flags == AMF_EMPTY)
                        break;
        }
        if (i >= msgtable_n)
                vm_panic("asynsend: should resize table", i);
        msgtable[i].dst= dst;
        msgtable[i].msg= *mp;
        msgtable[i].flags= AMF_VALID;   /* Has to be last. The kernel
                                         * scans this table while we are
                                         * sleeping.
                                         */

        /* Tell the kernel to rescan the table */
        return senda(msgtable, msgtable_n);
}

