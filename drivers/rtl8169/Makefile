# Makefile for the AT disk driver (AT_WINI)
app-y := rtl8169.elf32
rtl8169.elf32-type := driver
rtl8169.elf32-obj-y := rtl8169.o

LDFLAGS_rtl8169.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/rtl8169.elf32: drivers/driver_32.lds

# Create in minix3's aout format
rtl8169_a_flags   := 0x00
rtl8169_a_hdrlen  := 0x20
rtl8169_a_cpu     := i386
rtl8169_stackheap := 64kw

e2a-y := rtl8169.elf32,rtl8169
