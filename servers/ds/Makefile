# Makefile for Data Store Server (DS)
app-y := ds.elf32
ds.elf32-type := server
ds.elf32-obj-y := main.o store.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_ds.elf32 := -L$(lib-arch) -Llib \
		    -lsys -lnucc \
		    -Tservers/server_32.lds

$(src)/ds.elf32: servers/server_32.lds

# Create in minix3's aout format
ds_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
ds_a_hdrlen  := 0x20
ds_a_cpu     := i386
ds_stackheap := 16k

e2a-y        := ds.elf32,ds
