# Makefile for the AT disk driver (AT_WINI)
app-y := floppy.elf32
floppy.elf32-obj-y := floppy.o

floppy.elf32-extobj-y := $(lib-arch)/rts/crtso.o

LDFLAGS_floppy.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-L$(cc-lib-dir) -lgcc \
			-Tdrivers/driver_32.lds

$(src)/floppy.elf32: drivers/driver_32.lds

# Create in minix3's aout format
floppy_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
floppy_a_hdrlen  := 0x20
floppy_a_cpu     := i386
floppy_stackheap := 0x4000

e2a-y := floppy.elf32,floppy
