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

// void bzero(void *s, size_t n)
//      Set a chunk of memory to zero.
//      This is a BSD routine that escaped from the kernel.  Do not use.
//
.section .text
.globl bzero
        .p2align  4
bzero:
        push    %ebp
        mov     %esp, %ebp
        push    12(%ebp)  // Size
        push    $0        // Zero
        push    8(%ebp)   // String
        call    memset    // Call the proper routine
        leave
        ret
