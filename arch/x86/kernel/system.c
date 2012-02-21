/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* system dependent functions for use inside the whole kernel. */
#include <nucleos/ctype.h>
#include <nucleos/unistd.h>
#include <nucleos/string.h>
#include <nucleos/portio.h>
#include <nucleos/u64.h>
#include <nucleos/a.out.h>
#include <nucleos/uaccess.h>
#include <kernel/proto.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>
#include <kernel/debug.h>
#include <ibm/cmos.h>
#include <ibm/bios.h>
#include <asm/cpufeature.h>
#include <asm/kernel/const.h>

#ifdef CONFIG_X86_LOCAL_APIC
#include <asm/apic.h>
#endif

#define CR0_EM		0x0004		/* set to enable trap on any FP instruction */
#define CR0_MP_NE	0x0022		/* set MP and NE flags to handle FPU 
					   exceptions in native mode. */
#define CR4_OSFXSR	(1L<<9)		/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSXMMEXCPT	(1L<<10)	/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */

static void ser_debug(int c);

int obsolete_check_cpu_has_tsc;

void arch_shutdown(int how)
{
	/* Mask all interrupts, including the clock. */
	outb(INT_CTLMASK, ~0);

	if (kernel_in_panic) {
		/* We're panicing? Then retrieve and decode currently
		 * loaded segment selectors.
		 */
		printseg("cs: ", 1, proc_ptr, read_cs());
		printseg("ds: ", 0, proc_ptr, read_ds());
		if (read_ds() != read_ss())
			printseg("ss: ", 0, NULL, read_ss());
	}

	if (how == RBT_RESET) {
		/* Reset the system by forcing a processor shutdown. First stop
		 * the BIOS memory test by setting a soft reset flag.
		 */
		u16_t magic = STOP_MEM_CHECK;
		phys_copy(SOFT_RESET_FLAG_ADDR, vir2phys(&magic), SOFT_RESET_FLAG_SIZE);
		level0(reset);
	}

	/* halt in case of RBT_HALT and RBT_PANIC */
	while(1) level0(halt_cpu);
}

/* address of a.out headers, set in start.c */
static u8 kimage_aout_headers[MAX_IMG_PROCS_COUNT*A_MINHDR];

void arch_copy_aout_headers(struct boot_params *params)
{
	if (!params || !params->nucleos_kludge.aout_hdrs_addr)
		kernel_panic("Invalid AOUT headers address", NO_NUM);

	phys_copy(vir2phys(kimage_aout_headers), params->nucleos_kludge.aout_hdrs_addr,
		  (phys_bytes)A_MINHDR*MAX_IMG_PROCS_COUNT);
}

struct exec *arch_get_aout_header(int i)
{
	return (struct exec*)(kimage_aout_headers + i*A_MINHDR);
}

static void tss_init(struct tss_s *tss, void *kernel_stack, unsigned cpu)
{
	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	tss->sp0 = ((unsigned) kernel_stack) - 2*sizeof(void*);
	tss->ss0 = DS_SELECTOR;

	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stak in use when we trap to kernel
	 */
	*((reg_t*)(tss->sp0 + 1*sizeof(reg_t))) = cpu;
}

void arch_init(void)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	if ((sw & 0xff) == 0 && (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		fpu_presence = 1;

		if (cpufeature(_CPUF_I386_FXSR)) {
			register struct proc *rp;
			phys_bytes aligned_fp_area;
			/* Enable FXSR feature usage. */
			write_cr4(read_cr4() | CR4_OSFXSR | CR4_OSXMMEXCPT);
			osfxsr_feature = 1;

			for (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; ++rp) {
				/* FXSR requires 16-byte alignment of memory
				 * image, but unfortunately some old tools
				 * (probably linker)ignores ".balign 16"
				 * applied to our memory image.
				 * Thus we have to do manual alignment.
				 */
				aligned_fp_area = (phys_bytes) &rp->p_fpu_state.fpu_image;
				if (aligned_fp_area % FPUALIGN) {
					aligned_fp_area += FPUALIGN - (aligned_fp_area % FPUALIGN);
				}

				rp->p_fpu_state.fpu_save_area_p = (void *) aligned_fp_area;
			}
		} else {
			osfxsr_feature = 0;
		}
	} else {
		/* No FPU presents. */
		fpu_presence = 0;
		osfxsr_feature = 0;
		return;
	}

#ifdef CONFIG_X86_LOCAL_APIC
	/*
	 * this is setting kernel segments to cover most of the phys memory. The
	 * value is high enough to reach local APIC nad IOAPICs before paging is
	 * turned on.
	 */
	prot_set_kern_seg_limit(0xfff00000);
	reload_ds();
#endif

	idt_init();

	tss_init(&tss, &tss_stack_top, 0);

#if defined(CONFIG_X86_LOCAL_APIC) && !defined(CONFIG_SMP)
	if (config_no_apic) {
		BOOT_VERBOSE(printk("APIC disabled, using legacy PIC\n"));
	}
	else if (!apic_single_cpu_init()) {
		BOOT_VERBOSE(printk("APIC not present, using legacy PIC\n"));
	}
#endif
}

#define COM1_BASE	0x3F8
#define COM1_THR	(COM1_BASE + 0)
#define COM1_RBR	(COM1_BASE + 0)
#define COM1_LSR	(COM1_BASE + 5)
#define      LSR_DR		0x01
#define      LSR_THRE		0x20

void ser_putc(char c)
{
	int i;
	int lsr, thr;

	lsr= COM1_LSR;
	thr= COM1_THR;
	for (i= 0; i<100000; i++) {
		if (inb( lsr) & LSR_THRE)
			break;
	}

	outb( thr, c);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
void do_ser_debug()
{
	u8_t c, lsr;

	lsr= inb(COM1_LSR);
	if (!(lsr & LSR_DR))
		return;
	c = inb(COM1_RBR);
	ser_debug(c);
}

static void ser_dump_queues(void)
{
	int q;
	for(q = 0; q < NR_SCHED_QUEUES; q++) {
		struct proc *p;

		if(rdy_head[q])
			printk("%2d: ", q);

		for(p = rdy_head[q]; p; p = p->p_nextready) {
			printk("%s / %d  ", p->p_name, p->p_endpoint);
		}
		printk("\n");
	}
}

static void ser_dump_segs(void)
{
	struct proc *pp;
	for (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
	{
		if (isemptyp(pp))
			continue;
		printk("%d: %s ep %d\n", proc_nr(pp), pp->p_name, pp->p_endpoint);
		printseg("cs: ", 1, pp, pp->p_reg.cs);
		printseg("ds: ", 0, pp, pp->p_reg.ds);

		if(pp->p_reg.ss != pp->p_reg.ds) {
			printseg("ss: ", 0, pp, pp->p_reg.ss);
		}
	}
}

static void ser_debug(int c)
{
	int u = 0;

	do_serial_debug++;
	/* Disable interrupts so that we get a consistent state. */
	if(!intr_disabled()) { lock; u = 1; };

	switch(c)
	{
	case '1':
		ser_dump_proc();
		break;
	case '2':
		ser_dump_queues();
		break;
	case '3':
		ser_dump_segs();
		break;
#ifdef CONFIG_DEBUG_KERNEL_TRACE
#define TOGGLECASE(ch, flag)					\
	case ch: {						\
			if(verboseflags & flag) {		\
				verboseflags &= ~flag;		\
				printk("%s disabled\n", #flag);	\
			} else {				\
				verboseflags |= flag;		\
				printk("%s enabled\n", #flag);	\
			}					\
			break;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
	}
	do_serial_debug--;
	if(u) { unlock; }
}

static void printslot(struct proc *pp, int level)
{
	struct proc *depproc = NULL;
	int dep = ENDPT_NONE;
#define COL { int i; for(i = 0; i < level; i++) printk("> "); }

	if(level >= NR_PROCS) {
		printk("loop??\n");
		return;
	}

	COL

	printk("%d: %s %d prio %d/%d time %d/%d cr3 0x%lx rts %s misc %s",
		proc_nr(pp), pp->p_name, pp->p_endpoint,
		pp->p_priority, pp->p_max_priority, pp->p_user_time,
		pp->p_sys_time, pp->p_seg.p_cr3,
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags));

	if(pp->p_rts_flags & RTS_SENDING) {
		dep = pp->p_sendto_e;
		printk(" to: ");
	} else if(pp->p_rts_flags & RTS_RECEIVING) {
		dep = pp->p_getfrom_e;
		printk(" from: ");
	}

	if(dep != ENDPT_NONE) {
		if(dep == ENDPT_ANY) {
			printk(" ENDPT_ANY\n");
		} else {
			int procno;
			if(!isokendpt(dep, &procno)) {
				printk(" ??? %d\n", dep);
			} else {
				depproc = proc_addr(procno);
				if(isemptyp(depproc)) {
					printk(" empty slot %d???\n", procno);
					depproc = NULL;
				} else {
					printk(" %s\n", depproc->p_name);
				}
			}
		}
	} else {
		printk("\n");
	}

	COL
	proc_stacktrace(pp);

	if(depproc)
		printslot(depproc, level+1);
}

void ser_dump_proc()
{
	struct proc *pp;

	for (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
	{
		if (isemptyp(pp))
			continue;
		printslot(pp, 0);
	}
}

#ifdef CONFIG_DEBUG_KERNEL_STATS_PROFILE

int arch_init_profile_clock(u32_t freq)
{
	int r;
	/* Set CMOS timer frequency. */
	outb(RTC_INDEX, RTC_REG_A);
	outb(RTC_IO, RTC_A_DV_OK | freq);
	/* Enable CMOS timer interrupts. */
	outb(RTC_INDEX, RTC_REG_B);
	r = inb(RTC_IO);
	outb(RTC_INDEX, RTC_REG_B); 
	outb(RTC_IO, r | RTC_B_PIE);
	/* Mandatory read of CMOS register to enable timer interrupts. */
	outb(RTC_INDEX, RTC_REG_C);
	inb(RTC_IO);

	return CMOS_CLOCK_IRQ;
}

void arch_stop_profile_clock(void)
{
	int r;
	/* Disable CMOS timer interrupts. */
	outb(RTC_INDEX, RTC_REG_B);
	r = inb(RTC_IO);
	outb(RTC_INDEX, RTC_REG_B);  
	outb(RTC_IO, r & ~RTC_B_PIE);
}

void arch_ack_profile_clock(void)
{
	/* Mandatory read of CMOS register to re-enable timer interrupts. */
	outb(RTC_INDEX, RTC_REG_C);
	inb(RTC_IO);
}

#endif /* CONFIG_DEBUG_KERNEL_STATS_PROFILE */

#define COLOR_BASE	0xB8000L

void cons_setc(int pos, int c)
{
	char ch;

	ch= c;
	phys_copy(COLOR_BASE+(20*80+pos)*2, vir2phys((vir_bytes)&ch), 1);
}

void cons_seth(int pos, int n)
{
	n &= 0xf;
	if (n < 10)
		cons_setc(pos, '0'+n);
	else
		cons_setc(pos, 'A'+(n-10));
}

/* Saved by mpx386.s into these variables. */
u32_t params_size, params_offset, mon_ds;

void arch_copy_cmdline_params(char *buf, struct boot_params *params)
{
	if (!buf)
		kernel_panic("Invalid buffer address for command-line params", NO_NUM);

	phys_copy(vir2phys(buf), params->hdr.cmd_line_ptr, COMMAND_LINE_SIZE);
}

void arch_copy_boot_params(struct boot_params *params, u32 real_mode_params)
{
	if (!params)
		kernel_panic("Invalid buffer address for boot params", NO_NUM);

	phys_copy(vir2phys(params), real_mode_params, sizeof(struct boot_params));
}

void arch_do_syscall(struct proc *proc)
{
	/* Perform a previously postponed system call. */
	int call_nr, src_dst_e;
	kipc_msg_t *m_ptr;
	u32 bit_map;

	/* Get the system call parameters from their respective registers. */
	call_nr = proc->p_reg.cx;
	src_dst_e = proc->p_reg.retreg;
	m_ptr = (kipc_msg_t *) proc->p_reg.bx;
	bit_map = proc->p_reg.dx;

	/* kipc_call() expects the given process's memory to be accessible. */
	vm_set_cr3(proc);

	/* Make the system call, for real this time. */
	proc->p_reg.retreg = kipc_call(call_nr, bit_map, src_dst_e, m_ptr);
}

struct proc * arch_finish_schedcheck(void)
{
	char * stk;
	stk = (char *)tss.sp0;
	/* set pointer to the process to run on the stack */
	*((reg_t *)stk) = (reg_t) proc_ptr;
	return proc_ptr;
}

void restore_regs_syscall_0x80(struct proc *proc)
{
	int wait_waitpid = 0;
	int __user *p_status = 0;
	int status = 0;
	void __user *p_data = 0;
	long data = 0;

	if (!proc->syscall_0x80)
		return;

	if (proc->syscall_0x80 == __NR_sigreturn) {
		/* Get the saved return value */
		proc->p_reg.retreg = proc->clobregs[CLOBB_REG_EAX];
		proc->ret_from_sig = 0;
		proc->syscall_0x80 = 0;

		return;
	}

	proc->p_reg.retreg = proc->p_delivermsg.m_type;

	/* When the process returns from signal we must save its value
	 * otherwise the sigreturn (which also calls sigprocmask) overrides
	 * the return value saved in %eax register.
	 */
	if (proc->ret_from_sig == 1) {
		proc->clobregs[CLOBB_REG_EAX] = proc->p_reg.retreg;
		/* Set to 2 so the first value is not overwritten
		 * by functions called from sigreturn. Only the
		 * next signal could change it.
		 */
		proc->ret_from_sig = 2;
	}

	switch (proc->syscall_0x80) {
	case __NR_wait:
		wait_waitpid = 1;
		status = proc->p_delivermsg.m_data1;
		p_status = (int*)proc->clobregs[CLOBB_REG_EBX];
		break;

	case __NR_waitpid:
		wait_waitpid = 1;
		status = proc->p_delivermsg.m_data1;
		p_status = (int*)proc->clobregs[CLOBB_REG_ECX];
		break;

	case __NR_ptrace: {
		/* request is saved in %ebx register */
		int req = proc->clobregs[CLOBB_REG_EBX];

		if (req > 0 && req < 4) {
			/* The T_GETINS, T_GETDATA, T_GETUSER cases. */

			/* @nucleos: The current implementation (in PM) saves
			 *           the return value into m_data5 member which
			 *           was used as the address of user `data'.
			 *           This `data' is passed via %esi register
			 *           which is saved (as others GP registers).
			 */
			p_data = (void*)proc->clobregs[CLOBB_REG_ESI];
			data = proc->p_delivermsg.m_data5;

			/* copy data into user-space */
			if (p_data && copy_to_user(p_data, &data, sizeof(long)))
				proc->p_reg.retreg = -EFAULT;
		}

		break;
		}
	}

	/* copy status into user-space */
	if (wait_waitpid && p_status && copy_to_user(p_status, &status, sizeof(int)))
		proc->p_reg.retreg = -EFAULT;

	proc->p_reg.bx = proc->clobregs[CLOBB_REG_EBX];
	proc->p_reg.cx = proc->clobregs[CLOBB_REG_ECX];
	proc->p_reg.dx = proc->clobregs[CLOBB_REG_EDX];
	proc->p_reg.si = proc->clobregs[CLOBB_REG_ESI];
	proc->p_reg.di = proc->clobregs[CLOBB_REG_EDI];
	proc->p_reg.fp = proc->clobregs[CLOBB_REG_EBP];

	proc->syscall_0x80 = 0;

	return;
}
