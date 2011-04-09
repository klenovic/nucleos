/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef WAIT_H
#define WAIT_H
/* WAIT.H
// General purpose waiting routines

// Function prototypes
*/
int WaitBitb (int paddr, int bitno, int state, long tmout);
int WaitBitw (int paddr, int bitno, int state, long tmout);
int WaitBitd (int paddr, int bitno, int state, long tmout);
int MemWaitw (unsigned int volatile *gaddr, int bitno, int state, long tmout);

#endif
