# Makefile for the AT disk driver (AT_WINI)
app-y := rtl8139.elf32
rtl8139.elf32-type := driver
rtl8139.elf32-obj-y := rtl8139.o

LDFLAGS_rtl8139.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/rtl8139.elf32: drivers/driver_32.lds

# Create in minix3's aout format
rtl8139_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
rtl8139_a_hdrlen  := 0x20
rtl8139_a_cpu     := i386
rtl8139_stackheap := 64kw

e2a-y := rtl8139.elf32,rtl8139
