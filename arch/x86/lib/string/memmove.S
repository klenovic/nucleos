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

// void *memmove(void *s1, const void *s2, size_t n)
//      Copy a chunk of memory.  Handle overlap.
//
.section .text
.globl memmove
        .p2align  4
memmove:
        jmp     _memmove       // Call common code
