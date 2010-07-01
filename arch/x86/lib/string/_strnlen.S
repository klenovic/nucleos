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

// size_t _strnlen(const char *s, size_t ecx)
//      Return the length of a string.
//
.section .text
.globl _strnlen
        .p2align  4
_strnlen:
        push    %ebp
        mov     %esp, %ebp
        push    %edi
        mov     8(%ebp), %edi  // edi = string
        xorb    %al, %al       // Look for a zero byte
        mov     %ecx, %edx     // Save maximum count
        cmpb    $1, %cl        // 'Z' bit must be clear if ecx = 0
        cld
        repne
        scasb                  // Look for zero
        jne     no0
        inc     %ecx           // Do not count zero byte
no0:    mov     %edx, %eax
        sub     %ecx, %eax     // Compute bytes scanned
        pop     %edi
        pop     %ebp
        ret
