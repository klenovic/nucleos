/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//      Bootblock 1.5 - Minix boot block.               Author: Kees J. Bot
//                                                              21 Dec 1991
//
// When the PC is powered on, it will try to read the first sector of floppy
// disk 0 at address 0x7C00.  If this fails due to the absence of flexible
// magnetic media, it will read the master boot record from the first sector
// of the hard disk.  This sector not only contains executable code, but also
// the partition table of the hard disk.  When executed, it will select the
// active partition and load the first sector of that at address 0x7C00.
// This file contains the code that is eventually read from either the floppy
// disk, or the hard disk partition.  It is just smart enough to load /boot
// from the boot device into memory at address 0x10000 and execute that.  The
// disk addresses for /boot are patched into this code by installboot as 24-bit
// sector numbers and 8-bit sector counts above enddata upwards.  /boot is in
// turn smart enough to load the different parts of the Minix kernel into
// memory and execute them to finally get Minix started.
//
	.set LOADOFF,	0x7C00		// 0x0000:LOADOFF is where this code is loaded
	.set BOOTSEG,	0x1000		// Secondary boot code segment.
	.set BOOTOFF,	__HDRLEN__	// Offset into /boot above header
	.set BUFFER,	0x0600		// First free memory
	.set LOWSEC,	8		// Offset of logical first sector in partition
					//  table

	// Variables addressed using %bp register
	.set DEVICE,	0	// The boot device
	.set lowsec,	2	// Offset of boot partition within drive
	.set SECPCYL,	6	// Sectors per cylinder = heads * sectors

.section .text

// Start boot procedure.
.globl bootblock_entry
bootblock_entry:
	xor	%ax, %ax	// %ax = 0x0000, the vector segment
	mov	%ax, %ds
	cli			// Ignore interrupts while setting stack
	mov	%ax, %ss	// %ss = %ds = vector segment
	mov	$LOADOFF, %sp	// Usual place for a bootstrap stack
	sti

	push	%ax
	push	%ax		// Push a zero LOWSEC(%bp)

	push	%dx		// Boot device in %dl will be DEVICE(%bp)
	mov	%sp, %bp	// Using var(%bp) is one byte cheaper then var.

	push	%es
	push	%si		// %es:%si = partition table entry if hard %disk

	mov	$LOADOFF+sectors, %di	// char *%di = sectors;

	testb	%dl, %dl		// Winchester disks if %dl >= 0x80
	jge	floppy

winchester:
// Get the offset of the first sector of the boot partition from the partition
// table.  The table is found at %es:%si, the lowsec parameter at offset LOWSEC.
	les	%es:LOWSEC(%si), %ax	// %es:%ax = LOWSEC+2(%si):LOWSEC(%si)
	mov	%ax, lowsec+0(%bp)	// Low 16 bits of partition''s first sector
	mov	%es, lowsec+2(%bp)	// High 16 bits of partition''s first sector

// Get the drive parameters, the number of sectors is bluntly written into the
// floppy disk sectors/track array.

	movb	$0x08, %ah	// Code for drive parameters
	int	$0x13		// %dl still contains drive
	andb	$0x3F, %cl	// %cl = max sector number (1-origin)
	movb	%cl, (%di)	// Number of sectors per track
	incb	%dh		// %dh = 1 + max head number (0-origin)
	jmp	loadboot

// Floppy:
// Execute three read tests to determine the drive type.  Test for each floppy
// type by reading the last sector on the first track.  If it fails, try a type
// that has less sectors.  Therefore we start with 1.44M (18 sectors) then 1.2M
// (15 sectors) ending with 720K/360K (both 9 sectors).

next:
	inc	%di		// Next number of sectors per track

floppy:
	xorb	%ah, %ah	// Reset drive
	int	$0x13
	movb	(%di), %cl	// %cl = number of last sector on track
	cmpb	$9, %cl		// No need to do the last 720K/360K test
	je	success

// Try to read the last sector on track 0
	mov	lowsec(%bp), %es	// %es = vector segment (LOWSEC = 0)
	mov	$BUFFER, %bx		// %es:%bx buffer = 0x0000:0x0600
	mov	$0x0201, %ax		// Read sector, $sectors = 1
	xorb	%ch, %ch		// Track 0, last sector
	xorb	%dh, %dh		// Drive %dl, head 0
	int	$0x13
	jc	next			// Error, try the next floppy type

success:
	movb	$2, %dh			// Load number of heads for multiply

loadboot:
// Load /boot from the boot device
	movb	(%di), %al		// %al = (%di) = sectors per track
	mulb	%dh			// %dh = heads, %ax = heads * sectors
	mov	%ax, SECPCYL(%bp)	// Sectors per cylinder = heads * sectors

	mov	$BOOTSEG, %ax		// Segment to load /boot into
	mov	%ax, %es
	xor	%bx, %bx		// Load first sector at %es:%bx = BOOTSEG:0x0000
	mov	$LOADOFF+addresses, %si	// Start of the boot code addresses
load:
	mov	1(%si), %ax		// Get next sector number: low 16 bits
	movb	3(%si), %dl		// Bits 16-23 for your up to 8GB partition
	xorb	%dh, %dh		// %dx:%ax = sector within partition
	add	lowsec+0(%bp), %ax
	adc	lowsec+2(%bp), %dx	// %dx:%ax = sector within drive
	cmp	$[1024*255*63-255]>>16, %dx	// Near 8G limit?
	jae	bigdisk
	divw	SECPCYL(%bp)	// %ax = cylinder, %dx = sector within cylinder
	xchg	%dx, %ax	// %ax = sector within cylinder, %dx = cylinder
	movb	%dl, %ch	// ch = low 8 bits of cylinder
	divb	(%di)		// %al = head, %ah = sector (0-origin)
	xorb	%dl, %dl	// About to shift bits 8-9 of cylinder into %dl
	shr	$1, %dx
	shr	$1, %dx		// %dl[6..7] = high cylinder
	orb	%ah, %dl	// %dl[0..5] = sector (0-origin)
	movb	%dl, %cl	// %cl[0..5] = sector, %cl[6..7] = high cyl
	incb	%cl		// %cl[0..5] = sector (1-origin)
	movb	%al, %dh	// %dh = %al = head
	movb	DEVICE(%bp), %dl	// %dl = device to read
	movb	(%di), %al		// Sectors per track - Sector number (0-origin)
	subb	%ah, %al	// = Sectors left on this track
	cmpb	(%si), %al	// Compare with $ sectors to read
	jbe	read 		// Ca not read past the end of a cylinder?
	movb	(%si), %al	// (%si) < sectors left on this track
read:
	push	%ax		// Save %al = sectors to read
	movb	$0x02, %ah	// Code for disk read (all registers in use now)
	int	$0x13		// Call the BIOS for a read
	pop	%cx		// Restore %al in %cl
	jmp	rdeval
bigdisk:
	movb	(%si), %cl	// Number of sectors to read
	push	%si		// Save %si
	mov	$LOADOFF+ext_rw, %si	// %si = extended read/write parameter packet
	movb	%cl, 2(%si)		// Fill in $ blocks to transfer
	mov	%bx, 4(%si)		// Buffer address
	mov	%ax, 8(%si)		// Starting block number = %dx:%ax
	mov	%dx, 10(%si) 
	movb	DEVICE(%bp), %dl	// %dl = device to read
	movb	$0x42, %ah		// Extended read
	int	$0x13
	pop	%si			// Restore %si to point to the addresses array
	//jmp   rdeval
rdeval:
	jc	error		// Jump on disk read error
	movb	%cl,%al		// Restore %al = sectors read
	addb	%al,%bh		// %bx += 2 * %al * 256 (add bytes read)
	addb	%al,%bh         // %es:%bx = where next sector must be read
	add	%ax,1(%si)	// Update address by sectors read
	adcb	%ah,3(%si)	// Do not forget bits 16-23 (add %ah = 0)
	subb	%al,(%si)	// Decrement sector count by sectors read
	jnz	load		// Not all sectors have been read
	add	$4,%si		// Next (address, count) pair
	cmpb	(%si),%ah	// Done when no sectors to read
	jnz	load		// Read next chunk of /boot

done:

// Call /boot, assuming a long a.out header (48 bytes).  The a.out header is
// usually short (32 bytes), but to be sure /boot has two entry points:
// One at offset 0 for the long, and one at offset 16 for the short header.
// Parameters passed in registers are:
//
//      %dl     = Boot-device.
//      %es:%si = Partition table entry if hard disk.
//
	pop	%si	// Restore %es:%si = partition table entry
	pop	%es	// %dl is still loaded
	ljmp	$BOOTSEG, $BOOTOFF	// jmp to sec. boot (skipping header).

// Read error: print message, hang forever
error:
	mov	$LOADOFF+errno+1, %si
prnum:
	movb	%ah, %al	// Error number in %ah
	andb	$0x0F, %al	// Low 4 bits
	cmpb	$10, %al	// A-F?
	jb	digit		// 0-9//
	addb	$7, %al		// 'A' - ':'
digit:
	addb	%al, (%si)	// Modify '0' in string
	dec	%si
	movb	$4, %cl			// Next 4 bits
	shrb	%cl, %ah
	jnz	prnum			// Again if digit > 0
	mov	$LOADOFF+rderr, %si	// String to print
print:  lodsb				// %al = *%si++ is char to be printed
	testb	%al, %al		// Null byte marks end
hang:
	jz	hang		// Hang forever waiting for CTRL-ALT-DEL
	movb	$0x0E, %ah	// Print character in teletype mode
	mov	$0x0001, %bx	// Page 0, foreground color
	int	$0x10		// Call BIOS VIDEO_IO
	jmp	print

.section .data
rderr:	.ascii	"Read error "
errno:	.ascii	"00 \0"
errend:

// Floppy disk sectors per track for the 1.44M, 1.2M and 360K/720K types:
sectors:
	.byte	18, 15, 9

// Extended read/write commands require a parameter packet.
ext_rw:
	.byte	0x10		// Length of extended r/w packet
	.byte	0		// Reserved
	.word	0		// Blocks to transfer (to be filled in)
	.word	0		// Buffer address offset (tbfi)
	.word	BOOTSEG		// Buffer address segment
	.long	0		// Starting block number low 32 bits (tbfi)
	.long	0		// Starting block number high 32 bits

	.p2align	1
addresses: .byte 'b','0','0','7'
