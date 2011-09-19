# Makefile for terminal driver (TTY)
app-y := tty.elf32
tty.elf32-type := driver
tty.elf32-obj-y := tty.o console.o keyboard.o pty.o rs232.o

LDFLAGS_tty.elf32 := -L$(lib-arch) -Llib \
		     -lsys -ltimers -lnucc \
		     -Tdrivers/driver_32.lds

$(src)/tty.elf32: drivers/driver_32.lds

# Create in minix3's aout format
tty_a_flags   := 0x00
tty_a_hdrlen  := 0x20
tty_a_cpu     := i386
tty_stackheap := 16k

e2a-y := tty.elf32,tty
