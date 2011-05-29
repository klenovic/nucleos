# Makefile for the ES1371 sounddriver (SB16)
app-y := es1370.elf32
es1370.elf32-type := driver
es1370.elf32-obj-y := es1370.o ak4531.o pci_helper.o
es1370.elf32-extobj-y := $(audio-framework)/audio_fw.o

LDFLAGS_es1370.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/es1370.elf32: drivers/driver_32.lds

# Create in minix3's aout format
es1370_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
es1370_a_hdrlen  := 0x20
es1370_a_cpu     := i386
es1370_stackheap := 1024k

e2a-y := es1370.elf32,es1370
