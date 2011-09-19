# Makefile for Data Store Server (DS)
app-y := is.elf32
is.elf32-type := server
is.elf32-obj-y := main.o dmp.o dmp_kernel.o dmp_pm.o dmp_fs_minix.o dmp_rs.o dmp_ds.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_is.elf32 := -L$(lib-arch) -Llib \
		    -lsys -lnucc \
		    -Tservers/server_32.lds

$(src)/is.elf32: servers/server_32.lds

# Create in minix3's aout format
is_a_flags   := 0x00
is_a_hdrlen  := 0x20
is_a_cpu     := i386
is_stackheap := 32k

e2a-y := is.elf32,is
