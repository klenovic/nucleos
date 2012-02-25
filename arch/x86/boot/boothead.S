/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>
#include <nucleos/const.h>
#include <kernel/const.h>

#define NOFULL_HEADER_SUPPORT	1

#if NOFULL_HEADER_SUPPORT
#define ASK_VGA			0
#define STACK_SIZE		0
#define COMMAND_LINE_SIZE	0
#define LOAD_PHYSICAL_ADDR	0
#define MIN_KERNEL_ALIGN_LG2	0
#define HEAP_END_PTR		0	// should be _end+STACK_SIZE-512
#define HDR_KERNEL_VERSION	0	// should be kernel_version - 512

#define VO__end			0
#define VO__text		0
#define ZO__end 		0
#define ZO_startup_32		0
#define ZO_z_extract_offset	0
#define ZO_input_data		0
#define ZO_z_input_len		0
#endif /* NOFULL_HEADER_SUPPORT */

/* Early kernel stack size */
#define KSTACK_BOOT_SIZE	1024

BOOTSEG	= 0x07C0	/* original address of boot-sector */
SYSSEG	= 0x1000	/* historical load address >> 4 */

#ifndef SVGA_MODE
#define SVGA_MODE ASK_VGA
#endif

#ifndef RAMDISK
#define RAMDISK 0
#endif

#ifndef ROOT_RDONLY
#define ROOT_RDONLY 1
#endif

	.code16
	.section ".bstext", "ax"

	.global bootsect_start
bootsect_start:

	# Normalize the start address
	ljmp	$BOOTSEG, $start2

start2:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp
	sti
	cld

	movw	$bugger_off_msg, %si

msg_loop:
	lodsb
	andb	%al, %al
	jz	bs_die
	movb	$0xe, %ah
	movw	$7, %bx
	int	$0x10
	jmp	msg_loop

bs_die:
	# Allow the user to press a key, then reboot
	xorw	%ax, %ax
	int	$0x16
	int	$0x19

	# int 0x19 should never return.  In case it does anyway,
	# invoke the BIOS reset code...
	ljmp	$0xf000,$0xfff0

	.section ".bsdata", "ax"
bugger_off_msg:
	.ascii  "Direct booting from floppy is no longer supported.\r\n"
	.ascii  "Please use a boot loader program instead.\r\n"
	.ascii  "\n"
	.ascii  "Remove disk and press any key to reboot . . .\r\n"
	.byte   0

	# Kernel attributes; used by setup.  This is part 1 of the
	# header, from the old boot sector.

	.section ".header", "ax"
	.globl hdr
hdr:
setup_sects:	.byte 0			/* Filled in by build.c */
root_flags:	.word ROOT_RDONLY
syssize:	.long 0			/* Filled in by build.c */
ram_size:	.word 0			/* Obsolete */
vid_mode:	.word SVGA_MODE
root_dev:	.word 0			/* Filled in by build.c */
boot_flag:	.word 0xAA55

	.globl _start
_start:
		# Explicitly enter this as bytes, or the assembler
		# tries to generate a 3-byte jump here, which causes
		# everything else to push off to the wrong offset.
		.byte	0xeb		# short (2-byte) jump
		.byte	start_of_setup-1f
1:

	# Part 2 of the header, from the old setup.S

		.ascii	"HdrS"		# header signature
		.word	0x020a		# header version number (>= 0x0105)
					# or else old loadlin-1.5 will fail)
		.globl realmode_swtch
realmode_swtch:	.word	0, 0		# default_switch, SETUPSEG
start_sys_seg:	.word	SYSSEG		# obsolete and meaningless, but just
					# in case something decided to "use" it
		.word	HDR_KERNEL_VERSION
					# pointing to kernel version string
					# above section of header is compatible
					# with loadlin-1.5 (header v1.5). Do not
					# change it.

type_of_loader:	.byte	0		# 0 means ancient bootloader, newer
					# bootloaders know to change this.
					# See Documentation/x86/boot.txt for
					# assigned ids

# flags, unused bits must be zero (RFU) bit within loadflags
loadflags:
LOADED_HIGH	= 1			# If set, the kernel is loaded high
CAN_USE_HEAP	= 0x80			# If set, the loader also has set
					# heap_end_ptr to tell how much
					# space behind setup.S can be used for
					# heap purposes.
					# Only the loader knows what is free
		.byte	LOADED_HIGH

setup_move_size: .word  0x8000		# size to move, when setup is not
					# loaded at 0x90000. We will move setup
					# to 0x90000 then just before jumping
					# into the kernel. However, only the
					# loader knows how much data behind
					# us also needs to be loaded.

code32_start:				# here loaders can put a different
					# start address for 32-bit code.
		.long	0x100000	# 0x100000 = default for big kernel

ramdisk_image:	.long	0		# address of loaded ramdisk image
					# Here the loader puts the 32-bit
					# address where it loaded the image.
					# This only will be read by the kernel.

ramdisk_size:	.long	0		# its size in bytes

bootsect_kludge:
		.long	0		# obsolete

heap_end_ptr:	.word	HEAP_END_PTR
					# (Header version 0x0201 or later)
					# space from here (exclusive) down to
					# end of setup code can be used by setup
					# for local heap purposes.

ext_loader_ver:
		.byte	0		# Extended boot loader version
ext_loader_type:
		.byte	0		# Extended boot loader type

cmd_line_ptr:	.long	0		# (Header version 0x0202 or later)
					# If nonzero, a 32-bit pointer
					# to the kernel command line.
					# The command line should be
					# located between the start of
					# setup and the end of low
					# memory (0xa0000), or it may
					# get overwritten before it
					# gets read.  If this field is
					# used, there is no longer
					# anything magical about the
					# 0x90000 segment; the setup
					# can be located anywhere in
					# low memory 0x10000 or higher.

ramdisk_max:	.long 0x7fffffff
					# (Header version 0x0203 or later)
					# The highest safe address for
					# the contents of an initrd
					# The current kernel allows up to 4 GB,
					# but leave it at 2 GB to avoid
					# possible bootloader bugs.

kernel_alignment:	.long CONFIG_PHYSICAL_ALIGN	#physical addr alignment
							#required for protected mode
							#kernel
#ifdef CONFIG_RELOCATABLE
relocatable_kernel:	.byte 1
#else
relocatable_kernel:	.byte 0
#endif
min_alignment:		.byte MIN_KERNEL_ALIGN_LG2	# minimum alignment
pad3:			.word 0

cmdline_size:		.long COMMAND_LINE_SIZE-1	#length of the command line,
							#added with boot protocol
							#version 2.06

hardware_subarch:	.long 0			# subarchitecture, added with 2.07
						# default to 0 for normal x86 PC

hardware_subarch_data:	.quad 0

payload_offset:		.long ZO_input_data
payload_length:		.long ZO_z_input_len

setup_data:		.quad 0			# 64-bit physical pointer to
						# single linked list of
						# struct setup_data

pref_address:		.quad LOAD_PHYSICAL_ADDR	# preferred load addr

#define ZO_INIT_SIZE	(ZO__end - ZO_startup_32 + ZO_z_extract_offset)
#define VO_INIT_SIZE	(VO__end - VO__text)
#if ZO_INIT_SIZE > VO_INIT_SIZE
#define INIT_SIZE	ZO_INIT_SIZE
#else
#define INIT_SIZE	VO_INIT_SIZE
#endif

init_size:		.long INIT_SIZE		# kernel initialization size

# End of setup header #####################################################

	.set LOADSEG,	0x9000	// Where this code is loaded.
	.set A_FLAGS,	2	// From nucleos/a.out.h, struct exec
	.set A_TEXT,	8
	.set A_DATA,	12
	.set A_BSS,	16
	.set A_TOTAL,	24
	.set A_HDRLEN,	__HDRLEN__ 	// header length

// Set segment registers and stack pointer using the programs own header//
// The header is either 32 bytes (short form) or 48 bytes (long form).  The
// bootblock will jump to address 0x10030 in both cases, calling one of the
// two jmpf instructions below. The should be linked to run at offset 0x0000.
	.section ".entrytext", "ax"
	.globl start_of_setup
start_of_setup:
	ljmp	$LOADSEG, $bootcode_start
bootcode_start:
	mov	$LOADSEG, %ax
	mov	%ax, %ds	// %ds = header
	movb	A_FLAGS, %al
	xor	%ax, %ax
	xchg	A_TEXT, %ax	// No text
	add	%ax, A_DATA	// Treat all text as data
				// total = text + data + bss + heap + stack
				// linked from the same starting point
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
	mov	$_bss, %di    // Start of bss is at end of data (defined by linker script)
	mov	$_ebss, %cx   // End of bss (begin of heap)
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
	call	boot

.section .rodata
	.p2align  1
c60:	.word  60              // Constants for MUL and DIV
c1024:	.word  1024
c1080:	.word  1080
c19663:	.word  19663
