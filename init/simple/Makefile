# Makefile for commands/simple.
app-y := cdprobe.elf32 
app-y += dev2name.elf32 
app-y += loadramdisk.elf32 
app-y += newroot.elf32 
app-y += service.elf32 
app-y += sysenv.elf32 

# @klenovic: This is just a fake to compile these utilities.
#            They are _not_ the user part of kernel!
ccflags-y := -D__UKERNEL__

cdprobe.elf32-obj-y     := cdprobe.o
dev2name.elf32-obj-y    := dev2name.o
loadramdisk.elf32-obj-y := loadramdisk.o
newroot.elf32-obj-y     := newroot.o
service.elf32-obj-y     := service.o
sysenv.elf32-obj-y      := sysenv.o

extobj-y := $(lib-arch)/rts/crtso.o

LDFLAGS += -Tservers/server_32.lds

LDFLAGS_cdprobe.elf32 := -L$(lib-arch) -Llib -lnucc
LDFLAGS_dev2name.elf32 := -L$(lib-arch) -Llib -lnucc
LDFLAGS_loadramdisk.elf32 := -L$(lib-arch) -Llib -lnucc
LDFLAGS_newroot.elf32 := -L$(lib-arch) -Llib -lnucc
LDFLAGS_sysenv.elf32 := -L$(lib-arch) -Llib -lnucc
LDFLAGS_service.elf32 := -L$(lib-arch) -Llib -lnucc

$(src)/cdprobe.elf32: servers/server_32.lds
$(src)/dev2name.elf32: servers/server_32.lds
$(src)/loadramdisk.elf32: servers/server_32.lds
$(src)/newroot.elf32: servers/server_32.lds
$(src)/service.elf32: servers/server_32.lds
$(src)/sysenv.elf32: servers/server_32.lds

# Create in minix3's aout format
cdprobe_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
cdprobe_a_hdrlen  := 0x20
cdprobe_a_cpu     := i386
cdprobe_stackheap := 112k

e2a-y := cdprobe.elf32,cdprobe

dev2name_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
dev2name_a_hdrlen  := 0x20
dev2name_a_cpu     := i386
dev2name_stackheap := 16k

e2a-y += dev2name.elf32,dev2name

loadramdisk_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
loadramdisk_a_hdrlen  := 0x20
loadramdisk_a_cpu     := i386
loadramdisk_stackheap := 16k

e2a-y += loadramdisk.elf32,loadramdisk

newroot_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
newroot_a_hdrlen  := 0x20
newroot_a_cpu     := i386
newroot_stackheap := 128k

e2a-y += newroot.elf32,newroot

service_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
service_a_hdrlen  := 0x20
service_a_cpu     := i386
service_stackheap := 128k

e2a-y += service.elf32,service

sysenv_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
sysenv_a_hdrlen  := 0x20
sysenv_a_cpu     := i386
sysenv_stackheap := 16k

e2a-y += sysenv.elf32,sysenv
