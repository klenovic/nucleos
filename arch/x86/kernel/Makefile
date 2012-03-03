# Makefile for kernel
app-y := kernel.elf32
kernel.elf32-obj-y := apic/ cpu/
kernel.elf32-obj-y += arch_do_vmctl.o clock.o do_int86.o do_iopenable.o \
		      do_readbios.o do_sdevio.o exception.o i8259.o \
		      klib_32.o memory.o head_32.o protect.o setup.o \
		      system.o map_scall.o kernel-syms.o header.o version.o \
		      start.o apic/built-in.o

ccflags-y := -D__KERNEL__
asflags-y := -D__KERNEL__

kernel.elf32-extobj-y := kernel/built-in.o

# In case of virtual memory this is a kernel virtual memory
KERN_ENTRY = _start

targets := nucleos_32.lds

CPPFLAGS_nucleos_32.lds += -P
CPPFLAGS_nucleos_32.lds += -D__KERNEL__

# generate linker script
$(src)/kernel.elf32: arch/x86/kernel/nucleos_32.lds

LDFLAGS_kernel.elf32 := -N -S -e $(KERN_ENTRY) \
			-L$(lib-arch) -Llib \
			-ltimers -lsys -lnucc \
			-Tarch/x86/kernel/nucleos_32.lds

# Create in minix3's aout format
kernel_a_flags   := 0x00
kernel_a_hdrlen  := 0x20
kernel_a_cpu     := i386
kernel_stackheap := 0

e2a-y := kernel.elf32,kernel
