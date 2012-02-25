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

	.set DS_SELECTOR,	3*8	// Kernel data selector
	.set ES_SELECTOR,	4*8	// Flat 4 Gb
	.set SS_SELECTOR,	5*8	// Monitor stack
	.set CS_SELECTOR,	6*8	// Kernel code
	.set MCS_SELECTOR,	7*8	// Monitor code

.section .text

.globl halt_cpu
halt_cpu:
	sti
	hlt
	cli
	ret

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

	.globl pm_nucleos
// void nucleos(u32_t koff, u32_t kcs, u32_t kds, char *bootparams);
/*
	------------+--2b---+--2b---+-----4b-----+-----4b------+---2b----+---2b----+---2b-----+---2b-----+---2b-----+---2b-----+-4b-+--2b-+--2b-+
	  code16gcc | aoutH | aoutL | paramsize  | bootparams  |  kdsH   |  kdsL   |  kcsH    |  kcsL    |   koffH  |   koffL  | RA | %bx | %bp |
	------------+-------+-------+------------+-------------+---------+---------+----------+----------+----------+----------+----+--2b-+--2b-+
	  offset    |       30      28           24            20        18        16         14         12         10         8    4     2     0
 */
#define KOFFL	8
#define KOFFH	10
#define KCSL	12
#define KCSH	14
#define KDSL	16
#define KDSH	18
#define BOOTP	20
#define PARAMS	24
#define AOUTL	28
#define AOUTH	30

ebxval:	.long 0

pm_nucleos:
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

	pushl	BOOTP(%bp)  // 32 bit size of parameters on stack
	pushw	$0
	pushw	$CS_SELECTOR
	pushw	KOFFH(%bp)
	pushw	KOFFL(%bp)       // 32 bit far address to kernel entry point
	call	real2prot         // Switch to protected mode
	mov	$DS_SELECTOR, %ax // Kernel data
	mov	%ax, %ds
	mov	$ES_SELECTOR, %ax // Flat 4 Gb
	mov	%ax, %es
	lretl                     // Make a far call to the kernel
// should never get here
ret386:
1:
	call halt_cpu
	jmp 1b

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
