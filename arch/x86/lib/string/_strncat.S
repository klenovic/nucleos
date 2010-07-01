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

// char *_strncat(char *s1, const char *s2, size_t edx)
//      Append string s2 to s1.
//
.section .text
.globl _strncat
        .p2align  4
_strncat:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     8(%ebp), %edi    // String s1
        mov     $-1, %ecx
        xorb    %al, %al         // Null byte
        cld
        repne
        scasb                    // Look for the zero byte in s1
        dec     %edi             // Back one up (and clear 'Z' flag)
        push    %edi             // Save end of s1
        mov     12(%ebp), %edi   // edi = string s2
        mov     %edx, %ecx       // Maximum count
        repne
        scasb                   // Look for the end of s2
        jne     no0
        inc     %ecx            // Exclude null byte
no0:    sub     %ecx, %edx      // Number of bytes in s2
        mov     %edx, %ecx
        mov     12(%ebp), %esi  // esi = string s2
        pop     %edi            // edi = end of string s1
        rep
        movsb                   // Copy bytes
        stosb                   // Add a terminating null
        mov     8(%ebp), %eax   // Return s1
        pop     %edi
        pop     %esi
        pop     %ebp
        ret
