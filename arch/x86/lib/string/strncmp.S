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

// int strncmp(const char *s1, const char *s2, size_t n)
//      Compare two strings.
//
.section .text
.globl strncmp
        .p2align  4
strncmp:
        mov     12(%esp), %ecx // Maximum length
        jmp     _strncmp       // Common code
