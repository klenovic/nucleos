# Makefile for the Texas Instruments PCI1225 PC Card controller driver (ti1225)
app-y := ti1225.elf32
ti1225.elf32-type := driver
ti1225.elf32-obj-y := ti1225.o

LDFLAGS_ti1225.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/ti1225.elf32: drivers/driver_32.lds

# Create in minix3's aout format
ti1225_a_flags   := 0x00
ti1225_a_hdrlen  := 0x20
ti1225_a_cpu     := i386
ti1225_stackheap := 4k

e2a-y := ti1225.elf32,ti1225
