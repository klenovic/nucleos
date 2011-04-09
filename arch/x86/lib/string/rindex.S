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

// char *rindex(const char *s, int c)
//      Look for the last occurrence a character in a string.  Has suffered
//      from a hostile takeover by strrchr().
//
.section .text
.globl rindex
        .p2align  4
rindex:
        jmp     strrchr
