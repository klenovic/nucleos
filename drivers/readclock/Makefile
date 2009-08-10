# Makefile for readclock 'driver'
app-y := readclock.drv.elf32
readclock.drv.elf32-type := driver
readclock.drv.elf32-obj-y := readclock.o

ccflags-y := -D_SYSTEM=1

LDFLAGS_readclock.drv.elf32 := -L$(lib-arch) -Llib \
			       -lsys -lnucc \
			       -Tdrivers/driver_32.lds

$(src)/readclock.drv.elf32: drivers/driver_32.lds

# Create in minix3's aout format
readclock.drv_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
readclock.drv_a_hdrlen  := 0x20
readclock.drv_a_cpu     := i386
readclock.drv_stackheap := 16k

e2a-y := readclock.drv.elf32,readclock.drv
