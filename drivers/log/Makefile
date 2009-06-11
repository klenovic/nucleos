# Makefile for log driver
app-y := log.elf32
log.elf32-type := driver
log.elf32-obj-y := log.o diag.o

LDFLAGS_log.elf32 := -L$(lib-arch) -Llib \
		     -ldriver_asyn -lsys -lnucc \
		     -L$(cc-lib-dir) -lgcc \
		     -Tdrivers/driver_32.lds

$(src)/log.elf32: drivers/driver_32.lds

# Create in minix3's aout format
log_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
log_a_hdrlen  := 0x20
log_a_cpu     := i386
log_stackheap := 32k

e2a-y := log.elf32,log
