# Makefile for ISO9660 fs
app-y := iso9660fs.elf32
iso9660fs.elf32-type := server
iso9660fs.elf32-obj-y += main.o table.o mount.o super.o inode.o device.o \
			 utility.o misc.o path.o read.o stadir.o cache.o \
			 isofs-syms.o

NR_BUFS = 100

ccflags-y := -DNR_BUFS=$(NR_BUFS)

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_iso9660fs.elf32 := -L$(lib-arch) -Llib \
		       -lsys -ltimers -lnucc \
		       -Tservers/server_32.lds

$(src)/iso9660fs.elf32: servers/server_32.lds

# Create in minix3's aout format
iso9660fs_a_flags   := 0x00
iso9660fs_a_hdrlen  := 0x20
iso9660fs_a_cpu     := i386
iso9660fs_stackheap := 64k

e2a-y := iso9660fs.elf32,iso9660fs
