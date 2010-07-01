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
// int strncmp(const char *s1, const char *s2, size_t ecx)
//      Compare two strings.
//
.section .text

.globl _strncmp
        .p2align  4
_strncmp:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        test    %ecx, %ecx     // Max length is zero?
        je      done
        mov     8(%ebp), %esi  // esi = string s1
        mov     12(%ebp), %edi // edi = string s2
        cld
compare:
        cmpsb                 // Compare two bytes
        jne     done
        cmpb    $0, -1(%esi)  // End of string?
        je      done
        dec     %ecx          // Length limit reached?
        jne     compare
done:   seta    %al           // al = (s1 > s2)
        setb    %ah           // ah = (s1 < s2)
        subb    %ah, %al
        movsbl  %al, %eax     // eax = (s1 > s2) - (s1 < s2), i.e. -1, 0, 1
        pop     %edi
        pop     %esi
        pop     %ebp
        ret
