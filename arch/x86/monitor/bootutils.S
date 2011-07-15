/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
	.set BOOTOFF,		0x7C00	// 0x0000:BOOTOFF load a bootstrap here
	.set BUFFER,		0x0600	// First free memory
	.set PENTRYSIZE,	16	// Partition table entry size.
	.set K_INT86,		0x0040	// Requires generic INT support
	.set K_MEML,		0x0080	// Pass a list of free memory

	.set DS_SELECTOR,	3*8	// Kernel data selector
	.set ES_SELECTOR,	4*8	// Flat 4 Gb
	.set SS_SELECTOR,	5*8	// Monitor stack
	.set CS_SELECTOR,	6*8	// Kernel code
	.set MCS_SELECTOR,	7*8	// Monitor code

	.set ESC,		0x1B	// Escape character

.section .text
// void exit(int status)
// Exit the monitor by rebooting the system.
.globl exit   // Make various compilers happy
.globl _exit
.globl __exit
exit:
_exit:
__exit:
	push	%bx
	mov	%sp, %bx
	cmp	$0, 6(%bx)        // Good exit status?
	jz	reboot
quit:
	mov	$any_key, %ax
//	push	%ax
//	call	printf
	xorb	%ah, %ah          // Read character from keyboard
	int	$0x16
reboot:
	call	dev_reset
	call	restore_video
	int	$0x19             // Reboot the system

// u32_t mon2abs(void *ptr)
//       Address in monitor data to absolute address.
.globl mon2abs
mon2abs:
	push	%bx
	mov	%sp, %bx
	mov	6(%bx), %ax        // ptr
	mov	%ds, %dx           // Monitor data segment
	pop	%bx
	jmp	seg2abs

// u32_t vec2abs(vector *vec)
//       8086 interrupt vector to absolute address.
.globl vec2abs
vec2abs:
	push	%bx
	mov	%sp, %bx
	mov	6(%bx), %bx // get argument
	mov	(%bx), %ax
	mov	2(%bx), %dx // %dx:ax vector
	pop	%bx
	//jmp	seg2abs         // Translate

// Translate %dx:ax to the 32 bit address %dx-ax
.globl seg2abs
seg2abs:
	push	%cx
	movb	%dh, %ch
	movb	$4, %cl
	shl	%cl, %dx
	shrb	%cl, %ch   // ch-dx = %dx << 4
	add	%dx, %ax
	adcb	$0, %ch    // ch-ax = ch-dx + %ax
	movb	%ch, %dl
	xorb	%dh, %dh   // %dx-ax = ch-ax
	pop	%cx
	/* This part is just for the if the function is called 
	   from C source. The %eax contains the absolute address. */
	shl	$16, %eax
	mov	%dx, %ax
	rol	$16, %eax  // %eax = absolute address
	ret

// Translate the 32 bit address %dx-ax to %dx:ax
.globl abs2seg
abs2seg:
	push	%cx
	movb	%dl, %ch
	mov	%ax, %dx     // ch-dx = %dx-ax
	and	$0x000F, %ax // Offset in %ax
	movb	$4, %cl
	shr	%cl, %dx
	shlb	%cl, %ch
	orb	%ch, %dh     // %dx = ch-dx >> 4
	pop	%cx
	/* This part is just for the if the function is called 
	   from C source. Higher bits of %eax contain the segment
	   and lower 16 bits the offset. */
	shl	$16, %eax
	mov	%dx, %ax
	rol	$16, %eax    // %eax = segment:offset
	ret

// void raw_copy(u32_t dstaddr, u32_t srcaddr, u32_t count)
//       Copy count bytes from srcaddr to dstaddr.  Do not do overlaps.
//       Also handles copying words to or from extended memory.
/*	In case of using .code16gcc the arguments and return address are 
	pushed on stack which is 4 bytes aligned !!! Minix compiler aligns stack
	for 16-bit code to 2 bytes. In case the argument is 4 bytes long the it 
	is divided into two parts where the lower word is closer to the top of 
	the stack e.g. for this function:

	Minix3 stack             gcc with .code16gcc stack
	  2 bytes                     4 bytes
	+----------+                +-----------+
	|  countH  |                |   count   |
	+----------+ 12(%sp)        +-----------+ 12(%sp)
	|  countL  |                |  srcadr   |
	+----------+ 10(%sp)        +-----------+  8(%sp)
	| srcaddrH |                |  dstaddr  |
	+----------+  8(%sp)        +-----------+  4(%sp)
	| srcaddrL |                |    RA     |
	+----------+  6(%sp)        +-----------+   (%sp)
	| dstaddrH |
	+----------+  4(%sp)
	| dstaddrL |
	+----------+  2(%sp)
	|    RA    |
	+----------+   (%sp)

	Note that also in case of .code16gcc just using push/pop will align 
	the stack to two bytes until you are using pushl/popl or push a 4-byte 
	long argument e.g. register. */

/*		  code16gcc | orig. minix3 */
#define countH		16 // 14
#define countL		14 // 12
#define srcaddrH	12 // 10
#define srcaddrL	10 //  8
#define dstaddrH	8  //  6
#define dstaddrL	6  //  4

.globl raw_copy
raw_copy:
	push	%bp			// Note that also in case of .code16gcc 
					// the stack is 2 bytes aligned 
	mov	%sp, %bp
	push	%bx
	push	%si
	push	%di			// Save C variable registers
copy:
	cmp	$0, countH(%bp)
	jnz	bigcopy
	mov	countL(%bp), %cx
	cmp	$0, %cx			// Count is zero, end copy
	jz	copydone
	cmp	$0xFFF0, %cx
	jb	smallcopy
bigcopy:
	mov	$0xFFF0, %cx           // Do not copy more than about 64K at once
smallcopy:
	push	%cx                    // Save copying count
	mov	dstaddrL(%bp), %ax
	mov	dstaddrH(%bp), %dx
	cmp	$0x0010, %dx           // Copy to extended memory?
	jae	ext_copy
	cmp	$0x0010, srcaddrH(%bp) // Copy from extended memory?
	jae	ext_copy
	call	abs2seg
	mov	%ax, %di
	mov	%dx, %es     // es:di = dstaddr
	mov	srcaddrL(%bp), %ax
	mov	srcaddrH(%bp), %dx
	call	abs2seg
	mov	%ax, %si 
	mov	%dx, %ds     // %ds:%si = srcaddr
	shr	$1, %cx      // Words to move
	rep	
	movsw	             // Do the word copy
	adc	%cx, %cx     // One more byte?
	rep
	movsb                // Do the byte copy
	mov	%ss, %ax     // Restore %ds and %es from the remaining %ss
	mov	%ax, %ds
	mov	%ax, %es
	jmp	copyadjust
ext_copy:
	mov	%ax, x_dst_desc+2
	movb	%dl, x_dst_desc+4  // Set base of destination segment
	movb	%dh, x_dst_desc+7  // and the highest part of base
	mov	srcaddrL(%bp), %ax
	mov	srcaddrH(%bp), %dx
	mov	%ax, x_src_desc+2
	movb	%dl, x_src_desc+4  // Set base of source segment
	movb	%dh, x_src_desc+7  // and the highest part of base
	mov	$x_gdt, %si        // es:%si = global descriptor table
	shr	$1, %cx            // Words to move
	movb	$0x87, %ah         // Code for extended memory move
	int	$0x15
copyadjust:
	pop	%cx                // Restore count
	add	%cx, dstaddrL(%bp)
	adc	$0, dstaddrH(%bp)  // srcaddr += copycount
	add	%cx, srcaddrL(%bp)
	adc	$0, srcaddrH(%bp)  // dstaddr += copycount
	sub	%cx, countL(%bp)
	sbb	$0, countH(%bp)    // count -= copycount
	jmp	copy               // and repeat
copydone:
	pop	%di
	pop	%si                // Restore C variable register
	pop	%bx
	pop	%bp
	ret

// u16_t get_word(u32_t addr);
// void put_word(u32_t addr, u16_t word);
//       Read or write a 16 bits word at an arbitrary location.
/*
	---> stack grows --->
	------------+---2b----+----2b----+-2b-+-2b--+
	stack minix | addrH   |   addrL  | RA | %bx |
	------------+---------+----------+----+-----+
	  offset    |         8          4    2     0

	------------+---2b----+----2b----+-4b-+-2b--+
	stack minix | addrH   |   addrL  | RA | %bx |
	------------+---------+----------+----+-----+
	  offset    |         8          6    2     0
 */
.globl get_word
get_word:
	push	%bx
	mov	%sp, %bx
	call	gp_getaddr     // this puts 4 byte RA in case of .code16gcc
	mov	%ds:(%bx), %ax // Word to get from addr
	jmp	gp_ret

/*
	---> stack grows --->
	------------+---2b----+----2b----+----2b----+-2b-+-2b--+
	stack minix |  word   |   addrH  |   addrL  | RA | %bx |
	------------+---------+----------+----------+----+-----+
	  offset    |         8          6          4    2     0

	------------+---4b----+----2b----+----2b----+-4b-+-2b--+
	stack minix |  word   |   addrH  |   addrL  | RA | %bx |
	------------+---------+----------+----------+----+-----+
	  offset    |         10         8          6    2     0
 */
.globl put_word
put_word:
	push	%bx
	mov	%sp, %bx     // (0)%bx, (2)RA, (6)word, (10)addrL, (12)addrH
	pushw	10(%bx)      // Word to store at addr
	call	gp_getaddr   // this puts 4 byte RA in case of .code16gcc
	popw	(%bx)        // Store the word
	jmp	gp_ret
gp_getaddr:
	mov	6(%bx), %ax  // low word
	mov	8(%bx), %dx  // high word
	call	abs2seg
	mov	%ax, %bx
	mov	%dx, %ds      // %ds:bx = addr
	ret
gp_ret:
	push	%es
	pop	%ds           // Restore %ds
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret

// void relocate(void);
//       After the program has copied itself to a safer place, it needs to change
//       the segment registers.  Caddr has already been set to the new location.
.globl relocate
relocate:
	pop	%ecx         // Return address
	shl	$16, %ecx     //  save it at higher word
	mov	caddr+0, %ax
	mov	caddr+2, %dx
	call	abs2seg    // Be sure that it does not discard %ecx
	mov	%dx, %cx   // cx = new code segment
	mov	%cs, %ax   // Old code segment
	sub	%cx, %ax   // ax = -(new - old) = -Moving offset
	mov	%ds, %dx
	sub	%ax, %dx
	mov	%dx, %ds   // ds += (new - old)
	mov	%dx, %es
	mov	%dx, %ss
	xor	%ax, %ax
	call	seg2abs    // Be sure that it does not discard %ecx
	mov	%ax, daddr+0
	mov	%dx, daddr+2 // New data address
	push	%cx          // New text segment
	shr	$16, %ecx    // get the saved return address
	push	%cx          // Return offset of this function
	lretw	             // Relocate

// void* brk(void *addr)
// void* sbrk(size_t incr)
//       Cannot fail implementations of brk(2) and sbrk(3), so we can use
//       malloc(3).  They reboot on stack collision instead of returning -1.
.globl brk
.globl _brk
.globl sbrk
.globl _sbrk
brk:
_brk:                      // __brk is for the standard C compiler
	push	%bx
	xor	%ax, %ax
	jmp	.Lsbrk           // break= 0; return sbrk(addr);
sbrk:
_sbrk:
	push	%bx
	mov	break, %ax       // %ax= current break
.Lsbrk:
	push	%ax
	mov	%sp, %bx         // Stack is now: ((0)%ax, (2)%bx, (4)retaddr,(8)incr, ...)
	add	8(%bx), %ax      // %ax= break + increment
	mov	%ax, break       // Set new break
	lea	-1024(%bx), %dx  // sp minus a bit of breathing space
	cmp	%ax, %dx         // Compare with the new break
	jb	heaperr          // Suffocating noises
	lea	-4096(%bx), %dx  // A warning when heap+stack goes < 4K
	cmp	%ax, %dx
	jae	plenty           // No reason to complain

/* @nucleos: `printf' doesn't work from asm. for now thus no
	          warnings here
	mov	$0xbadc0de1,%eax
1:
	jmp	1b               // Hang on
	mov	$memwarn, %ax
	push	%ax
	call	printf         // Warn about memory running low
	pop	%ax
	movb	$0, memwarn      // No more warnings
*/
plenty:
	pop	%ax              // Return old break (0 for brk)
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret
heaperr:
	mov	$0xbadc0de2, %eax
1:
	jmp	1b            // Hang on
	pop	%bx
/* @nucleos: `printf' doesn't work from asm. for now
	mov	$chmem, %ax
	push	%ax
	mov	$nomem, %ax
	push	%ax
	call	printf
*/
	jmp	quit

// int dev_open(void);
//       Given the device "device" figure out if it exists and what its number
//       of heads and sectors may be.  Return the BIOS error code on error,
//       otherwise 0.
.globl dev_open
dev_open:
	push	%bx
	call	dev_reset         // Optionally reset the disks
	movb	$0, dev_state     // State is "closed"
	push	%es
	push	%di               // Save registers used by BIOS calls
	movb	device, %dl       // The default device
	cmpb	$0x80, %dl        // Floppy < 0x80, winchester >= 0x80
	jae	winchester
floppy:
	mov	$3, %di           // Three tries to init drive by reading sector 0
finit0:
	xor	%ax, %ax
	mov	%ax, %es
	mov	$BUFFER, %bx      // es:bx = scratch buffer
	mov	$0x0201, %ax      // Read sector, $sectors = 1
	mov	$0x0001, %cx      // Track 0, first sector
	xorb	%dh, %dh          // Drive %dl, head 0
	int	$0x13
	jnc	finit0ok          // Sector 0 read ok?
	cmpb	$0x80, %ah        // Disk timed out?  (Floppy drive empty)
	je	geoerr
	dec	%di
	jz	geoerr
	xorb	%ah, %ah          // Reset drive
	int	$0x13
	jc	geoerr
	jmp	finit0            // Retry once more, it may need to spin up
finit0ok:
	mov	$seclist, %di     // List of per floppy type sectors/track
flast:
	movb	(%di), %cl        // Sectors per track to test
	cmpb	$9, %cl           // No need to do the last 720K/360K test
	je	ftestok
	xor	%ax, %ax
	mov	%ax, %es
	mov	$BUFFER, %bx      // es:bx = scratch buffer
	mov	$0x0201, %ax      // Read sector, $sectors = 1
	xorb	%ch, %ch          // Track 0, last sector
	xorb	%dh, %dh          // Drive %dl, head 0
	int	$0x13
	jnc	ftestok           // Sector cl read ok?
	xorb	%ah, %ah          // Reset drive
	int	$0x13
	jc	geoerr
	inc	%di               // Try next sec/track number
	jmp	flast
ftestok:
	movb	$2, %dh           // Floppies have two sides
	jmp	geoboth
winchester:
	movb	$0x08, %ah    // Code for drive parameters
	int	$0x13         // dl still contains drive
	jc	geoerr        // No such drive?
	andb	$0x3F, %cl    // cl = max sector number (1-origin)
	incb	%dh           // dh = 1 + max head number (0-origin)
geoboth:
	movb	%cl, sectors  // Sectors per track
	movb	%cl, %al      // al = sectors per track
	mulb	%dh           // %ax = heads * sectors
	mov	%ax, secspcyl // Sectors per cylinder = heads * sectors
	movb	$1, dev_state // Device state is "open"
	xor	%ax, %ax      // Code for success
geodone:
	pop	%di
	pop	%es           // Restore %di and %es registers
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret
geoerr:
	movb	%al, %ah
	xorb	%ah, %ah          // %ax = BIOS error code
	jmp	geodone

// int dev_close(void);
//       Close the current device.  Under the BIOS this does nothing much.
.globl dev_close
dev_close:
	xor	%eax, %eax
	movb	%al, dev_state    // State is "closed"
	ret

// Reset the disks if needed.  Minix may have messed things up.
dev_reset:
	cmpb	$0, dev_state    // Need reset if dev_state < 0
	jge	0f
	xorb	%ah, %ah          // Reset (ah = 0)
	movb	$0x80, %dl        // All disks
	int	$0x13
	movb	$0, dev_state    // State is "closed"
0:
	ret

// int dev_boundary(u32_t sector);
//       True if a sector is on a boundary, i.e. sector % sectors == 0.
.globl dev_boundary
dev_boundary:
	push	%bx
	mov	%sp, %bx
	xor	%dx, %dx
	mov	8(%bx), %ax  // divide high half of sector number
	divw	sectors
	mov	6(%bx), %ax  // divide low half of sector number
	divw	sectors      // %dx = sector % sectors
	sub	$1, %dx      // CF = %dx == 0
	sbb	%ax, %ax     // %ax = -CF
	neg	%ax          // %ax = (sector % sectors) == 0
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret

// int readsectors(u32_t bufaddr, u32_t sector, u8_t count)
//       Read several sectors fromo disk or floppy.  The buffer must
//       be between 64K boundaries//  Count must fit in a byte.  The external
//       variables device, sectors and secspcyl describe the disk and its
//       geometry.  Returns 0 for success, otherwise the BIOS error code.
//
/*
	---> stack grows --->
	------------+---2b--+---2b----+---2b----+----2b----+---2b-----+-2b-+-2b--+
	stack minix | count | sectorH | sectorL | bufaddrH | bufaddrL | RA | %bp |
	------------+-------+---------+---------+----------+----------+----+-----+
	  offset    |       12        10        8          6          4    2     0


	------------+--4b---+---------4b--------+----------4b---------+-4b-+-2b--+
	  code16gcc | count | sectorH | sectorL | bufaddrH | bufaddrL | RA | %bp |
	------------+-------+-------------------+---------------------+----+-----+
	   offset   |       14        12        10         8          6    2     0
 */

#define BUFFL	6	//  4
#define BUFFH	8	//  6
#define SECTL	10	//  8
#define SECTH	12	// 10
#define COUNT	14	// 12
#define DSKRW	15	// 13

.globl readsectors
readsectors:
	push	%bp
	mov	%sp, %bp
	movb	$0x02, DSKRW(%bp) // Code for a disk read
rwsec:
	push	%bx
	push	%si
	push	%di
	push	%es
	cmpb	$0, dev_state  // Device state?
	jg	0f             // >0 if open
	call	dev_open       // Initialize
	test	%ax, %ax
	jnz	badopen
0:
	mov	BUFFL(%bp), %ax  // buffaddrL
	mov	BUFFH(%bp), %dx  // buffaddrH
	call	abs2seg
	mov	%ax, %bx
	mov	%dx, %es       // es:bx = bufaddr
	mov	$3, %di        // Execute 3 resets on floppy error
	cmpb	$0x80, device
	jb	nohd
	mov	$1, %di        // But only 1 reset on hard disk error
nohd:
	cmpb	$0, COUNT(%bp) // count equals zero?
	jz	done
more:
	mov	SECTL(%bp), %ax  // sectorL
	mov	SECTH(%bp), %dx  // sectorH, %dx:ax = abs sector.
	                         //  Divide it by sectors/cyl
	cmp	$[1024*255*63-255]>>16, %dx  // Near 8G limit?
	jae	bigdisk
	divw	secspcyl    // %ax = cylinder, %dx = sector within cylinder
	xchg	%dx, %ax     // %ax = sector within cylinder, %dx = cylinder
	movb	%dl, %ch     // ch = low 8 bits of cylinder
	divb	sectors      // al = head, %ah = sector (0-origin)
	xorb	%dl, %dl     // About to shift bits 8-9 of cylinder into dl
	shr	$1, %dx
	shr	$1, %dx      // dl[6..7] = high cylinder
	orb	%ah,%dl      // dl[0..5] = sector (0-origin)
	movb	%dl, %cl     // cl[0..5] = sector, %cl[6..7] = high cyl
	incb	%cl          // cl[0..5] = sector (1-origin)
	movb	%al, %dh     // dh = head
	movb	device, %dl  // dl = device to use
	movb	sectors, %al // Sectors per track - Sector number (0-origin)
	subb	%ah, %al     // = Sectors left on this track
	cmpb	COUNT(%bp), %al // Compare with $ sectors to transfer
	jbe	doit            // Can not go past the end of a cylinder?
	movb	COUNT(%bp), %al // 12(%bp) < sectors left on this track
doit:
	movb	DSKRW(%bp), %ah // Code for disk read (0x02) or write (0x03)
	push	%ax             // Save al = sectors to read
	int	$0x13           // call the BIOS to do the transfer
	pop	%cx             // Restore al in cl
	jmp	rdeval
bigdisk:
	mov	$ext_rw, %si  // %si = extended read/write parameter packet
	movb	COUNT(%bp), %cl
	movb	%cl, 2(%si)   // Fill in $ blocks to transfer
	mov	%bx, 4(%si)   // Buffer address = es:bx
	mov	%es, 6(%si)
	mov	%ax, 8(%si)   // Starting block number = %dx:ax
	mov	%dx, 10(%si)
	movb	device, %dl   // dl = device to use
	mov	$0x4000, %ax  // This, or-ed with 0x02 or 0x03 becomes
	orb	DSKRW(%bp), %ah  // extended read (0x4200) or write (0x4300)
	int	$0x13
	//jmp	rdeval
rdeval:
	jc	ioerr         // I/O error
	movb	%cl, %al      // Restore al = sectors read
	addb	%al, %bh      // %bx += 2 * al * 256 (add bytes transferred)
	addb	%al, %bh      // es:bx = where next sector is located
	add	%ax, SECTL(%bp)  // Update address by sectors transferred
	adc	$0, SECTH(%bp)   // Do not forget high word
	subb	%al, COUNT(%bp)  // Decrement sector count by sectors transferred
	jnz	more          // Not all sectors have been transferred
done:
	xorb	%ah, %ah      // No error here//
	jmp	finish
ioerr:
	cmpb	$0x80, %ah    // Disk timed out?  (Floppy drive empty)
	je	finish
	cmpb	$0x03, %ah    // Disk write protected?
	je	finish
	dec	%di           // Do we allow another reset?
	jl	finish        // No, report the error
	xorb	%ah, %ah      // Code for a reset (0)
	int	$0x13
	jnc	more          // Succesful reset, try request again
finish:
	movb	%ah, %al
	xorb	%ah, %ah      // %ax = error number
	and	$0xffff, %eax // Since function can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
badopen:
	pop	%es
	pop	%di
	pop	%si
	pop	%bx
	pop	%bp
	ret

// int putch(int c);
//       Write a character in teletype mode.  The putk synonym is
//       for the kernel printf function that uses it.
//       Newlines are automatically preceded by a carriage return.
//
.globl putch
putch:

.globl putk
putk:
	push	%bx
	mov	%sp, %bx
	movb	6(%bx), %al     // al = character to be printed
	testb	%al, %al        // Kernel printf adds a null char to flush queue
	jz	nulch
	pop	%bx             // restore %bx
	cmpb	$0x0A, %al      // al = newline?
	jnz	putc
	movb	$0x0D, %al
	call	putc            // putc('\r')
	movb	$0x0A, %al      // Restore the '\n' and print it
putc:
	push	%bx
	movb	$0x0E, %ah      // Print character in teletype mode
	mov	$0x0001, %bx    // Page 0, foreground color
	int	$0x10
	mov	line, %bx       // Serial line?
	test	%bx, %bx
	jz	nulch
	push	%ax             // Save character to print
	call	get_tick        // Current clock tick counter
	mov	%ax, %cx
	add	$2, %cx         // Do not want to see it count twice
1:
	lea	5(%bx), %dx     // Line Status Register
	inb	%dx
	testb	$0x20, %al      // Transmitter Holding Register Empty?
	jnz	0f
	call	get_tick
	cmp	%cx, %ax        // Clock ticked more than once?
	jne	1b
0:
	pop	%ax             // Restore character to print
	mov	%bx, %dx        // Transmit Holding Register
	outb	%dx             // Send character down the serial line
nulch:
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret

// void set_mode(unsigned mode);
//      Set video mode.
.globl set_mode
set_mode:
	push	%bx
	mov	%sp, %bx
	mov	6(%bx), %ax        // Video mode
	pop	%bx
	cmp	cur_vid_mode, %ax
	je	modeok             // Mode already as requested?
	mov	%ax, cur_vid_mode

// void clear_screen(void);
//      Clear the screen.
.globl clear_screen
clear_screen:
	push	%bx
	xor	%ax, %ax
	mov	%ax, %es           // %es = Vector segment
	mov	cur_vid_mode, %ax
	movb	%ah, %ch           // Copy of the special flags
	andb	$0x0F, %ah         // Test bits 8-11, clear special flags
	jnz	xvesa              // VESA extended mode?
	int	$0x10              // Reset video (ah = 0)
	jmp	md_480
xvesa:
	mov	%ax, %bx          // %bx = extended mode
	mov	$0x4F02, %ax      // Reset video
	int	$0x10
md_480:                     // Basic video mode is set, now build on it
	testb	$0x20, %ch        // 480 scan lines requested?
	jz	md_14pt
	mov	$0x3CC, %dx       // Get CRTC port
	inb	%dx
	movb	$0xD4, %dl
	testb	$1, %al           // Mono or color?
	jnz	0f
	movb	$0xB4, %dl
0:
	mov	$0x110C, %ax      // Vertical sync end (also unlocks CR0-7)
	call	out2
	mov	$0x060B, %ax      // Vertical total
	call	out2
	mov	$0x073E, %ax      // (Vertical) overflow
	call	out2
	mov	$0x10EA, %ax      // Vertical sync start
	call	out2
	mov	$0x12DF, %ax      // Vertical display end
	call	out2
	mov	$0x15E7, %ax      // Vertical blank start
	call	out2
	mov	$0x1604, %ax      // Vertical blank end
	call	out2
	push	%dx
	movb	$0xCC, %dl        // Misc output register (read)
	inb	%dx
	movb	$0xC2, %dl        // (write)
	andb	$0x0D, %al        // Preserve clock select bits and color bit
	orb	$0xE2, %al        // Set correct sync polarity
	outb	%dx
	pop	%dx              // Index register still in %dx
md_14pt:
	testb	$0x40, %ch        // 9x14 point font requested?
	jz	md_8pt
	mov	$0x1111, %ax      // Load ROM 9 by 14 font
	xorb	%bl, %bl          // Load block 0
	int	$0x10
	testb	$0x20, %ch        // 480 scan lines?
	jz	md_8pt
	mov	$0x12DB, %ax      // VGA vertical display end
	call	out2
	movb	$33, %es:0x0484      // Tell BIOS the last line number
md_8pt:
	testb	$0x80, %ch        // 8x8 point font requested?
	jz	setcur
	mov	$0x1112, %ax      // Load ROM 8 by 8 font
	xorb	%bl, %bl          // Load block 0
	int	$0x10
	testb	$0x20, %ch        // 480 scan lines?
	jz	setcur
	mov	$0x12DF, %ax      // VGA vertical display end
	call	out2
	movb	$59, %es:0x0484      // Tell BIOS the last line number
setcur:
	xor	%dx, %dx          // dl = column = 0, %dh = row = 0
	xorb	%bh, %bh          // Page 0
	movb	$0x02, %ah        // Set cursor position
	int	$0x10
	push	%ss
	pop	%es              // Restore es
	pop	%bx
modeok:
	ret

// Out to the usual [index, data] port pair that are common for VGA devices
// %dx = port, %ah = index, %al = data.
out2:
	push	%dx
	push	%ax
	movb	%ah, %al
	outb	%dx              // Set index
	inc	%dx
	pop	%ax
	outb	%dx              // Send data
	pop	%dx
	ret

restore_video:                  // To restore the video mode on exit
	movl	old_vid_mode, %eax
	pushl	%eax
	call	set_mode
	popl	%eax
	ret

// void serial_init(int line)
//       Initialize copying console I/O to a serial line.
.globl serial_init
serial_init:
	push	%bx
	mov	%sp, %bx
	mov	6(%bx), %dx        // Line number
	movw	$0, line
	test	%dx, %dx           // Off if line number < 0
	js	0f
	push	%ds
	xor	%ax, %ax
	mov	%ax, %ds           // Vector and BIOS data segment
	mov	%dx, %bx           // Line number
	shl	$1, %bx            // Word offset
	mov	0x0400(%bx), %bx   // I/O port for the given line
	pop	%ds
	mov	%bx, line          // Remember I/O port
	jmp	1f
serial_init_realmode:            // called from real mode
	push	%bx
1:
	mov	line, %bx
	test	%bx, %bx          // I/O port must be nonzero
	jz	0f
	mov	$0x00E3, %ax      // 9600 N-8-1
	int	$0x14             // Initialize serial line %dx
	lea	4(%bx), %dx       // Modem Control Register
	movb	$0x0B, %al        // DTR, RTS, OUT2
	outb	%dx
0:
	pop	%bx
	ret

// u32_t get_tick(void);
//       Return the current value of the clock tick counter.  This counter
//       increments 18.2 times per second.  Poll it to do delays.  Does not
//       work on the original PC, but works on the PC/XT.
.globl get_tick
get_tick:
	push	%cx
	xorb	%ah, %ah  // Code for get tick count
	int	$0x1A
	mov	%dx, %ax
	mov	%cx, %dx  // %dx:ax = %cx:dx = tick count
	pop	%cx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret

// Functions used to obtain info about the hardware.  Boot uses this information
// itself, but will also pass them on to a pure 386 kernel, because one ca not
// make BIOS calls from protected mode.  The video type could probably be
// determined by the kernel too by looking at the hardware, but there is a small
// chance on errors that the monitor allows you to correct by setting variables.

.globl get_bus                // returns type of system bus
// u16_t get_bus(void)
//       Return type of system bus, in order: XT, AT, MCA.
get_bus:
	push	%bx
	call	getprocessor
	xor	%dx, %dx        // Assume XT
	cmp	$286, %ax       // An AT has at least a 286
	jb	got_bus
	inc	%dx             // Assume AT
	movb	$0xC0,%ah       // Code for get configuration
	int	$0x15
	jc	got_bus         // Carry clear and ah = 00 if supported
	testb	%ah, %ah
	jne	got_bus
	movb	%es:5(%bx), %al  // Load feature byte $1
	inc	%dx              // Assume MCA
	testb	$0x02, %al       // Test bit 1 - "bus is Micro Channel"
	jnz	got_bus
	dec	%dx              // Assume AT
	testb	$0x40, %al       // Test bit 6 - "2nd 8259 installed"
	jnz	got_bus
	dec	%dx              // It is an XT
got_bus:
	push	%ds
	pop	%es              // Restore es
	mov	%dx, %ax         // Return bus code
	mov	%ax, bus         // Keep bus code, A20 handler likes to know
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret

.globl get_video              // returns type of display
// u16_t get_video(void)
//       Return type of display, in order: MDA, CGA, mono EGA, color EGA,
//       mono VGA, color VGA.
get_video:
	push	%bx          // save this, C code may depends on it
	mov	$0x1A00, %ax // Function 1A returns display code
	int	$0x10        // al = 1A if supported
	cmpb	$0x1A, %al
	jnz	no_dc        // No display code function supported
	mov	$2, %ax
	cmpb	$5, %bl      // Is it a monochrome EGA?
	jz	got_video
	inc	%ax
	cmpb	$4, %bl      // Is it a color EGA?
	jz	got_video
	inc	%ax
	cmpb	$7, %bl      // Is it a monochrome VGA?
	jz	got_video
	inc	%ax
	cmpb	$8, %bl      // Is it a color VGA?
	jz	got_video
no_dc:
	movb	$0x12, %ah   // Get information about the EGA
	movb	$0x10, %bl
	int	$0x10
	cmpb	$0x10, %bl   // Did it come back as 0x10? (No EGA)
	jz	no_ega
	mov	$2, %ax
	cmpb	$1, %bh      // Is it monochrome?
	jz	got_video
	inc	%ax
	jmp	got_video
no_ega:
	int	$0x11        // Get bit pattern for equipment
	and	$0x30, %ax   // Isolate color/mono field
	sub	$0x30, %ax
	jz	got_video    // Is it an MDA?
	mov	$1, %ax      // No it is CGA
got_video:
	pop	%bx
	and	$0xffff, %eax // Since function (or mon2abs) can be called
	                      // from C source compiled by .code16gcc
	                      // which checks %eax not just %ax. This will
	                      // cleanup the garbage from higher bits.
	ret


// Functions to leave the boot monitor.
.globl bootstrap              // Call another bootstrap
// void _bootstrap(int device, struct part_entry *entry)
//       Call another bootstrap routine to boot MS-DOS for instance.  (No real
//       need for that anymore, now that you can format floppies under Minix).
//       The bootstrap must have been loaded at BOOTSEG from "device".
bootstrap:
	push	%bx
	call	restore_video
	mov	%sp, %bx
	movb	6(%bx), %dl        // Device to boot from
	mov	10(%bx), %si        // %ds:%si = partition table entry
	xor	%ax, %ax
	mov	%ax, %es           // Vector segment
	mov	$BUFFER, %di      // es:di = buffer in low core
	mov	$PENTRYSIZE, %cx  // %cx = size of partition table entry
	rep	movsb                   // Copy the entry to low core
	mov	$BUFFER, %si      // es:%si = partition table entry
	mov	%ax, %ds           // Some bootstraps need zero segment registers
	cli
	mov	%ax, %ss
	mov	$BOOTOFF, %sp     // This should do it
	sti
	pop	%bx
	ljmp	$0,$BOOTOFF         // Back to where the BIOS loads the boot code

.globl minix                  // Call Minix
// void minix(u32_t koff, u32_t kcs, u32_t kds,
//            char *bootparams, size_t paramsize, u32_t aout);
//       Call Minix.
/*
	---> stack grows --->
	------------+--2b---+--2b---+-----2b-----+-----2b------+---2b----+---2b----+---2b-----+---2b-----+---2b-----+---2b-----+-2b-+--2b-+
	stack minix | aoutH | aoutL | paramsize  | bootparams  |  kdsH   |  kdsL   |  kcsH    |  kcsL    |   koffH  |   koffL  | RA | %bp |
	------------+-------+-------+------------+-------------+---------+---------+----------+----------+----------+----------+----+-----+
	  offset    |       22      20           18            16        14        12         10         8          6          4    2     0

	------------+--2b---+--2b---+-----4b-----+-----4b------+---2b----+---2b----+---2b-----+---2b-----+---2b-----+---2b-----+-4b-+--2b-+--2b-+
	  code16gcc | aoutH | aoutL | paramsize  | bootparams  |  kdsH   |  kdsL   |  kcsH    |  kcsL    |   koffH  |   koffL  | RA | %bx | %bp |
	------------+-------+-------+------------+-------------+---------+---------+----------+----------+----------+----------+----+--2b-+--2b-+
	  offset    |       30      28           24            20        18        16         14         12         10         8    4     2     0
 */
/*		code16gcc | minix3 */
#define KOFFL	8	 //  4
#define KOFFH	10	 //  6
#define KCSL	12	 //  8
#define KCSH	14	 // 10
#define KDSL	16	 // 12
#define KDSH	18	 // 14
#define BOOTP	20	 // 16
#define PARAMS	24	 // 18
#define AOUTL	28	 // 20
#define AOUTH	30	 // 22

ebxval:	.long 0

minix:
	push	%bx
	push	%bp
	mov	%sp, %bp          // Pointer to arguments
	mov	$0x03F2, %dx      // Floppy motor drive control bits
	movb	$0x0C, %al        // Bits 4-7 for floppy 0-3 are off
	outb	%dx               // Kill the motors
	push	%ds
	xor	%ax, %ax          // Vector & BIOS data segments
	mov	%ax, %ds
	andb	$0xF0,0x043F      // Clear diskette motor status bits of BIOS
	pop	%ds
	cli                       // No more interruptions
	mov	%cs, %cs:cs_real-2     // Patch CS and DS into the instructions that
	mov	%ds, %cs:ds_real-2     // reload them when switching back to real mode
	mov	%cr0, %eax
	orb	$0x01, %al         // Set PE (protection enable) bit
	movl	%eax, msw          // Save as protected mode machine status word
	mov	%ds, %dx           // Monitor %ds
	mov	$p_gdt, %ax        // %dx:ax = Global descriptor table
	call	seg2abs
	mov	%ax, p_gdt_desc+2
	movb	%dl, p_gdt_desc+4  // Set base of global descriptor table

	mov	KDSL(%bp), %ax
	mov	KDSH(%bp), %dx     // Kernel %ds (absolute address)

	mov	%ax, p_ds_desc+2
	movb	%dl, p_ds_desc+4   // Set base of kernel data segment

	mov	%ss, %dx           // Monitor ss
	xor	%ax, %ax           // %dx:ax = Monitor stack segment
	call	seg2abs            // Minix starts with the stack of the monitor
	mov	%ax, p_ss_desc+2
	movb	%dl, p_ss_desc+4

	mov	KCSL(%bp), %ax
	mov	KCSH(%bp), %dx     // Kernel cs (absolute address)
	mov	%ax, p_cs_desc+2
	movb	%dl, p_cs_desc+4

	mov	%cs, %dx           // Monitor cs
	xor	%ax, %ax           // %dx:ax = Monitor code segment
	call	seg2abs
	mov	%ax, p_mcs_desc+2
	movb	%dl, p_mcs_desc+4

	push	$MCS_SELECTOR
	testw	$K_INT86, k_flags  // Generic INT86 support?
	jz	0f
	push	$int86             // Far address to INT86 support
	jmp	1f
0:
	pushw	$bios13            // Far address to BIOS int 13 support
1:
	testw	$K_MEML, k_flags   // New memory arrangements?
	jz	0f
	pushl	AOUTL(%bp)    // Address of a.out headers
0:
	pushw	$0
	pushw	PARAMS(%bp)  // 32 bit size of parameters on stack
	pushw	$0
	pushw	BOOTP(%bp)   // 32 bit address of parameters (%ss relative)
	pushw	$0
	pushw	$0		// monitor far return address "Monitor CS_SEGMENT:ret386"
	pushw	$0		// @nucleos: monitor never entered on reboot/halt
	pushw	$CS_SELECTOR
	pushw	KOFFH(%bp)
	pushw	KOFFL(%bp)       // 32 bit far address to kernel entry point

	call	real2prot         // Switch to protected mode
	mov	$DS_SELECTOR, %ax // Kernel data
	mov	%ax, %ds
	mov	$ES_SELECTOR, %ax // Flat 4 Gb
	mov	%ax, %es
	lretl                     // Make a far call to the kernel
// Minix-386 returns here on a halt or reboot.
ret386:
	movl	%eax, reboot_code // Return value (obsolete method)
	call	prot2real         // Switch to real mode

return:
	mov	%bp, %sp    // Pop parameters
	sti                 // Can take interrupts again
	call	get_video   // MDA, CGA, EGA, ...

	movb	$24, %dh    // dh = row 24
	cmp	$2, %ax     // At least EGA?
	jb	is25        // Otherwise 25 rows
	push	%ds
	xor	%ax, %ax    // Vector & BIOS data segments
	mov	%ax, %ds
	movb	0x0484, %dh // Number of rows on display minus one
	pop	%ds
is25:
	xorb	%dl, %dl    // dl = column 0
	xorb	%bh, %bh    // Page 0
	movb	$0x02, %ah  // Set cursor position
	int	$0x10
	movb	$-1, dev_state       // Minix may have upset the disks, must reset.
	call	serial_init_realmode // Likewise with our serial console
	call	getprocessor
	cmp	$286, %ax
	jb	noclock
	xorb	%al, %al
tryclk:
	decb	%al
	jz	noclock
	movb	$0x02, %ah      // Get real-time clock time (from CMOS clock)
	int	$0x1A
	jc	tryclk          // Carry set, not running or being updated
	movb	%ch, %al        // ch = hour in BCD
	call	bcd             // al = (al >> 4) * 10 + (al & 0x0F)
	mulb	c60             // 60 minutes in an hour
	mov	%ax, %bx        // %bx = hour * 60
	movb	%cl, %al        // cl = minutes in BCD
	call	bcd
	add	%ax, %bx        // %bx = hour * 60 + minutes
	movb	%dh, %al        // dh = seconds in BCD
	call	bcd
	xchg	%bx, %ax        // %ax = hour * 60 + minutes, %bx = seconds
	mulw	c60            // %dx-ax = (hour * 60 + minutes) * 60
	add	%ax, %bx
	adc	$0, %dx         // %dx-bx = seconds since midnight
	mov	%dx, %ax 
	mulw	c19663
	xchg	%bx, %ax 
	mulw	c19663
	add	%bx, %dx        // %dx-ax = %dx-bx * (0x1800B0 / (2*2*2*2*5))
	mov	%ax, %cx        // (0x1800B0 = ticks per day of BIOS clock)
	mov	%dx, %ax
	xor	%dx, %dx
	divw	c1080
	xchg	%cx, %ax
	divw	c1080          // %cx-ax = %dx-ax / (24*60*60 / (2*2*2*2*5))
	mov	%ax, %dx        // %cx-dx = ticks since midnight
	movb	$0x01, %ah      // Set system time
	int	$0x1A
noclock:
	pop	%bp
	pop	%bx
	ret                     // Return to monitor as if nothing much happened

debug_str:	.ascii "Debug\n\0"

// Transform BCD number in al to a regular value in %ax.
bcd:
	movb	%al, %ah
	shrb	$4, %ah
	andb	$0x0F, %al
	aad                     // %ax = (al >> 4) * 10 + (al & 0x0F)
	ret

// Support function for Minix-386 to make a BIOS int 13 call (disk I/O).
bios13:
	mov	%sp, %bp
	call	prot2real
	sti                     // Enable interrupts
	mov	8(%bp), %ax     // Load parameters
	mov	10(%bp), %bx
	mov	12(%bp), %cx
	mov	14(%bp), %dx
	mov	16(%bp), %es
	int	$0x13            // Make the BIOS call
	mov	%ax, 8(%bp)      // Save results
	mov	%bx, 10(%bp)
	mov	%cx, 12(%bp)
	mov	%dx, 14(%bp)
	mov	%es, 16(%bp)
	cli                       // Disable interrupts
	call	real2prot
	mov	$DS_SELECTOR, %ax // Kernel data
	mov	%ax, %ds 
	lretl                     // Return to the kernel

// Support function for Minix-386 to make an 8086 interrupt call.
int86:
	mov	%sp, %bp 
	call	prot2real

	xorl	%eax, %eax
	mov	%ax, %es		// Vector & BIOS data segments
	movl	%eax, %es:0x046C	// Clear BIOS clock tick counter
	sti                             // Enable interrupts
	movb	$0xCD, %al      // INT instruction
	movb	8(%bp), %ah     // Interrupt number?
	testb	%ah, %ah
	jnz	0f              // Nonzero if INT, otherwise far call
	push	%cs
	push	$intret+2       // Far return address
	pushl	12(%bp)        // Far driver address
	mov	$0x90CB, %ax    // RETF; NOP
0:
	cmp	%cs:intret, %ax       // Needs to be changed?
	je	0f              // If not then avoid a huge I-cache stall
	mov	%ax, %cs:intret       // Patch 'INT n' or 'RETF; NOP' into code
	jmp	.+2             // Clear instruction queue
0:
	mov	16(%bp), %ds    // Load parameters
	mov	18(%bp), %es
	movl	20(%bp), %eax
	movl	24(%bp), %ebx
	movl	28(%bp), %ecx
	movl	32(%bp), %edx
	movl	36(%bp), %esi
	movl	40(%bp), %edi
	movl	44(%bp), %ebp

intret:
	int	$0xFF          // Do the interrupt or far call
	pushl	%ebp           // Save results
	pushfl
	mov	%sp, %bp
	popl	8+8(%bp)       // eflags
	mov	%ds, 8+16(%bp) 
	mov	%es, 8+18(%bp)
	movl	%eax, 8+20(%bp)
	movl	%ebx, 8+24(%bp)
	movl	%ecx, 8+28(%bp)
	movl	%edx, 8+32(%bp)
	movl	%esi, 8+36(%bp)
	movl	%edi,8+40(%bp)
	popl	8+44(%bp)          // ebp
	cli                        // Disable interrupts
	xor	%ax, %ax
	mov	%ax, %ds           // Vector & BIOS data segments
	movl	0x046C, %ecx        // Collect lost clock ticks in ecx
	mov	%ss, %ax
	mov	%ax, %ds           // Restore monitor %ds
	call	real2prot
	mov	$DS_SELECTOR, %ax  // Kernel data
	mov	%ax, %ds
	lretl                      // Return to the kernel

// Switch from real to protected mode.
real2prot:
	movb	$0x02, %ah        // Code for A20 enable
	call	gate_A20

	lgdt	p_gdt_desc        // Global descriptor table
	mov	pdbr, %eax        // Load page directory base register
	mov	%eax, %cr3
	mov	%cr0, %eax

	xchgl	msw, %eax          // Exchange real mode msw for protected mode msw
	mov	%eax, %cr0
	ljmp	$MCS_SELECTOR, $cs_prot  // Set code segment selector
cs_prot:
	mov	$SS_SELECTOR, %ax  // Set data selectors
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %ss
	ret

// Switch from protected to real mode.
prot2real:
	lidt	p_idt_desc       // Real mode interrupt vectors
	mov	%cr3, %eax

	movl	%eax, pdbr      // Save page directory base register
	mov	%cr0, %eax
	xchgl	msw, %eax        // Exchange protected mode msw for real mode msw
	mov	%eax, %cr0
	ljmp	$0xDEAD, $cs_real // Reload cs register
cs_real:
	mov	$0xBEEF, %ax
ds_real:
	mov	%ax, %ds    // Reload data segment registers
	mov	%ax, %es
	mov	%ax, %ss

	xorb	%ah, %ah    // Code for A20 disable
	//jmp	gate_A20

// Enable (ah = 0x02) or disable (ah = 0x00) the A20 address line.
gate_A20:
	cmp	$2, bus     // PS/2 bus?
	je	gate_PS_A20
	call	kb_wait
	movb	$0xD1, %al  // Tell keyboard that a command is coming
	outb	$0x64
	call	kb_wait
	movb	$0xDD, %al  // 0xDD = A20 disable code if ah = 0x00
	orb	%ah, %al    // 0xDF = A20 enable code if ah = 0x02
	outb	$0x60
	call	kb_wait
	movb	$0xFF, %al  // Pulse output port
	outb	$0x64
	call	kb_wait     // Wait for the A20 line to settle down
	ret
kb_wait:
	inb	$0x64
	testb	$0x02, %al  // Keyboard input buffer full?
	jnz	kb_wait     // If so, wait
	ret

gate_PS_A20:          // The PS/2 can twiddle A20 using port A
	inb	$0x92       // Read port A
	andb	$0xFD, %al
	orb	%ah, %al    // Set A20 bit to the required state
	outb	$0x92       // Write port A
	jmp	.+2         // Small delay
A20ok:
	inb	$0x92       // Check port A
	andb	$0x02, %al
	cmpb	%ah, %al    // A20 line settled down to the new state?
	jne	A20ok       // If not then wait
	ret

// void int15(bios_env_t *ep)
//       Do an "INT 15" call, primarily for APM (Power Management).
/*
	---> stack grows --->
	------------+----2b----+-2b-+--2b-+
	stack minix |    *ep   | RA | %si |
	------------+----------+----+-----+
	  offset    |          4    2     0

	---> stack grows --->
	------------+----4b----+-4b-+--2b-+
	  code16gcc |    *ep   | RA | %si |
	------------+----------+----+-----+
	  offset    |          6    2     0


	------------+--4b---+---------4b--------+----------4b---------+-4b-+-2b--+
	  code16gcc | count | sectorH | sectorL | bufaddrH | bufaddrL | RA | %bp |
	------------+-------+-------------------+---------------------+----+-----+
	   offset   |       14        12        10         8          6    2     0
 */

.globl int15
int15:
	push	%si           // Save callee-save register %si
	mov	%sp, %si
	push	%bx
	mov	6(%si), %si   // ep
	mov	(%si), %ax    // ep->ax
	mov	2(%si), %bx   // ep->bx
	mov	4(%si), %cx   // ep->cx
	int	$0x15         // INT 0x15 BIOS call
	pushfw                // Save flags
	mov	%ax, (%si)    // ep->ax
	mov	%bx, 2(%si)   // ep->bx
	mov	%cx, 4(%si)   // ep->cx
	popw	6(%si)        // ep->flags
	pop	%bx
	pop	%si           // Restore
	ret

// void scan_keyboard(void)
//       Read keyboard character. Needs to be done in case one is waiting.
.globl scan_keyboard
scan_keyboard:
	inb	$0x60
	inb	$0x61
	movb	%al, %ah
	orb	$0x80, %al
	outb	$0x61
	movb	%ah, %al
	outb	$0x61
	ret

.section .data
nomem:		.ascii  "\nOut of%s\0"
memwarn:	.ascii  "\nLow on"
chmem:		.ascii  " memory, use chmem to increase the heap\n\0"

	.p2align  1
break:		.word  _end       // A fake heap pointer

any_key:	.ascii  "\nHit any key to reboot\n\0"
seclist:	.byte 18, 15, 9       // 1.44M, 1.2M, and 360K/720K floppy sec/track

	.p2align 2
// Extended read/write commands require a parameter packet.
ext_rw:
	.byte 0x10            // Length of extended r/w packet
	.byte 0               // Reserved
	.word 0               // Blocks to transfer (to be filled in)
	.word 0               // Buffer address offset (tbfi)
	.word 0               // Buffer address segment (tbfi)
	.long 0               // Starting block number low 32 bits (tbfi)
	.long 0               // Starting block number high 32 bits
	.ascii  "(null)\0"    // Just in case someone follows a null pointer
	.p2align 1

c60:	.word 60              // Constants for MUL and DIV
c1024:	.word 1024
c1080:	.word 1080
c19663:	.word 19663

// Global descriptor tables.
.set	UNSET, 0              // Must be computed

// For "Extended Memory Block Move".
x_gdt:
x_null_desc:
	// Null descriptor
	.word 0x0000, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00
x_gdt_desc:
	// Descriptor for this descriptor table
	.word 6*8-1, UNSET
	.byte UNSET, 0x00, 0x00, 0x00
x_src_desc:
	// Source segment descriptor
	.word 0xFFFF, UNSET
	.byte UNSET, 0x92, 0x00, UNSET
x_dst_desc:
	// Destination segment descriptor
	.word 0xFFFF, UNSET
	.byte UNSET, 0x92, 0x00, UNSET
x_bios_desc:
	// BIOS segment descriptor (scratch for int 0x15)
	.word UNSET, UNSET
	.byte UNSET, UNSET, UNSET, UNSET
x_ss_desc:
	// BIOS stack segment descriptor (scratch for int 0x15)
	.word UNSET, UNSET
	.byte UNSET, UNSET, UNSET, UNSET

// Protected mode descriptor table.
p_gdt:
p_null_desc:
	// Null descriptor
	.word 0x0000, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00
p_gdt_desc:
	// Descriptor for this descriptor table
	.word 8*8-1, UNSET
	.byte UNSET, 0x00, 0x00, 0x00
p_idt_desc:
	// Real mode interrupt descriptor table descriptor
	.word 0x03FF, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00
p_ds_desc:
	// Kernel data segment descriptor (4 Gb flat)
	.word 0xFFFF, UNSET
	.byte UNSET, 0x92, 0xCF, 0x00
p_es_desc:
	// Physical memory descriptor (4 Gb flat)
	.word 0xFFFF, 0x0000
	.byte 0x00, 0x92, 0xCF, 0x00
p_ss_desc:
	// Monitor data segment descriptor (64 kb flat)
	.word 0xFFFF, UNSET
	.byte UNSET, 0x92, 0x00, 0x00
p_cs_desc:
	// Kernel code segment descriptor (4 Gb flat)
	.word 0xFFFF, UNSET
	.byte UNSET, 0x9A, 0xCF, 0x00
p_mcs_desc:
	// Monitor code segment descriptor (64 kb flat)
	.word 0xFFFF, UNSET
	.byte UNSET, 0x9A, 0x00, 0x00

.section .bss
	.comm	old_vid_mode, 2  // Video mode at startup
	.comm	cur_vid_mode, 2  // Current video mode
	.lcomm	dev_state, 2    // Device state: reset (-1), closed (0), open (1)
	.lcomm	sectors, 2      // number of sectors of current device
	.lcomm	secspcyl, 2     // (Sectors * heads) of current device
	.lcomm	msw, 4          // Saved machine status word (cr0)
	.lcomm	pdbr, 4         // Saved page directory base register (cr3)
	.lcomm	varescape, 2    // Escape typed?
	.lcomm	bus, 2          // Saved return value of get_bus
	.lcomm	unchar, 2       // Char returned by ungetch(c)
	.lcomm	line, 2         // Serial line I/O port to copy console I/O to.
