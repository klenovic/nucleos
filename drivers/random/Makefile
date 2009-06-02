# Makefile for random driver (RANDOM)
app-y := random.elf32
random.elf32-obj-y := main.o random.o sha2.o aes/rijndael_api.o aes/rijndael_alg.o 

random.elf32-extobj-y := lib/$(SRCARCH)/rts/crtso.o

LDFLAGS_random.elf32 := -Llib -ldriver_asyn -lsys -lnucc \
	                       -L$(cc-lib-dir) -lgcc \
	                       -Tdrivers/driver_32.lds

$(src)/random.elf32: drivers/driver_32.lds

# Create in minix3's aout format
random_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
random_a_hdrlen  := 0x20
random_a_cpu     := i386
random_stackheap := 0x2000

e2a-y := random.elf32,random
