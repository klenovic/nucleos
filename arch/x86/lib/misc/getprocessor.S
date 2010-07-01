/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//       getprocessor() - determine processor type
// based on code by Kees J. Bot
.section .text

// int getprocessor(void);
//       Return 386, 486, 586, ...

.globl getprocessor
getprocessor:
        push    %ebp
        mov     %esp, %ebp
        and     $0xFFFFFFFC, %esp // Align stack to avoid AC fault
        mov     $0x00040000, %ecx // Try to flip the AC bit introduced on the 486
        call    flip
        mov     $386, %eax        // 386 if it did not react to "flipping"
        jz      gotprocessor
        mov     $0x00200000, %ecx // Try to flip the ID bit introduced on the 586
        call    flip
        mov     $486, %eax        // 486 if it did not react
        jz      gotprocessor
        pushf
        pusha                     // Save the world
        mov     $1, %eax
        cpuid      // CPUID instruction tells the processor type
        andb    $0x0F, %ah        // Extract the family (5, 6, ...)
        movzbl  %ah, %eax
        cmp     $15, %eax         // 15: extended family
        jne     direct
        mov     $6, %eax           // Make it 686
direct:
        imul    $100, %eax         // 500, 600, ...
        add     $86, %eax          // 586, 686, ...
        mov     %eax, 7*4(%esp)   // Pass eax through
        popa
        popf
gotprocessor:
        leave
        ret

flip:
        pushf                   // Push eflags
        pop     %eax            // eax = eflags
        mov     %eax, %edx      // Save original eflags
        xor     %ecx, %eax      // Flip the bit to test
        push    %eax            // Push modified eflags value
        popf                    // Load modified eflags register
        pushf
        pop     %eax            // Get it again
        push    %edx
        popf                    // Restore original eflags register
        xor     %edx, %eax      // See if the bit changed
        test    %ecx, %eax
        ret
