app-y := dp8390.elf32
dp8390.elf32-type := driver
dp8390.elf32-obj-y := 3c503.o dp8390.o ne2000.o rtl8029.o wdeth.o

LDFLAGS_dp8390.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/dp8390.elf32: drivers/driver_32.lds

# Create in minix3's aout format
dp8390_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
dp8390_a_hdrlen  := 0x20
dp8390_a_cpu     := i386
dp8390_stackheap := 64k

e2a-y := dp8390.elf32,dp8390
