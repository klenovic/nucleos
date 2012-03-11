/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Interrupt numbers and hardware vectors. */

#ifndef __ASM_X86_IRQ_VECTORS_H
#define __ASM_X86_IRQ_VECTORS_H

/* 8259A interrupt controller ports. */
#define INT_CTL			0x20	/* I/O port for interrupt controller */
#define INT_CTLMASK		0x21	/* setting bits in this port disables ints */
#define INT2_CTL		0xA0	/* I/O port for second interrupt controller */
#define INT2_CTLMASK		0xA1	/* setting bits in this port disables ints */

/* Magic numbers for interrupt controller. */
#define END_OF_INT		0x20	/* code used to re-enable after an interrupt */

/* Interrupt and exception vectors defined/reserved by processor. */
#define EXVEC_DIVIDE			0	/* divide error */
#define EXVEC_DEBUG			1	/* single step (trace) */
#define IRQVEC_NMI			2	/* non-maskable interrupt */
#define EXVEC_BREAKPOINT		3	/* software breakpoint */
#define EXVEC_OVERFLOW			4	/* from INTO */
#define EXVEC_BOUNDS			5	/* bounds check failed */
#define EXVEC_INVAL_OP			6	/* invalid opcode */
#define EXVEC_COPROC_NOT_AVAILABLE	7	/* coprocessor not available */
#define EXVEC_DOUBLE_FAULT		8
#define EXVEC_COPROC_SEG		9	/* coprocessor segment overrun */
#define EXVEC_INVAL_TSS			10	/* invalid TSS */
#define EXVEC_SEG_NOT			11	/* segment not present */
#define EXVEC_STACK_FAULT		12	/* stack exception */
#define EXVEC_PROTECTION		13	/* general protection */
#define EXVEC_PAGE_FAULT		14
#define EXVEC_COPROC_ERR		16	/* coprocessor error */
#define EXVEC_ALIGNMENT_CHECK		17
#define EXVEC_MACHINE_CHECK		18
#define EXVEC_SIMD_EXCEPTION		19	/* SIMD Floating-Point Exception (#XM) */

/* Fixed system call vector. */
#define IRQVEC_SYSCALL	0x80	/* nucleos i386 system call (not for kernel modules) */
#define IRQVEC_KIPC	0xc0	/* internal communication i.e. between kernel modules */
#define IRQVEC_LEVEL0	0x22	/* for execution of a function at level 0 */

/* Suitable irq bases for hardware interrupts.  Reprogram the 8259(s) from
 * the PC BIOS defaults since the BIOS doesn't respect all the processor's
 * reserved vectors (0 to 31).
 */
#define BIOS_IRQ0_VEC		0x08	/* base of IRQ0-7 vectors used by BIOS */
#define BIOS_IRQ8_VEC		0x70	/* base of IRQ8-15 vectors used by BIOS */
#define IRQ0_VECTOR		0x50	/* nice vectors to relocate IRQ0-7 to */
#define IRQ8_VECTOR		0x70	/* no need to move IRQ8-15 */

/* Hardware interrupt numbers. */
#define NR_IRQ_VECTORS		16
#define CLOCK_IRQ		0
#define KEYBOARD_IRQ		1
#define CASCADE_IRQ		2	/* cascade enable for 2nd AT controller */
#define ETHER_IRQ		3	/* default ethernet interrupt vector */
#define SECONDARY_IRQ		3	/* RS232 interrupt vector for port 2 */
#define RS232_IRQ		4	/* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ		5	/* xt winchester */
#define FLOPPY_IRQ		6	/* floppy disk */
#define PRINTER_IRQ		7
#define CMOS_CLOCK_IRQ		8
#define KBD_AUX_IRQ		12	/* AUX (PS/2 mouse) port in kbd controller */
#define AT_WINI_0_IRQ		14	/* at winchester controller 0 */
#define AT_WINI_1_IRQ		15	/* at winchester controller 1 */

/* Interrupt number to hardware vector. */
#define BIOS_VECTOR(irq)	\
	(((irq) < 8 ? BIOS_IRQ0_VEC : BIOS_IRQ8_VEC) + ((irq) & 0x07))
#define VECTOR(irq)	\
	(((irq) < 8 ? IRQ0_VECTOR : IRQ8_VECTOR) + ((irq) & 0x07))

#endif /* __ASM_X86_IRQ_VECTORS_H */
