# Makefile for kernel
app-y := kernel.elf32
kernel.elf32-obj-y := apic/
kernel.elf32-obj-y += arch_do_vmctl.o clock.o do_int86.o do_iopenable.o \
		      do_readbios.o do_sdevio.o exception.o i8259.o \
		      klib386.o memory.o mpx386.o protect.o system.o \
		      map_scall.o kernel-syms.o apic/built-in.o

ccflags-y := -D__KERNEL__
asflags-y := -D__KERNEL__

kernel.elf32-extobj-y := kernel/built-in.o

# In case of virtual memory this is a kernel virtual memory
KERN_PAGE_OFFSET    = 0x0000
KERN_PAGE_ALIGNMENT = 0x1000
KERN_LOAD_PHYSICAL_ADDR = 0x0000
KERN_ENTRY = nucleos_startup

targets := nucleos_32.lds

CPPFLAGS_nucleos_32.lds := -D__PAGE_OFFSET__=$(KERN_PAGE_OFFSET) \
			   -D__PAGE_ALIGNMENT__=$(KERN_PAGE_ALIGNMENT) \
			   -D__LOAD_PHYSICAL_ADDR__=$(KERN_LOAD_PHYSICAL_ADDR)
CPPFLAGS_nucleos_32.lds += -P
CPPFLAGS_nucleos_32.lds += -D__KERNEL__

# generate linker script
$(src)/kernel.elf32: arch/x86/kernel/nucleos_32.lds

LDFLAGS_kernel.elf32 := -N -S -e $(KERN_ENTRY) \
			-L$(lib-arch) -Llib \
			-ltimers -lsys -lnucc \
			-Tarch/x86/kernel/nucleos_32.lds

# Create in minix3's aout format
kernel_a_flags   := $(CONFIG_KERNEL_LINKAGE_TYPE)
kernel_a_hdrlen  := 0x20
kernel_a_cpu     := i386
kernel_stackheap := 0

e2a-y := kernel.elf32,kernel
