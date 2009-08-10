# Makefile for the init program (init)
app-y := init.elf32
init.elf32-type := server
init.elf32-obj-y := init.o

ccflags-y += -O2

LDFLAGS_init.elf32 := -L$(lib-arch) -Llib \
		      -lnucc \
		      -Tservers/server_32.lds

$(src)/init.elf32: servers/server_32.lds 

# Create in minix3's aout format
init_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
init_a_hdrlen  := 0x20
init_a_cpu     := i386
init_stackheap := 64k

e2a-y := init.elf32,init
