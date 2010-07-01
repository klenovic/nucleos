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

// size_t strncat(char *s1, const char *s2, size_t n)
//      Append string s2 to s1.
//
.section .text
.globl strncat
        .p2align  4
strncat:
        mov     12(%esp), %edx // Maximum length
        jmp     _strncat       // Common code
