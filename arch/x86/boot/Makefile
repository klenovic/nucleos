# Create image from executables (kernel & servers)
hostprogs-y := tools/build

# NOTE: KEEP THE ORDER AS IN kernel/kernel-syms.c!
bin-y := arch/$(SRCARCH)/kernel/kernel
bin-y += servers/pm/pm
bin-y += servers/fs/vfs/vfs
bin-y += servers/rs/rs
bin-y += drivers/memory/memory
bin-y += drivers/log/log
bin-y += drivers/tty/tty
bin-y += servers/ds/ds
bin-y += servers/fs/minixfs/minixfs
bin-y += arch/$(SRCARCH)/servers/vm/vm
bin-y += servers/fs/pipefs/pipefs
bin-y += servers/fs/ext2/ext2
bin-y += servers/init/init

quiet_cmd_create_image = GEN     $@
      cmd_create_image = scripts/tools/mkimage $@ $(filter $(bin-y),$^)

define rule_create_image
	rm -f $@; \
	$(echo-cmd) $(cmd_$(1)); \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
	@echo 'Kernel: $@ is ready' ' (#'`cat .version`')'
endef

# create kernel image from kernel and servers binaries
$(src)/image: $(obj)/tools/build scripts/tools/mkimage
$(src)/image: $(bin-y) FORCE
	$(call if_changed_rule,create_image)

targets += image

quiet_cmd_create_kimage = GEN     $@
      cmd_create_kimage = $(obj)/tools/build $(obj)/.tmp.boot.rawbin $(obj)/image > $@

define rule_create_kimage
	rm -f $@; \
	$(echo-cmd) $(cmd_$(1)); \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
	@echo 'Kernel: $@ is ready' ' (#'`cat .version`')'
endef

# create a final image
$(src)/kimage: $(obj)/.tmp.boot.rawbin
$(src)/kimage: $(obj)/tools/build $(obj)/image FORCE
	$(call if_changed_rule,create_kimage)

targets += kimage

app-y      += boot.elf32
boot.elf32-obj-y := boot.o boothead.o bootimage.o bootutils.o rawfs.o \
	            errno.o printf.o

BOOT_ORG    = 0x0000
BOOT_ENTRY  = _start
HDRLEN      = 0x20
MAX_BOOT_SIZE = 0x3f
BOOT_OFF = 0x0001

# %.c files
ccflags-y := -D__KERNEL__ -include arch/$(SRCARCH)/include/asm/code16gcc.h

# %.S files
asflags-y := -include arch/$(SRCARCH)/include/asm/code16gcc.h

ASFLAGS_boothead.o += -D__HDRLEN__=$(HDRLEN)

CPPFLAGS_boot.lds := -D__ORIGIN__=$(BOOT_ORG)
CPPFLAGS_boot.lds += -C -P

targets += boot.lds
$(src)/boot.elf32: $(src)/boot.lds

LDFLAGS_boot.elf32 += -N -S -e $(BOOT_ENTRY) \
		      -L$(lib-arch) \
		      -lnucc_16 \
		      -T$(src)/boot.lds

# Create in minix3's aout format
boot_a_flags   := 0x00
boot_a_hdrlen  := $(HDRLEN)
boot_a_cpu     := i8086
boot_stackheap := 8k

e2a-y += boot.elf32,boot

### bootblock
app-y += bootblock.elf32
bootblock.elf32-obj-y := bootblock.o
ASFLAGS_bootblock.o += -D__NOAUTO_OP16_PREFIX__ \
		       -D__HDRLEN__=$(HDRLEN) \
		       -D__MAX_BOOT_SIZE__=$(MAX_BOOT_SIZE) \
		       -D__BOOT_OFF__=$(BOOT_OFF)

BOOTBLOCK_ORG   = 0x0000
BOOTBLOCK_ENTRY = bootblock_entry

CPPFLAGS_bootblock.lds := -D__ORIGIN__=$(BOOTBLOCK_ORG)
CPPFLAGS_bootblock.lds += -C -P

targets += bootblock.lds
$(src)/bootblock.elf32: $(src)/bootblock.lds

LDFLAGS_bootblock.elf32 += -N -S -e $(BOOTBLOCK_ENTRY) \
	                   -T$(src)/bootblock.lds

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
	                    -T$(src)/masterboot.lds

# Create in minix3's aout format
masterboot_a_flags   := 0x00
masterboot_a_hdrlen  := $(HDRLEN)
masterboot_a_cpu     := i8086
masterboot_stackheap := 0

e2a-y += masterboot.elf32,masterboot
