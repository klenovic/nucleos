# Makefile for the Orinoco wireless card (Prism chipset)
app-y := orinoco.elf32
orinoco.elf32-type := driver
orinoco.elf32-obj-y := orinoco.o hermes.o

LDFLAGS_orinoco.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -ltimers -lnucc \
			-Tdrivers/driver_32.lds

$(src)/orinoco.elf32: drivers/driver_32.lds

# Create in minix3's aout format
orinoco_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
orinoco_a_hdrlen  := 0x20
orinoco_a_cpu     := i386
orinoco_stackheap := 400k

e2a-y := orinoco.elf32,orinoco
