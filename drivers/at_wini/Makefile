# Makefile for the AT disk driver (AT_WINI)
app-y := at_wini.elf32
at_wini.elf32-obj-y := at_wini.o 

at_wini.elf32-extobj-y := $(lib-arch)/rts/crtso.o

# arch. library path has precedence over generic
LDFLAGS_at_wini.elf32 := -L$(lib-arch) -Llib/ \
			 -lsys -ldriver -lnucc \
			 -L$(cc-lib-dir) -lgcc \
			 -Tdrivers/driver_32.lds

$(src)/at_wini.elf32: drivers/driver_32.lds

# Create in minix3's aout format
at_wini_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
at_wini_a_hdrlen  := 0x20
at_wini_a_cpu     := i386
at_wini_stackheap := 0x4000

e2a-y := at_wini.elf32,at_wini
