app-y := filter.elf32
filter.elf32-type := driver
filter.elf32-obj-y := crc.o driver.o main.o md5.o optset.o sum.o util.o

LDFLAGS_filter.elf32 := -L$(lib-arch) -Llib \
			-lsys -lnucc \
			-Tdrivers/driver_32.lds

ccflags-y += -DDEBUG=1

$(src)/filter.elf32: drivers/driver_32.lds

# Create in minix3's aout format
filter_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
filter_a_hdrlen  := 0x20
filter_a_cpu     := i386
filter_stackheap := 32k

e2a-y := filter.elf32,filter
