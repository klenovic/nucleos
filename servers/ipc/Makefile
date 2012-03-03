# Makefile for IPC server
app-y := ipc.elf32
ipc.elf32-type := server
ipc.elf32-obj-y := main.o utility.o shm.o sem.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_ipc.elf32 := -L$(lib-arch) -Llib \
		     -lsys -lnucc \
		     -Tservers/server_32.lds

$(src)/ipc.elf32: servers/server_32.lds

# Create in minix3's aout format
ipc_a_flags   := 0x00
ipc_a_hdrlen  := 0x20
ipc_a_cpu     := i386
ipc_stackheap := 32k

e2a-y        := ipc.elf32,ipc
