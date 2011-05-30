# Makefile for Virtual File System (VFS)
appname := vfs
# for now same as kernel (may be changed in future)
appversion := $(KERNELVERSION)
apptype := server

app-y := $(appname).elf32
$(appname).elf32-type := $(apptype)
$(appname).elf32-obj-y := main.o open.o read.o write.o pipe.o dmap.o path.o device.o \
			  mount.o link.o exec.o filedes.o stadir.o protect.o time.o \
			  lock.o misc.o utility.o select.o timers.o table.o vnode.o \
			  vmnt.o request.o mmap.o fscall.o vfs-syms.o

$(appname).elf32-obj-$(CONFIG_VFS_ELF32_BINFMT) += binfmt_elf32.o
$(appname).elf32-obj-$(CONFIG_VFS_AOUT_BINFMT) += binfmt_aout.o

ccflags-y := -D__APPNAME__=$(appname) -D__APPVERSION__=$(appversion) -D__APPTYPE__=$(apptype)
ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_$(appname).elf32 := -L$(lib-arch) -Llib \
			    -lsys -ltimers -lnucc \
			    -Tservers/server_32.lds

$(src)/$(appname).elf32: servers/server_32.lds

ifdef CONFIG_VFS_AOUT_BINFMT
# Create in minix3's aout format
$(appname)_a_flags   := $(CONFIG_UKERNEL_LINKAGE_TYPE)
$(appname)_a_hdrlen  := 0x20
$(appname)_a_cpu     := i386
$(appname)_stackheap := 16k

e2a-y := $(appname).elf32,$(appname)
endif

clean-files := $(appname)
