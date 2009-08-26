app-y := amddev.elf32
amddev.elf32-type := driver
amddev.elf32-obj-y := amddev.o

LDFLAGS_amddev.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/amddev.elf32: drivers/driver_32.lds

# Create in minix3's aout format
amddev_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
amddev_a_hdrlen  := 0x20
amddev_a_cpu     := i386
amddev_stackheap := 320k

e2a-y := amddev.elf32,amddev
