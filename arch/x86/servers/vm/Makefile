# Makefile for x86 VM server
app-y := vm.elf32
vm.elf32-type := server
vm.elf32-obj-y := vm.o pagetable.o pagefaults.o util.o

# add its generic part
vm.elf32-extobj-y += servers/vm/built-in.o

ccflags-y := -D__UKERNEL__
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
