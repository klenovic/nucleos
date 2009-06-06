# Makefile for readclock 'driver'
app-y := readclock.drv.elf32
readclock.drv.elf32-obj-y := readclock.o
readclock.drv.elf32-extobj-y := $(lib-arch)/rts/crtso.o

ccflags-y := -D_MINIX=1 -D_POSIX_SOURCE=1 -D_SYSTEM=1

LDFLAGS_readclock.drv.elf32 := -L$(lib-arch) -Llib \
			       -lsys -lnucc \
			       -Tdrivers/driver_32.lds

$(src)/readclock.drv.elf32: drivers/driver_32.lds

# Create in minix3's aout format
readclock.drv_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
readclock.drv_a_hdrlen  := 0x20
readclock.drv_a_cpu     := i386
readclock.drv_stackheap := 0x4000

e2a-y := readclock.drv.elf32,readclock.drv
