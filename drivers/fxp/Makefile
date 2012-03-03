# Makefile for Intel Pro/100 driver (FXP)
app-y := fxp.elf32
fxp.elf32-type := driver
fxp.elf32-obj-y := fxp.o mii.o

LDFLAGS_fxp.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/fxp.elf32: drivers/driver_32.lds

# Create in minix3's aout format
fxp_a_flags   := 0x00
fxp_a_hdrlen  := 0x20
fxp_a_cpu     := i386
fxp_stackheap := 4k

e2a-y := fxp.elf32,fxp
