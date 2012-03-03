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

	.section ".text", "ax"

//       inb() - Input one byte
//       unsigned inb(u16 port);
	.globl inb
inb:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        xor     %eax, %eax
        inb     %dx                      // read 1 byte
        pop     %ebp
        ret

//       inl() - Input one dword
//       unsigned inl(u16 port);
	.globl inl
inl:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        inl     %dx                      // read 1 dword
        pop     %ebp
        ret

//       insb() - Input a byte array
//       void insb(u16 port, void *buf, size_t count);
	.globl insb
insb:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %edi
        mov     8(%ebp),%edx              // port
        mov     12(%ebp),%edi             // buf
        mov     16(%ebp),%ecx             // byte count
        rep     insb                      // input many bytes
        pop     %edi
        pop     %ebp
        ret

//       insl() - Input a dword array
//       void insl(u16 port, void *buf, size_t count);
	.globl insl
insl:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %edi
        mov     8(%ebp),%edx              // port
        mov     12(%ebp),%edi             // buf
        mov     16(%ebp),%ecx             // byte count
        shr     $2,%ecx                   // dword count
        rep     insl                      // input many dwords
        pop     %edi
        pop     %ebp
        ret

//       insw() - Input a word array
//       void insw(u16 port, void *buf, size_t count);
	.globl insw
insw:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %edi
        mov     8(%ebp),%edx             // port
        mov     12(%ebp),%edi            // buf
        mov     16(%ebp),%ecx            // byte count
        shr     $1,%ecx                  // word count
        rep     insw                     // input many words
        pop     %edi
        pop     %ebp
        ret

//       intr_disable(), intr_enable - Disable/Enable hardware interrupts.
//       void intr_disable(void);
//       void intr_enable(void);
	.globl intr_disable
intr_disable:
        cli
        ret

	.globl intr_enable
intr_enable:
        sti
        ret

//       inw() - Input one word
//       unsigned inw(u16 port);
	.globl inw
inw:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        xor     %eax,%eax
        inw     %dx                       // read 1 word
        pop     %ebp
        ret

//       outb() - Output one byte
//       void outb(u16 port, u8 value);
	.globl outb
outb:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        mov     8+4(%ebp),%eax            // value
        outb    %dx                      // output 1 byte
        pop     %ebp
        ret

//       outl() - Output one dword
//       void outl(u16 port, u32_t value);
	.globl outl
outl:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        mov     8+4(%ebp),%eax            // value
        outl    %dx                      // output 1 dword
        pop     %ebp
        ret

//       outsb() - Output a byte array
//       void outsb(u16 port, void *buf, size_t count);
	.globl outsb
outsb:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %esi
        mov     8(%ebp),%edx              // port
        mov     12(%ebp),%esi            // buf
        mov     16(%ebp),%ecx             // byte count
        rep     outsb                     // output many bytes
        pop     %esi
        pop     %ebp
        ret

//       outsl() - Output a dword array
//       void outsl(u16 port, void *buf, size_t count);
	.globl outsl
outsl:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %esi
        mov     8(%ebp),%edx              // port
        mov     12(%ebp),%esi             // buf
        mov     16(%ebp),%ecx             // byte count
        shr     $2,%ecx                   // dword count
        rep     outsl                     // output many dwords
        pop     %esi
        pop     %ebp
        ret

//       outsw() - Output a word array
//       void outsw(u16 port, void *buf, size_t count);
	.globl outsw
outsw:
        push    %ebp
        mov     %esp,%ebp
        cld
        push    %esi
        mov     8(%ebp),%edx              // port
        mov     12(%ebp),%esi             // buf
        mov     16(%ebp),%ecx             // byte count
        shr     $1,%ecx                   // word count
        rep     outsw                     // output many words
        pop     %esi
        pop     %ebp
        ret

//       outw() - Output one word
//       void outw(u16 port, u16 value);
	.globl outw
outw:
        push    %ebp
        mov     %esp,%ebp
        mov     8(%ebp),%edx              // port
        mov     8+4(%ebp),%eax            // value
        outw    %dx                      // output 1 word
        pop     %ebp
        ret
