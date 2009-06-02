# Makefile for Data Store Server (DS)
app-y := ds.elf32
ds.elf32-obj-y := main.o store.o
ds.elf32-extobj-y :=	lib/x86/rts/crtso.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_ds.elf32 := -Llib -lsys -lnucc \
	                  -Tservers/server_32.lds

$(src)/ds.elf32: servers/server_32.lds

# Create in minix3's aout format
ds_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
ds_a_hdrlen  := 0x20
ds_a_cpu     := i386
ds_stackheap := 0x4000

e2a-y        := ds.elf32,ds
