/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// based on Kees J. Bot code

// char *strchr(const char *s, int c)
//      Look for a character in a string.
//
.section .text
.globl strchr
        .p2align  4
strchr:
        push    %ebp
        mov     %esp, %ebp
        push    %edi
        cld
        mov     8(%ebp), %edi     // edi = string
        mov     $16, %edx         // Look at small chunks of the string
next:   shl     $1, %edx          // Chunks become bigger each time
        mov     %edx, %ecx
        xorb    %al, %al          // Look for the zero at the end
        repne
        scasb
        pushf                   // Remember the flags
        sub     %edx, %ecx
        neg     %ecx             // Some or all of the chunk
        sub     %ecx, %edi       // Step back
        movb    12(%ebp), %al    // The character to look for
        repne
        scasb
        je      found
        popf                    // Did we find the end of string earlier?
        jne     next            // No, try again
        xor     %eax, %eax        // Return NULL
        pop     %edi
        pop     %ebp
        ret
found:  pop     %eax             // Get rid of those flags
        lea     -1(%edi), %eax   // Address of byte found
        pop     %edi
        pop     %ebp
        ret
