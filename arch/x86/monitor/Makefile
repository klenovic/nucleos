# Makefile for the boot monitor package.
### boot aka boot monitor
app-y      += boot.elf32
boot.elf32-obj-y := boot.o boothead.o bootimage.o bootutils.o rawfs.o \
	            errno.o malloc.o printf.o

BOOT_ORG    = 0x0000
BOOT_ENTRY  = bootcode_entry
HDRLEN      = 0x20

# %.c files
ccflags-y := -D__KERNEL__ -include arch/$(SRCARCH)/include/asm/code16gcc.h

# %.S files
asflags-y := -include arch/$(SRCARCH)/include/asm/code16gcc.h

ASFLAGS_boothead.o += -D__HDRLEN__=$(HDRLEN)
CFLAGS_boot.o      += -I$(src)/../../../
CFLAGS_bootimage.o += -I$(src)/../../../
CFLAGS_rawfs.o     += -I$(src)/../../../

CPPFLAGS_boot.lds := -D__ORIGIN__=$(BOOT_ORG)
CPPFLAGS_boot.lds += -C -P

targets += boot.lds
$(src)/boot.elf32: $(src)/boot.lds

LDFLAGS_boot.elf32 += -N -S -e $(BOOT_ENTRY) \
		      -L$(lib-arch) \
		      -lnucc_16 \
		      -Tarch/x86/monitor/boot.lds

# Create in minix3's aout format
boot_a_flags   := $(CONFIG_BOOTMONITOR_LINKAGE_TYPE)
boot_a_hdrlen  := $(HDRLEN)
boot_a_cpu     := i8086
boot_stackheap := 8k

e2a-y += boot.elf32,boot

### bootblock
app-y += bootblock.elf32
bootblock.elf32-obj-y := bootblock.o
ASFLAGS_bootblock.o += -D__NOAUTO_OP16_PREFIX__ -D__HDRLEN__=$(HDRLEN)

BOOTBLOCK_ORG   = 0x0000
BOOTBLOCK_ENTRY = bootblock_entry

CPPFLAGS_bootblock.lds := -D__ORIGIN__=$(BOOTBLOCK_ORG)
CPPFLAGS_bootblock.lds += -C -P

targets += bootblock.lds
$(src)/bootblock.elf32: $(src)/bootblock.lds

LDFLAGS_bootblock.elf32 += -N -S -e $(BOOTBLOCK_ENTRY) \
	                   -Tarch/x86/monitor/bootblock.lds

targets += bootblock

OBJCOPYFLAGS_bootblock := -S -O binary

# create a raw binary from ELF32 boot
$(src)/bootblock: $(src)/bootblock.elf32
	$(call if_changed,objcopy)

### masterboot
app-y += masterboot.elf32
masterboot.elf32-obj-y := masterboot.o
ASFLAGS_masterboot.o += -D__NOAUTO_OP16_PREFIX__

MASTERBOOT_ORG   = 0
MASTERBOOT_ENTRY = start

CPPFLAGS_masterboot.lds := -D__ORIGIN__=$(MASTERBOOT_ORG)
CPPFLAGS_masterboot.lds += -C -P

targets += masterboot.lds
$(src)/masterboot.elf32: $(src)/masterboot.lds

LDFLAGS_masterboot.elf32 += -N -S -e $(MASTERBOOT_ENTRY) \
	                    -Tarch/x86/monitor/masterboot.lds

# Create in minix3's aout format
masterboot_a_flags   := 0x00
masterboot_a_hdrlen  := $(HDRLEN)
masterboot_a_cpu     := i8086
masterboot_stackheap := 0

e2a-y += masterboot.elf32,masterboot
