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

// void *memcpy(void *s1, const void *s2, size_t n)
//      Copy a chunk of memory.
//      This routine need not handle overlap, so it does not handle overlap.
//      One could simply call __memmove, the cost of the overlap check is
//      negligible, but you are dealing with a programmer who believes that
//      if anything can go wrong, it should go wrong.
//
.section .text
.globl memcpy
        .p2align  4
memcpy:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        mov     8(%ebp), %edi     // String s1
        mov     12(%ebp), %esi    // String s2
        mov     16(%ebp), %ecx    // Length
        // No overlap check here
        jmp     _memcpy        // Call the part of _memmove that copies up
