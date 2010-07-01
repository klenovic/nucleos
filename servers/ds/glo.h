/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Global variables. */

/* The parameters of the call are kept here. */
extern int who;                 /* caller's proc number */
extern int callnr;              /* system call number */
extern int dont_reply;          /* normally 0; set to 1 to inhibit reply */

