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
endef

# create kernel image from kernel and servers binaries
$(src)/.tmp.image: $(obj)/tools/build scripts/tools/mkimage
$(src)/.tmp.image: $(bin-y) FORCE
	$(call if_changed_rule,create_image)

quiet_cmd_create_kimage = GEN     $@
      cmd_create_kimage = $(obj)/tools/build $(obj)/.tmp.boot.rawbin $(obj)/.tmp.image > $@

define rule_create_kimage
	rm -f $@; \
	$(echo-cmd) $(cmd_$(1)); \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
	@echo 'Kernel: $@ is ready' ' (#'`cat .version`')'
endef

# create a final image
$(src)/kimage: $(obj)/.tmp.boot.rawbin
$(src)/kimage: $(obj)/tools/build $(obj)/.tmp.image FORCE
	$(call if_changed_rule,create_kimage)

targets += kimage

app-y      += boot.elf32
boot.elf32-obj-y := boot.o header.o bootimage.o bootutils.o printf.o

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
