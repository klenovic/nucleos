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

// int memcmp(const void *s1, const void *s2, size_t n)
//      Compare two chunks of memory.
//
.section .text
.globl memcmp
        .p2align  4
memcmp:
        cld
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     8(%ebp), %esi     // String s1
        mov     12(%ebp), %edi    // String s2
        mov     16(%ebp), %ecx    // Length
        cmp     $16, %ecx
        jb      cbyte           // Do not bother being smart with short arrays
        mov     %esi, %eax
        or      %edi, %eax
        testb   $1, %al
        jnz     cbyte           // Bit 0 set, use byte compare
        testb   $2, %al
        jnz     cword           // Bit 1 set, use word compare
clword: shrd    $2, %ecx, %eax  // Save low two bits of ecx in eax
        shr     $2, %ecx
        repe
        cmpsl                   // Compare longwords
        sub     $4, %esi
        sub     $4, %edi
        inc     %ecx            // Recompare the last longword
        shld    $2, %eax, %ecx  // And any excess bytes
        jmp     last
cword:  shrd    $1, %ecx, %eax  // Save low bit of ecx in eax
        shr     $1, %ecx
        repe
        cmpsw                   // Compare words
        sub     $2, %esi
        sub     $2, %edi
        inc     %ecx             // Recompare the last word
        shld    $1, %eax, %ecx   // And one more byte?
cbyte:  test    %ecx, %ecx       // Set 'Z' flag if ecx = 0
last:   repe
        cmpsb                   // Look for the first differing byte
        seta    %al             // al = (s1 > s2)
        setb    %ah             // ah = (s1 < s2)
        subb    %ah, %al
        movsbl  %al, %eax       // eax = (s1 > s2) - (s1 < s2), i.e. -1, 0, 1
        mov     %esi, %edx      // For bcmp() to play with
        pop     %edi
        pop     %esi
        pop     %ebp
        ret
