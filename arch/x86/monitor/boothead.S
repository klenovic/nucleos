/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//       Boothead.s - BIOS support for boot.c            Author: Kees J. Bot
//
//
// This file contains the startup and low level support for the secondary
// boot program.  It contains functions for disk, tty and keyboard I/O,
// copying memory to arbitrary locations, etc.
//
// The primary bootstrap code supplies the following parameters in registers:
//       dl      = Boot-device.
//       es:%si   = Partition table entry if hard disk.
//

	.set LOADSEG,	0x1000	// Where this code is loaded.
	.set A_FLAGS,	2	// From nucleos/a.out.h, struct exec
	.set A_TEXT,	8
	.set A_DATA,	12
	.set A_BSS,	16
	.set A_TOTAL,	24
	.set A_SEP,	0x20 		// Separate I&D flag
	.set A_HDRLEN,	__HDRLEN__ 	// header length

// Imported variables and functions:
.extern caddr, daddr, runsize, edata, end      // Runtime environment
.extern device                                 // BIOS device number
.extern rem_part                               // To pass partition info
.extern k_flags                                // Special kernel flags
.extern mem                                    // Free memory list

/*
  Common I&D is when everything, code and data, is linked from the same
  starting point.  So code address 0 and data address 0 both address the
  first byte of the program.  All segment registers have the same value,
  pointing to that first byte.

  Separate I&D is when code and data are separate.  The data segment also
  starts at address 0.  The data segment registers have a different value
  from the code segment, pointing to byte 0 of data.
*/
// Set segment registers and stack pointer using the programs own header//
// The header is either 32 bytes (short form) or 48 bytes (long form).  The
// bootblock will jump to address 0x10030 in both cases, calling one of the
// two jmpf instructions below. The should be linked to run at offset 0x0000.

.section .text.head
.globl bootcode_entry                 // entry point
bootcode_entry:
	ljmp	$[LOADSEG + [A_HDRLEN >> 4]], $bootcode_start // Set cs right (skipping short a.out header)
bootcode_start:
	mov	$LOADSEG, %ax
	mov	%ax, %ds	// %ds = header
	movb	A_FLAGS, %al
	testb	$A_SEP, %al	// Separate I&D?
	jnz	sepID
comID:
	xor	%ax, %ax
	xchg	A_TEXT, %ax	// No text
	add	%ax, A_DATA	// Treat all text as data
				// total = text + data + bss + heap + stack
				// linked from the same starting point
sepID:
	mov	A_TOTAL, %ax	// Total nontext memory usage
	and	$0xFFFE, %ax	// Round down to even
	mov	%ax, A_TOTAL  // total - text = data + bss + heap + stack
	cli                   // Ignore interrupts while stack in limbo
	mov	%ax, %sp      // Set sp at the top of all that
	mov	A_TEXT, %ax   // Determine offset of %ds above cs
	movb	$4, %cl
	shr	%cl, %ax
	mov	%cs, %cx
	add	%cx, %ax
	mov	%ax, %ds      // %ds = cs + text / 16
	mov	%ax, %ss
	sti                   // Stack ok now
	push	%es           // Save %es, we need it for the partition table
	mov	%ax, %es
	cld                   // C compiler wants UP

// Clear bss
	xor	%ax, %ax      // Zero
	mov	$__bss, %di    // Start of bss is at end of data (defined by linker script)
	mov	$__ebss, %cx   // End of bss (begin of heap)
	sub	%di, %cx      // Number of bss bytes
	shr	$1, %cx       // Number of words
	rep
	stosw                 // Clear bss

// Copy primary boot parameters to variables.  (Can do this now that bss is
// cleared and may be written into).
	xorb	%dh, %dh
	mov	%dx, device      // Boot device (probably 0x00 or 0x80)
	mov	%si, rem_part+0  // Remote partition table offset
	popw	rem_part+2       // and segment (saved es)

// Remember the current video mode for restoration on exit.
	movb	$0x0F, %ah        // Get current video mode
	int	$0x10
	andb	$0x7F, %al        // Mask off bit 7 (no blanking)
	movb	%al, old_vid_mode
	movb	%al, cur_vid_mode

// Give C code access to the code segment, data segment and the size of this
// process.
	xor	%ax, %ax
	mov	%cs, %dx
	call	seg2abs
	mov	%ax, caddr+0
	mov	%dx, caddr+2
	xor	%ax, %ax
	mov	%ds, %dx
	call	seg2abs
	mov	%ax, daddr+0
	mov	%dx, daddr+2
	push	%ds
	mov	$LOADSEG, %ax
	mov	%ax, %ds           // Back to the header once more
	mov	A_TOTAL+0, %ax
	mov	A_TOTAL+2, %dx     // %dx:ax = data + bss + heap + stack
	add	A_TEXT+0, %ax
	adc	A_TEXT+2, %dx      // %dx:ax = text + data + bss + heap + stack
	pop	%ds
	mov	%ax, runsize+0
	mov	%dx, runsize+2     // 32 bit size of this process

// Determine available memory as a list of (base,size) pairs as follows:
// mem[0] = low memory, mem[1] = memory between 1M and 16M, mem[2] = memory
// above 16M.  Last two coalesced into mem[1] if adjacent.
	mov	$mem, %di          // %di = memory list
	int	$0x12              // Returns low memory size (in K) in %ax
	mulw	c1024
	mov	%ax, 4(%di)        // mem[0].size = low memory size in bytes
	mov	%dx, 6(%di)
	call	getprocessor
	cmp	$286, %ax          // Only 286s and above have extended memory
	jb	no_ext
	cmp	$486, %ax          // Assume 486s were the first to have >64M
	jb	small_ext          // (It helps to be paranoid when using the BIOS)
big_ext:
	mov	$0xE801, %ax       // Code for get memory size for >64M
	int	$0x15              // %ax = mem at 1M per 1K, %bx = mem at 16M per 64K
	jnc	got_ext
small_ext:
	movb	$0x88, %ah         // Code for get extended memory size
	clc                        // Carry will stay clear if call exists
	int	$0x15              // Returns size (in K) in %ax for AT''s
	jc	no_ext
	test	%ax, %ax           // An AT with no extended memory?
	jz	no_ext
	xor	%bx, %bx           // %bx = mem above 16M per 64K = 0
got_ext:
	mov	%ax, %cx           // %cx = copy of ext mem at 1M
	movw	$0x0010, 10(%di)   // mem[1].base = 0x00100000 (1M)
	mulw	c1024
	mov	%ax, 12(%di)       // mem[1].size = "ext mem at 1M" * 1024
	mov	%dx, 14(%di)
	test	%bx, %bx
	jz	no_ext             // No more ext mem above 16M?
	cmp	$15*1024, %cx      // Chunks adjacent? (precisely 15M at 1M?)
	je	adj_ext
	movw	$0x0100, 18(%di)   // mem[2].base = 0x01000000 (16M)
	mov	%bx, 22(%di)       // mem[2].size = "ext mem at 16M" * 64K
	jmp	no_ext
adj_ext:
	add	%bx, 14(%di)       // Add ext mem above 16M to mem below 16M
no_ext:

// Time to switch to a higher level language (not much higher)
	call	boot

/* @nucleos:
	In case of separate I&D it is really important to place into correct
	section otherwise it is placed into .text section which different 
	segment.
*/

.section .rodata

	.p2align  1
c60:	.word  60              // Constants for MUL and DIV
c1024:	.word  1024
c1080:	.word  1080
c19663:	.word  19663
