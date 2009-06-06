# Makefile for File System (FS)
app-y := vfs.elf32
vfs.elf32-obj-y := main.o open.o read.o write.o pipe.o dmap.o \
	                 path.o device.o mount.o link.o exec.o \
	                 filedes.o stadir.o protect.o time.o \
	                 lock.o misc.o utility.o select.o timers.o table.o \
	                 vnode.o vmnt.o request.o mmap.o fscall.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

vfs.elf32-extobj-y := $(lib-arch)/rts/crtso.o

LDFLAGS_vfs.elf32 := -L$(lib-arch) -Llib \
		     -lsys -ltimers -lnucc \
		     -L$(cc-lib-dir) -lgcc \
		     -Tservers/server_32.lds

$(src)/vfs.elf32: servers/server_32.lds

# Create in minix3's aout format
vfs_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
vfs_a_hdrlen  := 0x20
vfs_a_cpu     := i386
vfs_stackheap := 0x4000

e2a-y := vfs.elf32,vfs
