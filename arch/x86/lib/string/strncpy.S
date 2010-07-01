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

// char *strncpy(char *s1, const char *s2, size_t n)
//      Copy string s2 to s1.
//
.section .text
.globl strncpy
        .p2align  4
strncpy:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     16(%ebp), %ecx // Maximum length
        call    _strncpy       // Common code
        mov     %edx, %ecx     // Number of bytes not copied
        rep
        stosb                   // strncpy always copies n bytes by null padding
        mov     8(%ebp), %eax   // Return s1
        pop     %edi
        pop     %esi
        pop     %ebp
        ret
