# Makefile for Reincarnation Server (RS)
app-y := rs.elf32
rs.elf32-type := server
rs.elf32-obj-y := exec.o main.o manager.o table.o utility.o memory.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_rs.elf32 := -L$(lib-arch) -Llib \
		    -lsys -lnucc \
		    -Tservers/server_32.lds

$(src)/rs.elf32: servers/server_32.lds 

# Create in minix3's aout format
rs_a_flags   := 0x00
rs_a_hdrlen  := 0x20
rs_a_cpu     := i386
rs_stackheap := 350k

e2a-y := rs.elf32,rs
