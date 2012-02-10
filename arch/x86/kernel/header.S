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

	.code32
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

	.section ".entrytext", "ax"
start_of_setup:
	// Set up a C stack frame on the monitor stack. (The monitor sets cs and ds
	// right. The ss descriptor still references the monitor data segment.)
	movzwl	%sp, %esp	// monitor stack is a 16 bit stack
	push	%ebp
	mov	%esp, %ebp
	push	%esi
	push	%edi

	/* Copy the monitor global descriptor table to the address space of kernel and
	   switch over to it.  Prot_init() can then update it with immediate effect. */
	sgdt	gdt+GDT_SELECTOR		// get the monitor gdtr
	mov	gdt+GDT_SELECTOR+2, %esi	// absolute address of GDT
	mov	$gdt, %ebx			// address of kernel GDT
	mov	$8*8, %ecx			// copying eight descriptors
copygdt:
	movb	%es:(%esi), %al
	movb	%al, (%ebx)
	inc	%esi
	inc	%ebx
	loop	copygdt
	mov	gdt+DS_SELECTOR+2, %eax		// base of kernel data
	and	$0x00FFFFFF, %eax		// only 24 bits
	add	$gdt, %eax			// eax = vir2phys(gdt)
	mov	%eax, gdt+GDT_SELECTOR+2	// set base of GDT
	lgdt	gdt+GDT_SELECTOR		// switch over to kernel GDT
	/* Locate boot parameters, set up kernel segment registers and stack. */
	mov	4(%ebp), %ebx			// boot parameters offset
	mov	8(%ebp), %edx			// boot parameters length
	mov	12(%ebp), %eax			// address of a.out headers
	mov	%ds, %ax			// kernel data
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss
	mov	$k_boot_stktop, %esp		// set sp to point to the top of kernel stack
	/* Save boot parameters into these global variables for i386 code */
	mov	%edx, params_size
	mov	%ebx, params_offset
	movl	$SS_SELECTOR, mon_ds

	call	nucleos_startup

	.section ".initdata", "ax"
	.p2align 2

	.globl params_size
params_size: .long 0

	.globl params_offset
params_offset: .long 0

	.globl mon_ds
mon_ds: .long 0

	.section ".bss", "wa"
/*
 * This stack is used temporarily for booting only. We switch to a proper kernel
 * stack after the first trap to kernel
 */
	.space KSTACK_BOOT_SIZE
	.globl k_boot_stktop
k_boot_stktop:
