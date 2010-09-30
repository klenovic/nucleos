# Makefile for the AT disk driver (AT_WINI)
app-y := lance.elf32
lance.elf32-type := driver
lance.elf32-obj-y := lance.o

LDFLAGS_lance.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-Tdrivers/driver_32.lds

$(src)/lance.elf32: drivers/driver_32.lds

# Create in minix3's aout format
lance_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
lance_a_hdrlen  := 0x20
lance_a_cpu     := i386
lance_stackheap := 128k

e2a-y := lance.elf32,lance

ifeq (0,1)
# Makefile for Intel Pro/100 driver (FXP)
DRIVER = lance

# directories
u = /usr
i = $u/include
s = $i/sys
m = $i/minix
b = $i/ibm
d = ..

# programs, flags, etc.
CC =	exec cc
CFLAGS = -I$i $(CPROFILE)
LDFLAGS = -i 
LIBS =  -lsys 

OBJ = lance.o

# build local binary
all build:	$(DRIVER)
$(DRIVER):	$(OBJ) 
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	install -S 128k $(DRIVER)

# install with other drivers
install:	/usr/sbin/$(DRIVER)
/usr/sbin/$(DRIVER):	$(DRIVER)
	install -o root -cs $? $@

# clean up local files
clean:
	rm -f *.o *.bak $(DRIVER)

depend:
	mkdep "$(CC) -E $(CPPFLAGS)" *.c > .depend

# Include generated dependencies.
include .depend
endif
