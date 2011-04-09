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

// void *memchr(const void *s, int c, size_t n)
//      Look for a character in a chunk of memory.
//
.section .text
.globl memchr
        .p2align  4
memchr:
        push    %ebp
        mov     %esp, %ebp
        push    %edi
        mov     8(%ebp), %edi     // edi = string
        movb    12(%ebp), %al     // The character to look for
        mov     16(%ebp), %ecx    // Length
        cmpb    $1, %cl           // 'Z' bit must be clear if ecx = 0
        cld
        repne
        scasb
        jne     failure
        lea     -1(%edi), %eax    // Found
        pop     %edi
        pop     %ebp
        ret
failure:
        xor     %eax, %eax
        pop     %edi
        pop     %ebp
        ret
