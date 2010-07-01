/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//       oneC_sum() - One complement`s checksum
// base on code by Kees J. Bot

// See RFC 1071, "Computing the Internet checksum"
// See also the C version of this code.

.section .text

.globl oneC_sum
        .p2align  4
oneC_sum:
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        movzwl  8(%ebp), %eax             // Checksum of previous block
        mov     12(%ebp), %esi            // Data to compute checksum over
        mov     16(%ebp), %edi            // Number of bytes

        xor     %edx, %edx
        xorb    %cl, %cl
.Lalign:
        test    $3, %esi                  // Is the data aligned?
        jz      aligned
        test    %edi, %edi
        jz      0f
        movb    (%esi), %dl              // Rotate the first unaligned bytes
        dec     %edi                     // into the edx register
0:      inc     %esi
        ror     $8, %edx
        ror     $8, %eax                  // Rotate the checksum likewise
        addb    $8, %cl                   // Number of bits rotated
        jmp     .Lalign
aligned:add     %edx, %eax                // Summate the unaligned bytes
        adc     $0, %eax                  // Add carry back in for one`s complement

        jmp     add6test
        .p2align  4
add6:   add     (%esi), %eax              // Six times unrolled loop, see below
        adc     4(%esi), %eax
        adc     8(%esi), %eax
        adc     12(%esi), %eax
        adc     16(%esi), %eax
        adc     20(%esi), %eax
        adc     $0, %eax
        add     $24, %esi
add6test:
        sub     $24, %edi
        jae     add6
        add     $24, %edi

        jmp     add1test
        .p2align  4
add1:   add     (%esi), %eax              // while ((edi -= 4) >= 0)
        adc     $0, %eax                  //       eax += *esi++;
        add     $4, %esi                  // edi += 4;
add1test:
        sub     $4, %edi
        jae     add1
        add     $4, %edi

        jz      done                    // Are there extra bytes?
        mov     (%esi), %edx              // Load extra bytes in a full dword
        and     mask-4(,%edi,4), %edx      // Mask off excess
        add     %edx, %eax                // Add in the last bits
        adc     $0, %eax
done:   rol     %cl, %eax                 // Undo the rotation at the beginning
        mov     %eax, %edx
        shr     $16, %eax
        addw    %dx, %ax                  // Add the two words in eax to form
        adcw    $0, %ax                   // a 16 bit sum
        pop     %edi
        pop     %esi
        pop     %ebp
        ret

.section .rodata
        .p2align  2
mask:   .long  0x000000FF, 0x0000FFFF, 0x00FFFFFF
