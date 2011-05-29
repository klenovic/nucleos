# Makefile for pipe file system
app-y := pipefs.elf32
pipefs.elf32-type := server
pipefs.elf32-obj-y := open.o table.o inode.o main.o super.o link.o \
		      buffer.o read.o misc.o utility.o stadir.o \
		      pipefs-syms.o

NR_BUFS=256
BS=4096

ccflags-y := -DNR_BUFS=$(NR_BUFS)
ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_pipefs.elf32 := -L$(lib-arch) -Llib \
			-lsys -lnucc \
			-Tservers/server_32.lds

$(src)/pipefs.elf32: servers/server_32.lds

# Create in minix3's aout format
pipefs_a_flags := $(CONFIG_UKERNEL_LINKAGE_TYPE)
pipefs_a_hdrlen := 0x20
pipefs_a_cpu := i386
pipefs_stackheap := 32kb

e2a-y := pipefs.elf32,pipefs
