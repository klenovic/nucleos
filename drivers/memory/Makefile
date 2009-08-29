# Makefile for memory driver (MEMORY)
app-y := memory.elf32
memory.elf32-type := driver
memory.elf32-obj-y := allocmem.o memory.o initrd.o

# Compile the initial ramdisk into memory driver
memory.elf32-extobj-$(CONFIG_BUILTIN_INITRD) += $(wildcard $(srctree)/init/initrd.o)

LDFLAGS_memory.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/memory.elf32: drivers/driver_32.lds

# Create in minix3's aout format
memory_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
memory_a_hdrlen  := 0x20
memory_a_cpu     := i386
memory_stackheap := 8k

e2a-y := memory.elf32,memory
