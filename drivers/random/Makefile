# Makefile for random driver (RANDOM)
app-y := random.elf32
random.elf32-type := driver
random.elf32-obj-y := main.o random.o sha2.o aes/

random.elf32-obj-y += aes/built-in.o

LDFLAGS_random.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/random.elf32: drivers/driver_32.lds

# Create in minix3's aout format
random_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
random_a_hdrlen  := 0x20
random_a_cpu     := i386
random_stackheap := 8k

e2a-y := random.elf32,random
