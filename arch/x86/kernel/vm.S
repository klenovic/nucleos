/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

.globl load_kernel_cr3
.globl last_cr3

#define LOADKERNELCR3                ;\
        incl    cr3switch            ;\
        mov     kernel_cr3, %eax     ;\
        cmp     %eax, last_cr3       ;\
        jz      9f                   ;\
        push    $load_kernel_cr3      ;\
        call    level0               ;\
        pop     %eax                 ;\
        mov     kernel_cr3, %eax     ;\
        mov     %eax, last_cr3       ;\
        incl    cr3reload            ;\
9:

#define LOADCR3WITHEAX(type, newcr3)   ;\
        incl    %ss:cr3switch         ;\
        mov     %ss:newcr3, %eax       ;\
        cmp     %eax, %ss:last_cr3     ;\
        jz      8f                     ;\
        mov     %eax, %cr3             ;\
        incl    %ss:cr3reload         ;\
        mov     %eax, %ss:last_cr3    ;\
8:

