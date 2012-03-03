# Makefile for Centronics printer driver (PRINTER)
app-y := printer.elf32
printer.elf32-type := driver
printer.elf32-obj-y := printer.o

LDFLAGS_printer.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/printer.elf32: drivers/driver_32.lds

# Create in minix3's aout format
printer_a_flags   := 0x00
printer_a_hdrlen  := 0x20
printer_a_cpu     := i386
printer_stackheap := 256k

e2a-y := printer.elf32,printer
