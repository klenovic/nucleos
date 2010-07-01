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

// void *_memmove(void *s1, const void *s2, size_t n)
//      Copy a chunk of memory.  Handle overlap.
//
.section .text

.globl _memmove
        .p2align  4
_memmove:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     8(%ebp), %edi      // String s1
        mov     12(%ebp), %esi     // String s2
        mov     16(%ebp), %ecx     // Length
        mov     %edi, %eax
        sub     %esi, %eax
        cmp     %ecx, %eax
        jb      downwards       // if (s2 - s1) < n then copy downwards

.globl _memcpy
_memcpy:
        cld                     // Clear direction bit: upwards
        cmp     $16, %ecx
        jb      upbyte          // Do not bother being smart with short arrays
        mov     %esi, %eax
        or      %edi, %eax
        testb   $1, %al
        jnz     upbyte          // Bit 0 set, use byte copy
        testb   $2, %al
        jnz     upword          // Bit 1 set, use word copy
uplword:
        shrdl   $2, %ecx, %eax // Save low 2 bits of ecx in eax
        shrl    $2, %ecx
        rep
        movsl                   // Copy longwords.
        shldl   $2, %eax, %ecx  // Restore excess count
upword: shr     $1, %ecx
        rep     movsw           // Copy words
        adc     %ecx, %ecx      // One more byte?
upbyte: rep
        movsb                   // Copy bytes
done:   mov     8(%ebp), %eax   // Absolutely noone cares about this value
        pop     %edi
        pop     %esi
        pop     %ebp
        ret

// Handle bad overlap by copying downwards, do not bother to do word copies.
downwards:
        std                     // Set direction bit: downwards
        lea     -1(%esi,%ecx,1), %esi
        lea     -1(%edi,%ecx,1), %edi
        rep
        movsb                   // Copy bytes
        cld
        jmp     done
