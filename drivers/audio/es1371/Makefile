# Makefile for the ES1371 sounddriver (SB16)
app-y := es1371.elf32
es1371.elf32-type := driver
es1371.elf32-obj-y := es1371.o AC97.o pci_helper.o wait.o sample_rate_converter.o
es1371.elf32-extobj-y := $(audio-framework)/audio_fw.o

LDFLAGS_es1371.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/es1371.elf32: drivers/driver_32.lds

# Create in minix3's aout format
es1371_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
es1371_a_hdrlen  := 0x20
es1371_a_cpu     := i386
es1371_stackheap := 1024k

e2a-y := es1371.elf32,es1371
