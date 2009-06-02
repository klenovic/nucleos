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

// char *_strncpy(char *s1, const char *s2, size_t ecx)
//      Copy string s2 to s1.
//
.section .text
.globl _strncpy
        .p2align  4
_strncpy:
        mov     12(%ebp), %edi  // edi = string s2
        xorb    %al, %al        // Look for a zero byte
        mov     %ecx, %edx      // Save maximum count
        cld
        repne
        scasb                   // Look for end of s2
        sub     %ecx, %edx      // Number of bytes in s2 including null
        xchg    %edx, %ecx
        mov     12(%ebp), %esi  // esi = string s2
        mov     8(%ebp), %edi   // edi = string s1
        rep
        movsb                   // Copy bytes
        ret
