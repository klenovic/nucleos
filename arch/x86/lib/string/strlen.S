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

// size_t strlen(const char *s)
//      Return the length of a string.
//
.section .text
.globl strlen
        .p2align  4
strlen:
        mov     $-1, %ecx       // Unlimited length
        jmp     _strnlen       // Common code
