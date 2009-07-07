/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// This file, mpx386.s, is included by mpx.s when Minix is compiled for 
// 32-bit Intel CPUs. The alternative mpx88.s is compiled for 16-bit CPUs.

// This file is part of the lowest layer of the Nucleos kernel.  (The other part
// is "proc.c".)  The lowest layer does process switching and message handling.
// Furthermore it contains the assembler startup code for Minix and the 32-bit
// interrupt handlers.  It cooperates with the code in "start.c" to set up a 
// good environment for main().

// Every transition to the kernel goes through this file.  Transitions to the 
// kernel may be nested.  The initial entry may be with a system call (i.e., 
// send or receive a message), an exception or a hardware interrupt;  kernel 
// reentries may only be made by hardware interrupts.  The count of reentries 
// is kept in "k_reenter". It is important for deciding whether to switch to 
// the kernel stack and for protecting the message passing code in "proc.c".

// For the message passing trap, most of the machine state is saved in the
// proc table.  (Some of the registers need not be saved.)  Then the stack is
// switched to "k_stack", and interrupts are reenabled.  Finally, the system
// call handler (in C) is called.  When it returns, interrupts are disabled
// again and the code falls into the restart routine, to finish off held-up
// interrupts and run the process or task whose pointer is in "proc_ptr".

// Hardware interrupt handlers do the same, except  (1) The entire state must
// be saved.  (2) There are too many handlers to do this inline, so the save
// routine is called.  A few cycles are saved by pushing the address of the
// appropiate restart routine for a return later.  (3) A stack switch is
// avoided when the stack is already switched.  (4) The (master) 8259 interrupt
// controller is reenabled centrally in save().  (5) Each interrupt handler
// masks its interrupt line using the 8259 before enabling (other unmasked)
// interrupts, and unmasks it after servicing the interrupt.  This limits the
// nest level to the number of lines and protects the handler from itself.

// For communication with the boot monitor at startup time some constant
// data are compiled into the beginning of the text segment. This facilitates 
// reading the data at the start of the boot process, since only the first
// sector of the file needs to be read.

// Some data storage is also allocated at the end of this file. This data 
// will be at the start of the data segment of the kernel and will be read
// and modified by the boot monitor before the kernel starts.

#include <sys/vm_i386.h>

#include <nucleos/const.h>
#include <nucleos/com.h>
#include <ibm/interrupt.h>
#include <asm/kernel/const.h>
#include <kernel/const.h>
#include <asm/page.h>

#include "vm.S"
#include "sconst.S"

/* Selected 386 tss offsets. */
#define TSS3_S_SP0      4

// Exported functions
// Note: in assembly language the .globl statement applied to a function name 
// is loosely equivalent to a prototype in C code -- it makes it possible to
// link to an entity declared in the assembly code but does not create
// the entity.

.globl  restart
.globl  save

.globl  kernel_cr3
.globl  pagefault_cr2
.globl  pagefault_count

.globl  errexception
.globl  xception1
.globl  exception

.globl  divide_error
.globl  single_step_exception
.globl  nmi
.globl  breakpoint_exception
.globl  overflow
.globl  bounds_check
.globl  inval_opcode
.globl  copr_not_available
.globl  double_fault
.globl  copr_seg_overrun
.globl  inval_tss
.globl  segment_not_present
.globl  stack_exception
.globl  general_protection
.globl  page_fault
.globl  copr_error

.globl  params_size
.globl  params_offset
.globl  mon_ds

.globl  hwint00        // handlers for hardware interrupts
.globl  hwint01
.globl  hwint02
.globl  hwint03
.globl  hwint04
.globl  hwint05
.globl  hwint06
.globl  hwint07
.globl  hwint08
.globl  hwint09
.globl  hwint10
.globl  hwint11
.globl  hwint12
.globl  hwint13
.globl  hwint14
.globl  hwint15

.globl  s_call
.globl  p_s_call
.globl  level0_call

/* @klenovic: All these flags are used and defined by monitor. So this
           can lead into inconsistence if we change something there
           but forget to change also here! */

/* legacy features (see monitor sources) */
#define K_FLAGS			0x01FD

#ifdef CONFIG_BUILTIN_INITRD
#define K_BUILTIN_INITRD	0x0001
#else
#define K_BUILTIN_INITRD	0x0000
#endif

#define K_FLAGS_EXT	K_BUILTIN_INITRD

.section .text.head
//===========================================================================
//                         nucleos_startup
//===========================================================================
.globl nucleos_startup
nucleos_startup:                // this is the entry point for the Nucleos kernel
/* @klenovic: Do not put anything here otherwise boot will not find CLICK_SHIFT
           These "magic" numbers look a bit jerky and unflexible... passing
           kernel options/features via commad line or something similar would be
           cool. */

        jmp     over_flags      // skip over the next few bytes
        .word   CLICK_SHIFT     // for the monitor: memory granularity
flags:
        .word   K_FLAGS         // boot monitor flags:
                                //      call in 386 mode, make bss, make stack,
                                //      load high, do not patch, will return,
                                //      uses generic INT, memory vector,
                                //      new boot code return
flags_ext:
        .word   K_FLAGS_EXT     // boot monitor extended flags:
                                //      built in initrd (in memory driver)
        nop                     // extra byte to sync up disassembler

over_flags:
// Set up a C stack frame on the monitor stack.  (The monitor sets cs and ds
// right.  The ss descriptor still references the monitor data segment.)
        movzwl  %sp, %esp         // monitor stack is a 16 bit stack
        push    %ebp
        mov     %esp, %ebp
        push    %esi
        push    %edi
        cmp     $0, 4(%ebp)      // monitor return vector is
        jz      noret            // nonzero if return possible
        incb    mon_return
noret:
        mov     %esp, mon_sp     // save stack pointer for later return

// Copy the monitor global descriptor table to the address space of kernel and
// switch over to it.  Prot_init() can then update it with immediate effect.
        sgdt    gdt+GDT_SELECTOR             // get the monitor gdtr
        mov     gdt+GDT_SELECTOR+2, %esi     // absolute address of GDT
        mov     $gdt, %ebx                   // address of kernel GDT
        mov     $8*8, %ecx                   // copying eight descriptors
copygdt:
        movb    %es:(%esi), %al
        movb    %al, (%ebx)
        inc     %esi
        inc     %ebx
        loop    copygdt
        mov     gdt+DS_SELECTOR+2, %eax  // base of kernel data
        and     $0x00FFFFFF, %eax        // only 24 bits
        add     $gdt, %eax               // eax = vir2phys(gdt)
        mov     %eax, gdt+GDT_SELECTOR+2 // set base of GDT
        lgdt    gdt+GDT_SELECTOR         // switch over to kernel GDT
// Locate boot parameters, set up kernel segment registers and stack.
        mov     8(%ebp), %ebx     // boot parameters offset
        mov     12(%ebp), %edx    // boot parameters length
        mov     16(%ebp), %eax    // address of a.out headers
        mov     %eax, aout
        mov     %ds, %ax          // kernel data
        mov     %ax, %es
        mov     %ax, %fs
        mov     %ax, %gs
        mov     %ax, %ss
        mov     $k_stktop, %esp   // set sp to point to the top of kernel stack
// Save boot parameters into these global variables for i386 code
        mov     %edx, params_size
        mov     %ebx, params_offset
        movl    $SS_SELECTOR, mon_ds

// Call C startup code to set up a proper environment to run main().
        push    %edx
        push    %ebx
        push    $SS_SELECTOR
        push    $DS_SELECTOR
        push    $CS_SELECTOR
        call    cstart         // cstart(cs, ds, mds, parmoff, parmlen)
        add     $5*4, %esp

// Reload gdtr, idtr and the segment registers to global descriptor table set
// up by prot_init().
        lgdt    gdt+GDT_SELECTOR
        lidt    gdt+IDT_SELECTOR
        ljmp    $CS_SELECTOR,$csinit
csinit:
        mov     $DS_SELECTOR, %ax
        mov     %ax, %ds
        mov     %ax, %es
        mov     %ax, %fs
        mov     %ax, %gs
        mov     %ax, %ss

//    o16 mov     $TSS_SELECTOR,%ax       // no other TSS is used
        movw     $TSS_SELECTOR, %ax     // no other TSS is used
        ltr     %ax
        push    $0                      // set flags to known good state
        popf                            // esp, clear nested task and int enable

#ifdef CONFIG_KERNEL_VM_NOPAGEZERO
       jmp     laststep
.p2align PAGE_SHIFT
laststep:
#endif
        jmp     main                    // main()

.section .text
//===========================================================================
//                             interrupt handlers
//             interrupt handlers for 386 32-bit protected mode
//===========================================================================

//===========================================================================
//                             hwint00 - 07
//===========================================================================
// Note this is a macro, it just looks like a subroutine.
#define hwint_master(irq)       \
        call    save                 /* save interrupted process state */;\
        push    irq_handlers+4*irq   /* irq_handlers[irq]              */;\
        LOADCR3WITHEAX(irq, kernel_cr3) /* switch to kernel page table */;\
        call    intr_handle          /* intr_handle(irq_handlers[irq]) */;\
        pop     %ecx                                                     ;\
        cmp     $0, irq_actids+4*irq /* interrupt still active?        */;\
        jz      0f                                                       ;\
        inb     $INT_CTLMASK         /* get current mask */              ;\
        orb     $[1<<irq], %al       /* mask irq */                      ;\
        outb    $INT_CTLMASK         /* disable the irq                */;\
0:      movb    $END_OF_INT, %al                                         ;\
        outb    $INT_CTL             /* reenable master 8259           */;\
        ret                          /* restart (another) process      */

// Each of these entry points is an expansion of the hwint_master macro
        .p2align 4
hwint00:               // Interrupt routine for irq 0 (the clock).
        hwint_master(0)
        .p2align 4
hwint01:               // Interrupt routine for irq 1 (keyboard)
        hwint_master(1)
        .p2align 4
hwint02:               // Interrupt routine for irq 2 (cascade//)
        hwint_master(2)
        .p2align 4
hwint03:               // Interrupt routine for irq 3 (second serial)
        hwint_master(3)
        .p2align 4
hwint04:               // Interrupt routine for irq 4 (first serial)
        hwint_master(4)
        .p2align 4
hwint05:               // Interrupt routine for irq 5 (XT winchester)
        hwint_master(5)
        .p2align 4
hwint06:               // Interrupt routine for irq 6 (floppy)
        hwint_master(6)
        .p2align 4
hwint07:               // Interrupt routine for irq 7 (printer)
        hwint_master(7)

//===========================================================================
//                             hwint08 - 15
//===========================================================================
// Note this is a macro, it just looks like a subroutine.
#define hwint_slave(irq)        \
        call    save                 /* save interrupted process state */;\
        push    irq_handlers+4*irq   /* irq_handlers[irq]              */;\
        LOADCR3WITHEAX(irq, kernel_cr3) /* switch to kernel page table */;\
        call    intr_handle          /* intr_handle(irq_handlers[irq]) */;\
        pop     %ecx                                                     ;\
        cmp     $0, irq_actids+4*irq /* interrupt still active?        */;\
        jz      0f                                                       ;\
        inb     $INT2_CTLMASK                                            ;\
        orb     $[1<<[irq-8]], %al                                       ;\
        outb    $INT2_CTLMASK        /* disable the irq                */;\
0:      movb    $END_OF_INT, %al                                         ;\
        outb    $INT_CTL             /* reenable master 8259           */;\
        outb    $INT2_CTL            /* reenable slave 8259            */;\
        ret                          /* restart (another) process      */

// Each of these entry points is an expansion of the hwint_slave macro
        .p2align 4
hwint08:               // Interrupt routine for irq 8 (realtime clock)
        hwint_slave(8)
        .p2align 4
hwint09:               // Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave(9)
        .p2align 4
hwint10:               // Interrupt routine for irq 10
        hwint_slave(10)
        .p2align 4
hwint11:               // Interrupt routine for irq 11
        hwint_slave(11)
        .p2align 4
hwint12:               // Interrupt routine for irq 12
        hwint_slave(12)
        .p2align 4
hwint13:               // Interrupt routine for irq 13 (FPU exception)
        hwint_slave(13)
        .p2align 4
hwint14:               // Interrupt routine for irq 14 (AT winchester)
        hwint_slave(14)
        .p2align 4
hwint15:               // Interrupt routine for irq 15
        hwint_slave(15)

//===========================================================================
//                             save
//===========================================================================
// Save for protected mode.
// This is much simpler than for 8086 mode, because the stack already points
// into the process table, or has already been switched to the kernel stack.
        .p2align 4
save:
        cld                      // set direction flag to a known value
        pusha                    // save "general" registers
        pushw    %ds             // save ds
        pushw    %es             // save es
        pushw    %fs             // save fs
        pushw    %gs             // save gs
        mov     %ss, %dx         // ss is kernel data segment
        mov     %dx, %ds         // load rest of kernel segments
        mov     %dx, %es         // kernel does not use fs, gs
        mov     %esp, %eax       // prepare to return
        incb    k_reenter        // from -1 if not reentering
        jnz     set_restart1     // stack is already kernel stack
        mov     $k_stktop, %esp
        push    $restart         // build return address for int handler
        xor     %ebp, %ebp       // for stacktrace
        jmp     *RETADR-P_STACKBASE(%eax)

        .p2align 2
set_restart1:
        push    $restart1
        jmp     *RETADR-P_STACKBASE(%eax)

//===========================================================================
//                             _s_call
//===========================================================================
        .p2align 4
s_call:
p_s_call:
        cld                      // set direction flag to a known value
        sub     $6*4, %esp       // skip RETADR, eax, ecx, edx, ebx, est
        push    %ebp             // stack already points into proc table
        push    %esi
        push    %edi
        pushw    %ds
        pushw    %es
        pushw    %fs
        pushw    %gs
        mov     %ss, %si          // ss is kernel data segment
        mov     %si, %ds          // load rest of kernel segments
        mov     %si, %es          // kernel does not use fs, gs
        incb    k_reenter         // increment kernel entry count
        mov     %esp, %esi        // assumes P_STACKBASE == 0
        mov     $k_stktop, %esp
        xor     %ebp, %ebp        // for stacktrace
                                  // end of inline save
                                  // now set up parameters for sys_call()
        push    %edx              // event set or flags bit map 
        push    %ebx              // pointer to user message
        push    %eax              // source / destination
        push    %ecx              // call number (ipc primitive to use)

//      LOADCR3WITHEAX(0x20, kernel_cr3)

        call    sys_call          // sys_call(call_nr, src_dst, m_ptr, bit_map)
                                  // caller is now explicitly in proc_ptr
        mov     %eax, AXREG(%esi) // sys_call MUST PRESERVE si

// Fall into code to restart proc/task running.

//===========================================================================
//                             restart
//===========================================================================
restart:
// Restart the current process or the next process if it is set. 
        cmp     $0, next_ptr           // see if another process is scheduled
        jz      0f
        mov     next_ptr, %eax
        mov     %eax, proc_ptr         // schedule new process 
        movl    $0, next_ptr
0:
        mov     proc_ptr, %esp         // will assume P_STACKBASE == 0
        lldt    P_LDT_SEL(%esp)        // enable process'' segment descriptors 
        LOADCR3WITHEAX(0x21, P_CR3(%esp)) // switch to process page table
        lea     P_STACKTOP(%esp), %eax // arrange for next interrupt
        mov     %eax, tss+TSS3_S_SP0   // to save state in process table
restart1:
        decb    k_reenter
        popw     %gs
        popw     %fs
        popw     %es
        popw     %ds
        popa
        add     $4, %esp        // skip return adr
        iret                    // continue process

//===========================================================================
//                             exception handlers
//===========================================================================
divide_error:
        push    $DIVIDE_VECTOR
        jmp     exception

single_step_exception:
        push    $DEBUG_VECTOR
        jmp     exception

nmi:
        push    $NMI_VECTOR
        jmp     exception

breakpoint_exception:
        push    $BREAKPOINT_VECTOR
        jmp     exception

overflow:
        push    $OVERFLOW_VECTOR
        jmp     exception

bounds_check:
        push    $BOUNDS_VECTOR
        jmp     exception

inval_opcode:
        push    $INVAL_OP_VECTOR
        jmp     exception

copr_not_available:
        push    $COPROC_NOT_VECTOR
        jmp     exception

double_fault:
        push    $DOUBLE_FAULT_VECTOR
        jmp     errexception

copr_seg_overrun:
        push    $COPROC_SEG_VECTOR
        jmp     exception

inval_tss:
        push    $INVAL_TSS_VECTOR
        jmp     errexception

segment_not_present:
        push    $SEG_NOT_VECTOR
        jmp     errexception

stack_exception:
        push    $STACK_FAULT_VECTOR
        jmp     errexception

general_protection:
        push    $PROTECTION_VECTOR
        jmp     errexception

page_fault:
        push    $PAGE_FAULT_VECTOR
        push    %eax
        mov     %cr2, %eax
        mov     %eax, %ss:pagefault_cr2
        incl    %ss:pagefault_count
        pop     %eax

        jmp     errexception

copr_error:
        push    $COPROC_ERR_VECTOR
        jmp     exception

//===========================================================================
//                             exception
//===========================================================================
// This is called for all exceptions which do not push an error code.
        .p2align 4
exception:
        movl     $0, %ss:trap_errno     // clear trap_errno
        pop     %ss:ex_number
        jmp     exception1

//===========================================================================
//                             errexception
//===========================================================================
// This is called for all exceptions which push an error code.
        .p2align 4
errexception:
        pop     %ss:ex_number
        pop     %ss:trap_errno
exception1:                           // Common for all exceptions.
        push    %eax                  // eax is scratch register
        mov     0+4(%esp), %eax       // old eip
        mov     %eax, %ss:old_eip
        movzwl  4+4(%esp), %eax       // old cs
        mov     %eax, %ss:old_cs
        mov     8+4(%esp), %eax       // old eflags
        mov     %eax, %ss:old_eflags

        LOADCR3WITHEAX(0x24, kernel_cr3)

        pop     %eax
        call    save
        push    old_eflags
        push    old_cs
        push    old_eip
        push    trap_errno
        push    ex_number
        call    exception_handler      // (ex_number, trap_errno, old_eip,
                                       //      old_cs, old_eflags)
                                       // @klenovic: defined inside C source
        add     $5*4, %esp
        ret

//===========================================================================
//                            level0_call
//===========================================================================
level0_call:
        call    save
        jmp     *level0_func
//===========================================================================
//                             load_kernel_cr3
//===========================================================================
.p2align 4
load_kernel_cr3:
       mov     kernel_cr3, %eax
       mov     %eax, %cr3
       ret

//===========================================================================
//                             rodata
//===========================================================================
.section .bss

/* Kernel stack */
k_stack:
	.space   CONFIG_KERNEL_STACK_SIZE  

/* Top of kernel stack */
k_stktop:
	.lcomm   ex_number, 4
	.lcomm   trap_errno, 4
	.lcomm   old_eip, 4
	.lcomm   old_cs, 4
	.lcomm   old_eflags, 4
