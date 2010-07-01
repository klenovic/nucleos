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

// size_t strnlen(const char *s, size_t n)
//      Return the length of a string.
//
.section .text
.globl strnlen
        .p2align  4
strnlen:
        mov     8(%esp), %ecx  // Maximum length
        jmp     _strnlen       // Common code
