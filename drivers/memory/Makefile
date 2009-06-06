# Makefile for memory driver (MEMORY)
app-y := memory.elf32

memory.elf32-obj-y := allocmem.o memory.o

# Compile the initial ramdisk into memory driver
memory.elf32-obj-$(CONFIG_BUILTIN_INITRD) += initrd.o

memory.elf32-extobj-y := $(lib-arch)/rts/crtso.o

LDFLAGS_memory.elf32 := -L$(lib-arch) -Llib \
			-ldriver -lsys -lnucc \
			-L$(cc-lib-dir) -lgcc \
			-Tdrivers/driver_32.lds

$(src)/memory.elf32: drivers/driver_32.lds

# Create in minix3's aout format
memory_a_flags   := $(CONFIG_DRIVERS_LINKAGE_TYPE)
memory_a_hdrlen  := 0x20
memory_a_cpu     := i386
memory_stackheap := 0x2000

e2a-y := memory.elf32,memory

ifeq ($(CONFIG_BUILTIN_INITRD),y)

# Handle builtin ramdisk
# ---------------------------------------------------------------------------

__initrd_path := $(patsubst "%",%,$(CONFIG_INITRD_FSIMG_SOURCE))
initrd_path := $(wildcard $(abspath $(strip \
	               $(if $(patsubst /%,%,$(__initrd_path)),$(__initrd_path),$(srctree)/$(__initrd_path)))))

quiet_cmd_gen_initrd_h = GEN     $@
      cmd_gen_initrd_h = scripts/tools/bintoc -o $@ $<

targets += initrd.h

# generate this as first
first := initrd.h

$(src)/initrd.h: scripts/tools/bintoc
$(src)/initrd.h: $(initrd_path)
	$(call if_changed,gen_initrd_h)
endif

# just delete if exists
clean-files := initrd.h
