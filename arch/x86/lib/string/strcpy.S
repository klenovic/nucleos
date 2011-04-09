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

// char *strcpy(char *s1, const char *s2)
//      Copy string s2 to s1.
//
.section .text
.globl strcpy
        .p2align  4
strcpy:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     $-1, %ecx        // Unlimited length
        call    _strncpy       // Common code
        mov     8(%ebp), %eax   // Return s1
        pop     %edi
        pop     %esi
        pop     %ebp
        ret
