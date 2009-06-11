# Makefile for VM server
app-y := vm.elf32
vm.elf32-type := server
obj-y := $(SRCARCH)/
vm.elf32-obj-y := alloc.o break.o exec.o exit.o fork.o main.o \
	                mmap.o signal.o slaballoc.o region.o pagefaults.o \
	                utility.o vfs.o

vm.elf32-extobj-y += $(src)/$(SRCARCH)/built-in.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_vm.elf32 := -L$(lib-arch) -Llib \
		    -lsys -ltimers -lnucc \
		    -Tservers/server_32.lds

$(src)/vm.elf32: servers/server_32.lds

# Create in minix3's aout format
vm_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
vm_a_hdrlen  := 0x20
vm_a_cpu     := i386
vm_stackheap := 128k

e2a-y        := vm.elf32,vm
