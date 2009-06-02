# Makefile for Reincarnation Server (RS)
app-y := rs.elf32
rs.elf32-obj-y := exec.o main.o manager.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

rs.elf32-extobj-y := lib/x86/rts/crtso.o 

LDFLAGS_rs.elf32 := -Llib -lsys -lnucc \
	                  -Tservers/server_32.lds

$(src)/rs.elf32: servers/server_32.lds 

# Create in minix3's aout format
rs_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
rs_a_hdrlen  := 0x20
rs_a_cpu     := i386
rs_stackheap := 0x32000

e2a-y := rs.elf32,rs
