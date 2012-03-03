# Makefile for inet.
app-y := inet.elf32
inet.elf32-type := server
inet.elf32-obj-y := generic/
inet.elf32-obj-y += buf.o clock.o inet.o inet_config.o \
		    mnx_eth.o mq.o qp.o queryparam.o sr.o sha2.o

inet.elf32-obj-y += generic/built-in.o

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_inet.elf32 := -L$(lib-arch) -Llib \
		      -lsys -lnucc \
		      -Tservers/server_32.lds

$(src)/inet.elf32: servers/server_32.lds

# Create in minix3's aout format
inet_a_flags   := 0x00
inet_a_hdrlen  := 0x20
inet_a_cpu     := i386
inet_stackheap := 32k

e2a-y := inet.elf32,inet
