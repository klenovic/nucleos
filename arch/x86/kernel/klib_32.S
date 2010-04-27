/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/const.h>
#include <kernel/const.h>
#include <asm/irq_vectors.h>

// The routines only guarantee to preserve the registers the C compiler
// expects to be preserved (ebx, esi, edi, ebp, esp, segment registers, and
// direction bit in the flags).

.section .text
//===========================================================================
//                             monitor
//===========================================================================
// void monitor();
// Return to the monitor.
.globl monitor				// exit Minix and return to the monitor
monitor:
	mov	mon_sp, %esp		// restore monitor stack pointer
	movw	$SS_SELECTOR, %dx	// monitor data segment
	mov	%dx, %ds
	mov	%dx, %es
	mov	%dx, %fs
	mov	%dx, %gs
	mov	%dx, %ss
	pop	%edi
	pop	%esi
	pop	%ebp
	lretw				// return to the monitor

//===========================================================================
//                             int86
//===========================================================================
// void int86();
.globl int86          // let the monitor make an 8086 interrupt call
int86:
	cmpb	$0,mon_return  // is the monitor there?
	jnz	0f
	movb	$0x01, %ah     // an int 13 error seems appropriate
	movb	%ah, reg86+ 0  // reg86.w.f = 1 (set carry flag)
	movb	%ah, reg86+13  // reg86.b.ah = 0x01 = "invalid command"
	ret
0:      push	%ebp           // save C registers
	push	%esi
	push	%edi
	push	%ebx
	pushf			// save flags
	cli			// no interruptions

	inb	$INT2_CTLMASK
	movb	%al, %ah
	inb	$INT_CTLMASK
	push	%eax                   // save interrupt masks
	mov	irq_use, %eax          // map of in-use IRQ''s
	and	$~[1<<CLOCK_IRQ], %eax // keep the clock ticking
	outb	$INT_CTLMASK           // enable all unused IRQ''s and vv.
	movb	%ah, %al
	outb	$INT2_CTLMASK

	mov	$SS_SELECTOR, %eax    // monitor data segment
	mov	%ax, %ss
	xchg	mon_sp, %esp          // switch stacks
	push	reg86+36              // parameters used in INT call
	push	reg86+32
	push	reg86+28
	push	reg86+24
	push	reg86+20
	push	reg86+16
	push	reg86+12
	push	reg86+ 8
	push	reg86+ 4
	push	reg86+ 0
	mov	%ax, %ds              // remaining data selectors
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	push	%cs
	push	$return                  // kernel return address and selector
	ljmpw	*20+2*4+10*4+2*4(%esp)  // make the call
return:
	pop	reg86+ 0
	pop	reg86+ 4
	pop	reg86+ 8
	pop	reg86+12
	pop	reg86+16
	pop	reg86+20
	pop	reg86+24
	pop	reg86+28
	pop	reg86+32
	pop	reg86+36
	lgdt	gdt+GDT_SELECTOR      // reload global descriptor table
	ljmp	$CS_SELECTOR, $csinit // restore everything
csinit: 
	mov	$DS_SELECTOR, %eax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss
	xchg	mon_sp, %esp         // unswitch stacks
	lidt	gdt+IDT_SELECTOR     // reload interrupt descriptor table

#ifdef CONFIG_X86_LOCAL_APIC
	cmpl	$0x0, lapic_addr
	jne	3f
	mov	$0, %ebx
	jmp	4f
3:
	mov	$FLAT_DS_SELECTOR, %ebx
	mov	%bx, %fs
	movl	lapic_addr, %eax
	add	$0x20, %eax
	mov	%fs:(%eax), %ebx
	and	$0xFF000000, %ebx
	shr	$24, %ebx
	movzb	%bl, %ebx

4:
	add	$apicid2cpuid, %ebx
	movzb	(%ebx), %eax
	shl	$3, %eax
	mov	%eax, %ebx
	add	$TSS_SELECTOR, %eax
	addl	gdt+DESC_ACCESS, %eax
	and	$~0x02, %eax
	ltr	%bx     /* set TSS register */

	mov	$DS_SELECTOR, %eax
	mov	%ax, %fs
#endif /* CONFIG_X86_LOCAL_APIC */

	pop	%eax
	outb	$INT_CTLMASK        // restore interrupt masks
	movb	%ah, %al
	outb	$INT2_CTLMASK

6:
	add	%ecx, lost_ticks    // record lost clock ticks

	popf                        // restore flags
	pop	%ebx                // restore C registers
	pop	%edi
	pop	%esi
	pop	%ebp
	ret

//===========================================================================
//                             phys_insw
//===========================================================================
// void phys_insw(port_t port, phys_bytes buf, size_t count);
// Input an array from an I/O port.  Absolute address version of insw().
.globl phys_insw      // transfer data from (disk controller) port to memory
phys_insw:
	push	%ebp
	mov	%esp, %ebp
	cld
	push	%edi
	push	%es

	mov	$FLAT_DS_SELECTOR, %ecx
	mov	%cx, %es
	mov	8(%ebp), %edx             // port to read from
	mov	12(%ebp), %edi            // destination addr
	mov	16(%ebp), %ecx            // byte count
	shr	$1, %ecx                  // word count
	rep	insw                      // input many words
	pop	%es
	pop	%edi
	pop	%ebp
	ret

//===========================================================================
//                             phys_insb
//===========================================================================
// void phys_insb(port_t port, phys_bytes buf, size_t count);
// Input an array from an I/O port.  Absolute address version of insb().
.globl phys_insb      // likewise byte by byte
phys_insb:
	push	%ebp
	mov	%esp, %ebp
	cld
	push	%edi
	push	%es

	mov	$FLAT_DS_SELECTOR, %ecx
	mov	%cx, %es
	mov	8(%ebp), %edx             // port to read from
	mov	12(%ebp), %edi            // destination addr
	mov	16(%ebp), %ecx            // byte count
	rep	insb                      // input many bytes
	pop	%es
	pop	%edi
	pop	%ebp
	ret

//===========================================================================
//                             phys_outsw
//===========================================================================
// void phys_outsw(port_t port, phys_bytes buf, size_t count);
// Output an array to an I/O port.  Absolute address version of outsw().
	.p2align 4
.globl phys_outsw     // transfer data from memory to (disk controller) port
phys_outsw:
	push	%ebp
	mov	%esp, %ebp
	cld
	push	%esi
	push	%ds

	mov	$FLAT_DS_SELECTOR, %ecx
	mov	%cx, %ds
	mov	8(%ebp), %edx             // port to write to
	mov	12(%ebp), %esi            // source addr
	mov	16(%ebp), %ecx            // byte count
	shr	$1, %ecx                  // word count
	rep	outsw                     // output many words
	pop	%ds
	pop	%esi
	pop	%ebp
	ret

//===========================================================================
//                             phys_outsb
//===========================================================================
// void phys_outsb(port_t port, phys_bytes buf, size_t count);
// Output an array to an I/O port.  Absolute address version of outsb().
	.p2align 4
.globl phys_outsb     // likewise byte by byte
phys_outsb:
	push	%ebp
	mov	%esp, %ebp
	cld
	push	%esi
	push	%ds

	mov	$FLAT_DS_SELECTOR, %ecx
	mov	%cx, %ds
	mov	8(%ebp), %edx             // port to write to
	mov	12(%ebp), %esi            // source addr
	mov	16(%ebp), %ecx            // byte count
	rep	outsb                     // output many bytes
	pop	%ds
	pop	%esi
	pop	%ebp
	ret

//===========================================================================
//                             phys_copy
//===========================================================================
// phys_bytes phys_copy(phys_bytes source, phys_bytes destination,
//                      phys_bytes bytecount);
// Copy a block of physical memory.

.set PC_ARGS, 4 + 4 + 4 + 4   // 4 + 4 + 4
//              es edi esi eip   src dst len
	.p2align 4
.globl phys_copy      // copy data from anywhere to anywhere in memory
phys_copy:
	cld
	push	%esi
	push	%edi
	push	%es

	mov	$FLAT_DS_SELECTOR, %eax
	mov	%ax, %es

	mov	PC_ARGS(%esp), %esi
	mov	PC_ARGS+4(%esp), %edi
	mov	PC_ARGS+4+4(%esp), %eax

	cmp	$10, %eax                // avoid align overhead for small counts
	jb	pc_small
	mov	%esi, %ecx               // align source, hope target is too
	neg	%ecx
	and	$3, %ecx                 // count for alignment
	sub	%ecx, %eax
	rep
	movsb	%es:(%esi), %es:(%edi)   // @nucleos: this long form is important here
	                                 //         otherwise the source %ds is used for source
	mov	%eax, %ecx
	shr	$2, %ecx                 // count of dwords
	rep
	movsl	%es:(%esi), %es:(%edi)
	and	$3, %eax
pc_small:
	xchg	%eax, %ecx               // remainder
	rep
	movsb	%es:(%esi), %es:(%edi)
	mov	$0, %eax			// 0 means no fault

.globl phys_copy_fault
phys_copy_fault:				// kernel can send us here
	pop	%es
	pop	%edi
	pop	%esi
	ret

.globl phys_copy_fault_in_kernel
phys_copy_fault_in_kernel:                     /* kernel can send us here */
	pop	%es
	pop	%edi
	pop	%esi
	mov	%cr2, %eax
	ret

//===========================================================================
//                             phys_memset
//===========================================================================
// void phys_memset(phys_bytes source, unsigned long pattern,
//      phys_bytes bytecount);
// Fill a block of physical memory with pattern.
	.p2align 4
.globl phys_memset    // write pattern anywhere in memory
phys_memset:
	push	%ebp
	mov	%esp, %ebp
	push	%esi
	push	%ebx
	push	%ds

	mov	8(%ebp), %esi
	mov	16(%ebp), %eax
	mov	$FLAT_DS_SELECTOR, %ebx
	mov	%bx, %ds
	mov	12(%ebp), %ebx
	shr	$2, %eax
fill_start:
	mov	%ebx, (%esi)
	add	$4, %esi
	dec	%eax
	jnz	fill_start
	// Any remaining bytes?
	mov	16(%ebp), %eax
	and	$3, %eax
remain_fill:
	cmp	$0, %eax
	jz	fill_done
	movb	12(%ebp), %bl
	movb	%bl, (%esi)
	add	$1, %esi
	inc	%ebp
	dec	%eax
	jmp	remain_fill
fill_done:
	pop	%ds
	pop	%ebx
	pop	%esi
	pop	%ebp
	ret

//===========================================================================
//                             mem_rdw
//===========================================================================
// u16_t mem_rdw(u16 segment, u16_t *offset);
// Load and return word at far pointer segment:offset.
	.p2align 4
.globl mem_rdw        // copy one word from [segment:offset]
mem_rdw:
	mov	%ds, %cx
	mov	4(%esp), %ds              // segment
	mov	4+4(%esp), %eax           // offset
	movzwl	(%eax), %eax              // word to return
	mov	%cx, %ds
	ret

//===========================================================================
//                             reset
//===========================================================================
// void reset();
// Reset the system by loading IDT with offset 0 and interrupting.
.globl reset          // reset the system
reset:
	lidt    idt_zero
	int     $3               // anything goes, the 386 will not like it

/*===========================================================================*/
/*                             halt_cpu                                     */
/*===========================================================================*/
/*
 * void halt_cpu(void);
 * reanables interrupts and puts the cpu in the halts state. Once an interrupt
 * is handled the execution resumes by disabling interrupts and continues
 */
.globl halt_cpu
halt_cpu:
	sti
	hlt
	cli
	ret

//===========================================================================
//                           level0
//===========================================================================
// void level0(void (*func)(void))
// Call a function at permission level 0.  This allows kernel tasks to do
// things that are only possible at the most privileged CPU level.
//
.globl level0         // call a function at level 0
level0:
	/* check whether we are already running in kernel, the kernel cs
	 * selector has 3 lower bits zeroed */
	mov	%cs, %ax
	cmpw	$CS_SELECTOR, %ax
	jne	0f

	/* call the function directly as if it was a normal function call */
	mov	4(%esp), %eax
	call	*%eax
	ret

	/* if not runnig in the kernel yet, trap to kernel */
0:
	mov	4(%esp), %eax
	int	$LEVEL0_VECTOR
	ret

//===========================================================================
//                           read_flags
//===========================================================================
// unsigned long read_cpu_flags(void);
// Read CPU status flags from C.
	.p2align 4
.globl read_cpu_flags // read the cpu flags
read_cpu_flags:
	pushf
	mov	(%esp), %eax
	add	$4, %esp
	ret

/*
 * Read the Model Specific Register (MSR) of IA32 architecture
 *
 * void ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo)
 */
.globl ia32_msr_read
ia32_msr_read:
	push	%ebp
	mov	%esp, %ebp
	mov	8(%ebp), %ecx
	rdmsr
	mov	12(%ebp), %ecx
	mov	%edx, (%ecx)
	mov	16(%ebp), %ecx
	mov	%eax, (%ecx)

	pop	%ebp
	ret

/*
 * Write the Model Specific Register (MSR) of IA32 architecture
 *
 * void ia32_msr_write(u32_t reg, u32_t hi, u32_t lo)
 */
.globl ia32_msr_write
ia32_msr_write:
	push	%ebp
	mov	%esp, %ebp

	mov	12(%ebp), %edx
	mov	16(%ebp), %eax
	mov	8(%ebp), %ecx
	wrmsr

	pop	%ebp
	ret

/*===========================================================================*/
/*                           idt_reload                                     */
/*===========================================================================*/
/* void idt_reload (void); 
 * Reload idt when returning to monitor. 
 */
	.p2align 4
.globl idt_reload
idt_reload:
	lidt	gdt+IDT_SELECTOR	/*  reload interrupt descriptor table */
	ret

/*
 * void reload_segment_regs(void)
 */
#define RELOAD_SEG_REG(reg) \
	mov	reg, %ax	;\
	mov	%ax, reg	;

.globl reload_ds
reload_ds:
	RELOAD_SEG_REG(%ds)
	ret

/*===========================================================================*/
/*                            fpu_routines                                   */
/*===========================================================================*/
/* non-waiting FPU initialization */
.globl fninit
fninit:
	fninit
	ret

/* store status word (non-waiting) */
.globl fnstsw
fnstsw:
	xor	%eax, %eax
	fnstsw	%ax
	ret

/* store control word (non-waiting) */
.globl fnstcw 
fnstcw:
	push	%eax
	mov	8(%esp), %eax
	fnstcw	(%eax)
	pop	%eax
	ret

//===========================================================================
//                           read_cr0
//===========================================================================
// unsigned long read_cr0(void);
.globl read_cr0       // read cr0
read_cr0:
	push	%ebp
	mov	%esp, %ebp
	mov	%cr0, %eax
	pop	%ebp
	ret

//===========================================================================
//                           write_cr0
//===========================================================================
// void write_cr0(unsigned long value);
.globl write_cr0      // write a value in cr0
write_cr0:
	push	%ebp
	mov	%esp, %ebp
	mov	8(%ebp), %eax
	mov	%eax, %cr0
	jmp	0f              // A jump is required for some flags
0:
	pop	%ebp
	ret

/*===========================================================================*/
/*                            read_cr2                                       */
/*===========================================================================*/
/* reg_t read_cr2(void); */
.globl read_cr2
read_cr2:
	mov	%cr2, %eax
	ret

//===========================================================================*
//                            read_cr4                                       *
//===========================================================================*
// unsigned long read_cr4(void);
.globl read_cr4
read_cr4:
	push	%ebp
	mov	%esp, %ebp
	mov	%cr4, %eax
	pop	%ebp
	ret

//===========================================================================*
//                            write_cr4                                      *
//===========================================================================*
//void write_cr4(unsigned long value);
.globl write_cr4
write_cr4:
	push	%ebp
	mov	%esp, %ebp
	mov	8(%ebp), %eax
	mov	%eax, %cr4
	jmp	0f
0:
	pop	%ebp
	ret

.globl read_ds
read_ds:
	mov	$0, %eax
	mov	%ds, %eax
	ret

.globl read_cs
read_cs:
	mov	$0, %eax
	mov	%cs, %eax
	ret

.globl read_ss
read_ss:
	mov	$0, %eax
	mov	%ss, %eax
	ret

//===========================================================================*
//                           getcr3val                                       *
//===========================================================================*
// unsigned long getcr3val(void);
.globl getcr3val
getcr3val:
	mov	%cr3, %eax
	mov	%eax, thecr3
	ret

/**
 * strnlen_user: - Get the size of a string specified by linear address.
 * @s: The string to measure (linear address).
 * @maxlen: The maximum valid length
 *
 * Get the size of a NUL-terminated string.
 *
 * Returns the size of the string _including_ the terminating NUL.
 * On kernel exception, returns 0.
 * If the string is too long, returns a value greater than @n.
 */
.globl __phys_strnlen
__phys_strnlen:
	cld
	push	%es
	mov	$FLAT_DS_SELECTOR, %eax
	mov	%ax, %es

	mov	8(%esp), %ecx	/* get s */
	mov	8+4(%esp), %edx	/* get maxlen */
	mov	%ecx, %eax
	jmp	2f
1:
	cmpb	$0, %es:(%eax)	/* compare with '\0' */
	je	3f		/* '\0'? */
	inc	%eax		/* next char */
2:
	dec	%edx		/* decrement maxlen by 1 (preserve CF flag) */
	cmp	$-1, %edx	/* compare,  %edx = %edx - $1 */
	jne	1b		/* is result zero (ZF==1)? */
3:
	sub	%ecx, %eax	/* %eax = %eax - %ecx or maxlen */

.globl __phys_strnlen_fault
__phys_strnlen_fault:		/* kernel can send us here */
	inc	%eax		/* including '\0' or maxlen+1 */
	pop	%es
	ret

.globl __phys_strnlen_fault_in_kernel
__phys_strnlen_fault_in_kernel:	/* kernel can send us here */
	xor	%eax, %eax	/* returns 0 in this case */
	pop	%es
	ret

.section .data
idt_zero: .long 0, 0
