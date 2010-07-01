/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// based on Kees J. Bot code

// char *strrchr(const char *s, int c)
//      Look for the last occurrence a character in a string.
//
.section .text
.globl strrchr
        .p2align  4
strrchr:
        push    %ebp
        mov     %esp, %ebp
        push    %edi
        mov     8(%ebp), %edi   // edi = string
        mov     $-1, %ecx
        xorb    %al, %al
        cld
        repne
        scasb                   // Look for the end of the string
        not     %ecx            // -1 - ecx = Length of the string + null
        dec     %edi            // Put edi back on the zero byte
        movb    12(%ebp), %al   // The character to look for
        std                     // Downwards search
        repne
        scasb
        cld                    // Direction bit back to default
        jne     failure
        lea     1(%edi), %eax  // Found it
        pop     %edi
        pop     %ebp
        ret
failure:xor     %eax, %eax        // Not there
        pop     %edi
        pop     %ebp
        ret
