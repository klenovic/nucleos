# Makefile for the Sound Blaster 16 driver (SB16)
app-y := sb16_dsp.elf32
sb16_dsp.elf32-type := driver
sb16_dsp.elf32-obj-y := sb16.o sb16_dsp.o

LDFLAGS_sb16_dsp.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/sb16_dsp.elf32: drivers/driver_32.lds

# Create in minix3's aout format
sb16_dsp_a_flags   := 0x00
sb16_dsp_a_hdrlen  := 0x20
sb16_dsp_a_cpu     := i386
sb16_dsp_stackheap := 128k

e2a-y := sb16_dsp.elf32,sb16_dsp

app-y += sb16_mixer.elf32
sb16_mixer.elf32-type := driver
sb16_mixer.elf32-obj-y := sb16.o sb16_mixer.o

LDFLAGS_sb16_mixer.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/sb16_mixer.elf32: drivers/driver_32.lds

# Create in minix3's aout format
sb16_mixer_a_flags   := 0x00
sb16_mixer_a_hdrlen  := 0x20
sb16_mixer_a_cpu     := i386
sb16_mixer_stackheap := 128k

e2a-y += sb16_mixer.elf32,sb16_mixer

