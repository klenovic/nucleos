# Makefile for the BIOS disk driver (BIOS_WINI)
app-y := bios_wini.elf32
bios_wini.elf32-type := driver
bios_wini.elf32-obj-y := bios_wini.o

LDFLAGS_bios_wini.elf32 := -L$(lib-arch) -Llib \
			   -ldriver -lsys -ltimers -lnucc \
			   -Tdrivers/driver_32.lds

$(src)/bios_wini.elf32: drivers/driver_32.lds

# Create in minix3's aout format
bios_wini_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
bios_wini_a_hdrlen  := 0x20
bios_wini_a_cpu     := i386
bios_wini_stackheap := 8k

e2a-y := bios_wini.elf32,bios_wini
