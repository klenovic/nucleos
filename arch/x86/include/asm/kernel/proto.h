/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __X86_ASM_KERNEL_PROTO_H
#define __X86_ASM_KERNEL_PROTO_H

#ifdef __KERNEL__

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
void int07(void), copr_not_available(void);
void double_fault(void);
void copr_seg_overrun(void);
void inval_tss(void);
void segment_not_present(void);
void stack_exception(void);
void general_protection(void);
void page_fault(void);
void copr_error(void);

/* Software interrupt handlers, in numerical order. */
void trp(void);
void s_call(void), p_s_call(void); 
void level0_call(void);

/* memory.c */
void vir_insb(u16_t port, struct proc *proc, u32_t vir, size_t count);
void vir_outsb(u16_t port, struct proc *proc, u32_t vir, size_t count);
void vir_insw(u16_t port, struct proc *proc, u32_t vir, size_t count);
void vir_outsw(u16_t port, struct proc *proc, u32_t vir, size_t count);


/* exception.c */
void exception(unsigned vec_nr, u32_t trap_errno, u32_t old_eip, U16_t old_cs, u32_t old_eflags);

/* klib386.s */
void level0(void (*func)(void));
void monitor(void);
void reset(void);
void int86(void);
unsigned long read_cr0(void);
void write_cr0(unsigned long value);
unsigned long read_cr4(void);
void write_cr4(unsigned long value);
void write_cr3(unsigned long value);
unsigned long read_cpu_flags(void);
void phys_insb(U16_t port, phys_bytes buf, size_t count);
void phys_insw(U16_t port, phys_bytes buf, size_t count);
void phys_outsb(U16_t port, phys_bytes buf, size_t count);
void phys_outsw(U16_t port, phys_bytes buf, size_t count);
void i386_invlpg(U32_t addr);

/* protect.c */
void prot_init(void);
void init_codeseg(struct segdesc_s *segdp, phys_bytes base, vir_bytes size, int privilege);
void init_dataseg(struct segdesc_s *segdp, phys_bytes base, vir_bytes size, int privilege);
void enable_iop(struct proc *pp);

#endif /* __KERNEL__ */
#endif /* __X86_ASM_KERNEL_PROTO_H */
