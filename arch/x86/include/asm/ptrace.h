/* linux */
#ifndef __ASM_X86_PTRACE_H
#define __ASM_X86_PTRACE_H

#include <nucleos/compiler.h>	/* For __user */

/* @nucleos:  Only the i386 architecture and old (legacy minix3) layout of registers
	      is used for now. */
#define OLD_REGS_LAYOUT		1

#ifndef __ASSEMBLY__

#ifdef __i386__
/* this struct defines the way the registers are stored on the
   stack during a system call. */

#if !defined(__KERNEL__) && !defined(__UKERNEL__)

struct pt_regs {
#ifdef OLD_REGS_LAYOUT
	unsigned short xgs;
	unsigned short xfs;
	unsigned short xes;
	unsigned short xds;

	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;

	unsigned long orig_ax;
	unsigned long eip;
	unsigned long xcs;
	unsigned long eflags;
	unsigned long old_esp;
	unsigned long xss;
#else
	long ebx;
	long ecx;
	long edx;
	long esi;
	long edi;
	long ebp;
	long eax;
	int  xds;
	int  xes;
	int  xfs;
	int  xgs;
	long orig_eax;
	long eip;
	int  xcs;
	long eflags;
	long esp;
	int  xss;
#endif /* OLD_REGS_LAYOUT */
};

#else /* defined(__KERNEL__) || defined(__UKERNEL__) */

struct pt_regs {
#ifdef OLD_REGS_LAYOUT
	unsigned short gs;
	unsigned short fs;
	unsigned short es;
	unsigned short ds;

	unsigned long di;
	unsigned long si;
	unsigned long bp;
	unsigned long sp;
	unsigned long bx;
	unsigned long dx;
	unsigned long cx;
	unsigned long ax;

	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long old_sp;
	unsigned long ss;
#else
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long bp;
	unsigned long ax;

	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;

	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
#endif /* OLD_REGS_LAYOUT */
};

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#else /* __i386__ */

#if !defined(__KERNEL__) && !defined(__UKERNEL__)

struct pt_regs {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long rbp;
	unsigned long rbx;
/* arguments: non interrupts/non tracing syscalls only save upto here*/
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rax;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rsi;
	unsigned long rdi;
	unsigned long orig_rax;
/* end of arguments */
/* cpu exception frame or undefined */
	unsigned long rip;
	unsigned long cs;
	unsigned long eflags;
	unsigned long rsp;
	unsigned long ss;
/* top of stack page */
};

#else /* defined(__KERNEL__) || defined(__UKERNEL__) */

struct pt_regs {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
/* arguments: non interrupts/non tracing syscalls only save upto here*/
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long orig_ax;
/* end of arguments */
/* cpu exception frame or undefined */
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
/* top of stack page */
};

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */
#endif /* !__i386__ */
#endif /* !__ASSEMBLY__ */

#endif /* _ASM_X86_PTRACE_H */
