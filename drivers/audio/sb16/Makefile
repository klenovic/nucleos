# Makefile for the Sound Blaster 16 driver (SB16)
app-y := sb16.elf32
sb16.elf32-type := driver
sb16.elf32-obj-y := sb16.o mixer.o
sb16.elf32-extobj-y:= $(audio-framework)/audio_fw.o

LDFLAGS_sb16.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/sb16.elf32: drivers/driver_32.lds

# Create in minix3's aout format
sb16_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
sb16_a_hdrlen  := 0x20
sb16_a_cpu     := i386
sb16_stackheap := 512k

e2a-y := sb16.elf32,sb16
