/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __ASM_X86_KERNEL_PROTO_H
#define __ASM_X86_KERNEL_PROTO_H

#ifdef __KERNEL__

#include <kernel/types.h>

struct proc;
struct segdesc_s;

/* Hardware interrupt handlers. */
void hwint00(void);
void hwint01(void);
void hwint02(void);
void hwint03(void);
void hwint04(void);
void hwint05(void);
void hwint06(void);
void hwint07(void);
void hwint08(void);
void hwint09(void);
void hwint10(void);
void hwint11(void);
void hwint12(void);
void hwint13(void);
void hwint14(void);
void hwint15(void);

/* Exception handlers (real or protected mode), in numerical order. */
void int00(void), divide_error(void);
void int01(void), single_step_exception(void);
void int02(void), nmi(void);
void int03(void), breakpoint_exception(void);
void int04(void), overflow(void);
void int05(void), bounds_check(void);
void int06(void), inval_opcode(void);
void int07(void), coproc_not_available(void);
void double_fault(void);
void copr_seg_overrun(void);
void inval_tss(void);
void segment_not_present(void);
void stack_exception(void);
void general_protection(void);
void page_fault(void);
void copr_error(void);
void alignment_check(void);
void machine_check(void);
void simd_exception(void);

/* Software interrupt handlers, in numerical order. */
void __kipc_entry(void);	/* internal comunication */
void __syscall_entry(void);	/* nucleos system call (C library) */
void level0_call(void);

/* memory.c */
void i386_freepde(int pde);
void getcr3val(void);
void vm_set_cr3(struct proc *);

/* prototype of an interrupt vector table entry */
struct gate_table_s {
	void (*gate)(void);
	unsigned char vec_nr;
	unsigned char privilege;
};

extern struct gate_table_s gate_table_pic[];

/* copies an array of vectors to the IDT. The last vector must be zero filled */
void idt_copy_vectors(struct gate_table_s * first);
void idt_reload(void);

/* exception.c */
struct exception_frame {
	reg_t vector;         /* which interrupt vector was triggered */
	reg_t errcode;        /* zero if no exception does not push err code */
	reg_t eip;
	reg_t cs;
	reg_t eflags;
	reg_t esp;            /* undefined if trap is nested */
	reg_t ss;             /* undefined if trap is nested */
};

void exception(struct exception_frame * frame);

/* klib386.S */
void level0(void (*func)(void));
void monitor(void);
void reset(void);
void int86(void);

reg_t read_cr0(void);
reg_t read_cr2(void);
reg_t read_cs(void);
reg_t read_ds(void);
reg_t read_ss(void);

void write_cr0(unsigned long value);
unsigned long read_cr4(void);
void write_cr4(unsigned long value);
void write_cr3(unsigned long value);
unsigned long read_cpu_flags(void);
void phys_insb(u16 port, phys_bytes buf, size_t count);
void phys_insw(u16 port, phys_bytes buf, size_t count);
void phys_outsb(u16 port, phys_bytes buf, size_t count);
void phys_outsw(u16 port, phys_bytes buf, size_t count);

u32_t read_cr3(void);
void reload_cr3(void);

void phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
void reload_ds(void);
void ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
void ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
void fninit(void);
unsigned short fnstsw(void);
void fnstcw(unsigned short* cw);

/* protect.c */
struct tss_s {
	reg_t backlink;
	reg_t sp0;                    /* stack pointer to use during interrupt */
	reg_t ss0;                    /*   "   segment  "  "    "        "     */
	reg_t sp1;
	reg_t ss1;
	reg_t sp2;
	reg_t ss2;
	reg_t cr3;
	reg_t ip;
	reg_t flags;
	reg_t ax;
	reg_t cx;
	reg_t dx;
	reg_t bx;
	reg_t sp;
	reg_t bp;
	reg_t si;
	reg_t di;
	reg_t es;
	reg_t cs;
	reg_t ss;
	reg_t ds;
	reg_t fs;
	reg_t gs;
	reg_t ldt;
	u16_t trap;
	u16_t iobase;
/*	u8_t iomap[0]; */
};

extern struct tss_s tss;

extern void *k_boot_stktop;
void tss_init(struct tss_s * tss, void * kernel_stack, unsigned cpu);

void int_gate(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);
void i8259_disable(void);

void idt_init(void);
void init_codeseg(struct segdesc_s *segdp, phys_bytes base, vir_bytes size, int privilege);
void init_dataseg(struct segdesc_s *segdp, phys_bytes base, vir_bytes size, int privilege);
void enable_iop(struct proc *pp);
int prot_set_kern_seg_limit(vir_bytes limit);
void printseg(char *banner, int iscs, struct proc *pr, u32_t selector);

#endif /* __KERNEL__ */
#endif /* __ASM_X86_KERNEL_PROTO_H */
