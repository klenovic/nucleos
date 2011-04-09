/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// based on Kees J. Bot code

// void bcopy(const void *s1, void *s2, size_t n)
//      Copy a chunk of memory.  Handle overlap.
//      This is a BSD routine that escaped from the kernel.  Do not use.
//
.section .text

.globl bcopy
        .p2align  4
bcopy:
        mov     4(%esp), %eax   // Exchange string arguments
        xchg    8(%esp), %eax
        mov     %eax, 4(%esp)
        jmp     _memmove       // Call the proper routine
