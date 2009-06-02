/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
.section .text

.globl  getsysinfo
.globl  getsysinfo_up
.p2align 1
getsysinfo:
        jmp     _getsysinfo
getsysinfo_up:
        jmp     _getsysinfo_up
