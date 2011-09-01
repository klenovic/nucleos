app-y := e1000.elf32
e1000.elf32-type := driver
e1000.elf32-obj-y := e1000.o

LDFLAGS_e1000.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/e1000.elf32: drivers/driver_32.lds

# Create in minix3's aout format
e1000_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
e1000_a_hdrlen  := 0x20
e1000_a_cpu     := i386
e1000_stackheap := 128k

e2a-y := e1000.elf32,e1000
