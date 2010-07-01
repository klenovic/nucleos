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

// int strcmp(const char *s1, const char *s2)
//      Compare two strings.
//
.section .text
.globl strcmp
        .p2align  4
strcmp:
        mov     $-1, %ecx      // Unlimited length
        jmp     _strncmp       // Common code
