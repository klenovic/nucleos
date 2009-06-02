/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//	masterboot 2.0 - Master boot block code		Author: Kees J. Bot
//
// This code may be placed in the first sector (the boot sector) of a floppy,
// hard disk or hard disk primary partition.  There it will perform the
// following actions at boot time:
//
// - If the booted device is a hard disk and one of the partitions is active
//   then the active partition is booted.
//
// - Otherwise the next floppy or hard disk device is booted, trying them one
//   by one.
//
// To make things a little clearer, the boot path might be:
//	/dev/fd0	- Floppy disk containing data, tries fd1 then d0
//	[/dev/fd1]	- Drive empty
//	/dev/c0d0	- Master boot block, selects active partition 2
//	/dev/c0d0p2	- Submaster, selects active subpartition 0
//	/dev/c0d0p2s0	- Minix bootblock, reads Boot Monitor /boot
//	Minix		- Started by /boot from a kernel image in /minix

	.set	LOADOFF	   ,	0x7C00	// 0x0000:LOADOFF is where this code is loaded
	.set	BUFFER	   ,	0x0600	// First free memory
	.set	PART_TABLE ,	   446	// Location of partition table within this code
	.set	PENTRYSIZE ,	    16	// Size of one partition table entry
	.set	MAGIC	   ,	   510	// Location of the AA55 magic number

	// <ibm/partition>.h:
	.set	bootind	   ,	     0
	.set	sysind	   ,	     4
	.set	lowsec	   ,	     8

.section .text
// Find active (sub)partition, load its first sector, run it.
.globl start
start:
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es
	cli
	mov	%ax, %ss	// ds = es = ss = Vector segment
	mov	$LOADOFF, %sp
	sti

// Copy this code to safety, then jump to it.
	mov	%sp, %si		// si = start of this code
	push	%si			// Also where we will return to eventually
	mov	$BUFFER, %di		// Buffer area
	mov	$512/2, %cx		// One sector
	cld
	rep	movsw
	ljmp	$0, $BUFFER+migrate	// To safety
migrate:

// Find the active partition
findactive:
	testb	%dl, %dl
	jns	nextdisk		// No bootable partitions on floppies
	mov	$BUFFER+PART_TABLE, %si
find:	cmpb	$0, sysind(%si)	// Partition type, nonzero when in use
	jz	nextpart
	testb	$0x80, bootind(%si)	// Active partition flag in bit 7
	jz	nextpart		// It is not active
loadpart:
	call	load			// Load partition bootstrap
	jc	error1			// Not supposed to fail
bootstrap:
	ret				// Jump to the master bootstrap
nextpart:
	add	$PENTRYSIZE, %si
	cmp	$BUFFER+PART_TABLE+4*PENTRYSIZE, %si
	jb	find
// No active partition, tell them
	call	print
	.ascii	"No active partition\0"
	jmp	reboot

// There are no active partitions on this drive, try the next drive.
nextdisk:
	incb	%dl			// Increment dl for the next drive
	testb	%dl, %dl
	js	nexthd			// Hard disk if negative
	int	$0x11			// Get equipment configuration
	shl	$1, %ax			// Highest floppy drive $ in bits 6-7
	shl	$1, %ax			// Now in bits 0-1 of ah
	andb	$0x03, %ah		// Extract bits
	cmpb	%ah, %dl		// Must be dl <= ah for drive to exist
	ja	nextdisk		// Otherwise try disk 0 eventually
	call	load0			// Read the next floppy bootstrap
	jc	nextdisk		// It failed, next disk please
	ret				// Jump to the next master bootstrap
nexthd:	call	load0			// Read the hard disk bootstrap
error1:	jc	error			// No disk?
	ret


// Load sector 0 from the current device.  It is either a floppy bootstrap or
// a hard disk master bootstrap.
load0:
	mov	$BUFFER+zero-lowsec, %si	// si = where lowsec(si) is zero
	//jmp	load

// Load sector lowsec(%si) from the current device.  The obvious head, sector,
// and cylinder numbers are ignored in favour of the more trustworthy absolute
// start of partition.
load:
	mov	$3, %di		// Three retries for floppy spinup
retry:	push	%dx		// Save drive code
	push	%es
	push	%di		// Next call destroys es and di
	movb	$0x08, %ah	// Code for drive parameters
	int	$0x13
	pop	%di
	pop	%es
	andb	$0x3F, %cl	// cl = max sector number (1-origin)
	incb	%dh		// dh = 1 + max head number (0-origin)
	movb	%cl, %al	// al = cl = sectors per track
	mulb	%dh		// dh = heads, ax = heads * sectors
	mov	%ax, %bx	// bx = sectors per cylinder = heads * sectors
	mov	lowsec+0(%si), %ax
	mov	lowsec+2(%si), %dx		// dx:ax = sector within drive
	cmp	$[1024*255*63-255]>>16, %dx	// Near 8G limit?
	jae	bigdisk
	div	%bx		// ax = cylinder, dx = sector within cylinder
	xchg	%dx, %ax	// ax = sector within cylinder, dx = cylinder
	movb	%dl, %ch	// ch = low 8 bits of cylinder
	divb	%cl		// al = head, ah = sector (0-origin)
	xorb	%dl, %dl	// About to shift bits 8-9 of cylinder into dl
	shr	$1, %dx
	shr	$1, %dx		// dl[6..7] = high cylinder
	orb	%ah, %dl	// dl[0..5] = sector (0-origin)
	movb	%dl, %cl	// cl[0..5] = sector, cl[6..7] = high cyl
	incb	%cl		// cl[0..5] = sector (1-origin)
	pop	%dx		// Restore drive code in dl
	movb	%al, %dh	// dh = al = head
	mov	$LOADOFF, %bx	// es:bx = where sector is loaded
	mov	$0x0201, %ax	// Code for read, just one sector
	int	$0x13		// Call the BIOS for a read
	jmp	rdeval		// Evaluate read result
bigdisk:
	mov	%dx, %bx	// bx:ax = dx:ax = sector to read
	pop	%dx		// Restore drive code in dl
	push	%si		// Save si
	mov	$BUFFER+ext_rw, %si	// si = extended read/write parameter packet
	mov	%ax, 8(%si)		// Starting block number = bx:ax
	mov	%bx, 10(%si)
	movb	$0x42, %ah	// Extended read
	int	$0x13
	pop	%si		// Restore si to point to partition entry
	//jmp	rdeval
rdeval:
	jnc	rdok		// Read succeeded
	cmpb	$0x80, %ah	// Disk timed out?  (Floppy drive empty)
	je	rdbad
	dec	%di
	jl	rdbad		// Retry count expired
	xorb	%ah, %ah
	int	$0x13		// Reset
	jnc	retry		// Try again
rdbad:	stc			// Set carry flag
	ret
rdok:	cmpw	$0xAA55, LOADOFF+MAGIC
	jne	nosig		// Error if signature wrong
	ret			// Return with carry still clear
nosig:	call	print
	.ascii	"Not bootable.\0"
	jmp	reboot

// A read error occurred, complain and hang
error:
	mov	$LOADOFF+errno+1, %si
prnum:	movb	%ah, %al	// Error number in ah
	andb	$0x0F, %al	// Low 4 bits
	cmpb	$10, %al	// A-F?
	jb	digit		// 0-9//
	addb	$7, %al		// 'A' - ':'
digit:	addb	%al, (%si)	// Modify '0' in string
	dec	%si
	movb	$4, %cl		// Next 4 bits
	shrb	%cl, %ah
	jnz	prnum		// Again if digit > 0
	call	print
	.ascii	"Read error "
errno:	.ascii	"00\0"
	//jmp	reboot

reboot:
	call	print
	.ascii	"\n\rHit any key to reboot.\0"
	xorb	%ah, %ah	// Wait for keypress
	int	$0x16
	call	print
	.ascii	"\r\n\0"
	int	$0x19

// Print a message.
print:	pop	%si		// si = String following 'call print'
prnext:	lodsb			// al = *si++ is char to be printed
	testb	%al, %al		// Null marks end
	jz	prdone
	movb	$0x0E, %ah 	// Print character in teletype mode
	mov	$0x0001, %bx	// Page 0, foreground color
	int	$0x10
	jmp	prnext
prdone:	jmp	*%si		// Continue after the string

.section .data

// Extended read/write commands require a parameter packet.
ext_rw:
	.byte	0x10		// Length of extended r/w packet
	.byte	0		// Reserved
	.word	1		// Blocks to transfer (just one)
	.word	LOADOFF		// Buffer address offset
	.word	0		// Buffer address segment
	.long	0		// Starting block number low 32 bits (tbfi)
zero:	.long	0		// Starting block number high 32 bits
