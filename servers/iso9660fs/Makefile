# Makefile for ISO9660 fs
app-y := isofs.elf32
isofs.elf32-type := server
isofs.elf32-obj-y += main.o table.o mount.o super.o inode.o device.o \
		     utility.o misc.o path.o read.o stadir.o cache.o \
		     protect.o

NR_BUFS = 100

ccflags-y := -DNR_BUFS=$(NR_BUFS)
isofs.elf32-obj-y += generic/built-in.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_isofs.elf32 := -L$(lib-arch) -Llib \
		       -lsys ltimers -lnucc \
		       -Tservers/server_32.lds

$(src)/isofs.elf32: servers/server_32.lds

# Create in minix3's aout format
isofs_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
isofs_a_hdrlen  := 0x20
isofs_a_cpu     := i386
isofs_stackheap := 64k

e2a-y := isofs.elf32,isofs
