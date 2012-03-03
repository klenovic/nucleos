# Makefile for ISA ethernet drivers	May  02, 2000
app-y := dpeth.elf32
dpeth.elf32-type := driver
dpeth.elf32-obj-y := 3c501.o 3c509.o 3c503.o ne.o wd.o 8390.o devio.o netbuff.o dp.o

LDFLAGS_dpeth.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/dpeth.elf32: drivers/driver_32.lds

# Create in minix3's aout format
dpeth_a_flags   := 0x00
dpeth_a_hdrlen  := 0x20
dpeth_a_cpu     := i386
dpeth_stackheap := 16k

e2a-y := dpeth.elf32,dpeth
