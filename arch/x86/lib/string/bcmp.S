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

// int bcmp(const void *s1, const void *s2, size_t n)
//      Compare two chunks of memory.
//      This is a BSD routine that escaped from the kernel.  Don't use.
//      (Alas it is not without some use, it reports the number of bytes
//      after the bytes that are equal.  So it can't be simply replaced.)
//
.section .text

.globl bcmp
        .p2align  4
bcmp:
        push    %ebp
        mov     %esp, %ebp
        push    16(%ebp)
        push    12(%ebp)
        push    8(%ebp)
        call    _memcmp         // Let memcmp do the work
        test    %eax, %eax
        jz      equal
        sub     8(%ebp), %edx   // Memcmp was nice enough to leave "esi" in edx
        dec     %edx            // Number of bytes that are equal
        mov     16(%ebp), %eax
        sub     %edx, %eax      // Number of bytes that are unequal
equal:  leave
        ret
