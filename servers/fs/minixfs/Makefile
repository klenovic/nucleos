# Makefile for Minix File System
app-y := minixfs.elf32
minixfs.elf32-type := server
minixfs.elf32-obj-y := cache.o device.o link.o mount.o misc.o \
		       open.o protect.o read.o stadir.o \
		       table.o time.o utility.o write.o inode.o \
		       main.o path.o super.o minixfs-syms.o

NR_BUFS=1024
BS=4096

ccflags-y := -DNR_BUFS=$(NR_BUFS)
ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_minixfs.elf32 := -L$(lib-arch) -Llib \
			 -lsys -lnucc \
			 -Tservers/server_32.lds

$(src)/minixfs.elf32: servers/server_32.lds

# Create in minix3's aout format
minixfs_a_flags := $(CONFIG_UKERNEL_LINKAGE_TYPE)
minixfs_a_hdrlen := 0x20
minixfs_a_cpu := i386
minixfs_stackheap := 32kb

e2a-y := minixfs.elf32,minixfs
