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

// void *memset(void *s, int c, size_t n)
//      Set a chunk of memory to the same byte value.
//
.section .text
.globl memset
        .p2align  4
memset:
        push    %ebp
        mov     %esp, %ebp
        push    %edi
        mov     8(%ebp), %edi     // The string
        movzbl  12(%ebp), %eax    // The fill byte
        mov     16(%ebp), %ecx    // Length
        cld
        cmp     $16, %ecx
        jb      sbyte           // Do not bother being smart with short arrays
        test    $1, %edi
        jnz     sbyte           // Bit 0 set, use byte store
        test    $2, %edi
        jnz     sword           // Bit 1 set, use word store
slword: movb    %al, %ah
        mov     %eax, %edx
        sal     $16, %edx
        or      %edx, %eax        // One byte to four bytes
        shrdl   $2, %ecx, %edx      // Save low two bits of ecx in edx
        shr     $2, %ecx
        rep
        stosl                   // Store longwords.
        shld    $2, %edx, %ecx       // Restore low two bits
sword:  movb    %al, %ah          // One byte to two bytes
        shr     $1, %ecx
        rep
        stosw                    // Store words
        adc     %ecx, %ecx      // One more byte?
sbyte:  rep
        stosb                   // Store bytes
done:   mov     8(%ebp), %eax   // Return some value you have no need for
        pop     %edi
        pop     %ebp
        ret
