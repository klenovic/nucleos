# Makefile for ext2 file system
app-y := ext2.elf32
ext2.elf32-type := server
ext2.elf32-obj-y := balloc.o cache.o device.o link.o \
		    mount.o misc.o open.o protect.o read.o \
		    stadir.o table.o time.o utility.o \
		    write.o ialloc.o inode.o main.o path.o \
		    super.o optset.o ext2-syms.o

NR_BUFS=1024

ccflags-y := -DNR_BUFS=$(NR_BUFS)

LDFLAGS_ext2.elf32 := -L$(lib-arch) -Llib \
		      -lsys -lnucc \
		      -Tservers/server_32.lds

$(src)/ext2.elf32: servers/server_32.lds

# Create in minix3's aout format
ext2_a_flags := $(CONFIG_SERVERS_LINKAGE_TYPE)
ext2_a_hdrlen := 0x20
ext2_a_cpu := i386
ext2_stackheap := 128kb

e2a-y := ext2.elf32,ext2
