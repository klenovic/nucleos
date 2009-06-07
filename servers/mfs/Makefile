# Makefile for File System (FS)
app-y := mfs.elf32
mfs.elf32-type := server
mfs.elf32-obj-y := cache.o device.o link.o mount.o misc.o \
	                 open.o pipe.o protect.o read.o stadir.o \
	                 table.o time.o utility.o write.o inode.o \
	                 main.o path.o super.o

NR_BUFS=1024
BS=4096

ccflags-y := -DNR_BUFS=$(NR_BUFS)
ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_mfs.elf32 := -L$(lib-arch) -Llib \
		     -lsys -ltimers -lnucc \
		     -L$(cc-lib-dir) -lgcc \
		     -Tservers/server_32.lds

$(src)/mfs.elf32: servers/server_32.lds

# Create in minix3's aout format
mfs_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
mfs_a_hdrlen  := 0x20
mfs_a_cpu     := i386
mfs_stackheap := `expr $(NR_BUFS) \* $(BS) \* 3`

e2a-y        := mfs.elf32,mfs
