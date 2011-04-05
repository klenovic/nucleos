obj-y := simple/

CFLAGS_initrd.o := -D__UKERNEL__

# Handle initial ramdisk
# ---------------------------------------------------------------------------

# These below must exist before we start to create initrd
bin-y := $(srctree)/drivers/at_wini \
	 $(srctree)/drivers/bios_wini \
	 $(srctree)/drivers/floppy \
	 $(srctree)/drivers/pci \
	 $(srctree)/servers/fs/minixfs/minixfs \
	 $(srctree)/servers/fs/ext2/ext2 \
	 $(src)/sh \
	 $(src)/simple/cdprobe \
	 $(src)/simple/dev2name \
	 $(src)/simple/loadramdisk \
	 $(src)/simple/newroot \
	 $(src)/simple/service \
	 $(src)/simple/sysenv

scripts := modules.conf mtab passwd proto.gen rc

targets += initrd.minixfs initrd

quiet_cmd_gen_initrd.minixfs = GEN     $@
      cmd_gen_initrd.minixfs = $(srctree)/scripts/tools/mkfs $@ -p $(src)/proto.gen

$(src)/initrd.minixfs: $(srctree)/scripts/tools/mkfs
$(src)/initrd.minixfs: $(addprefix $(src)/,$(scripts)) $(bin-y) FORCE
	$(call if_changed,gen_initrd.minixfs)

quiet_cmd_gen_initrd = GEN     $@
      cmd_gen_initrd = $(srctree)/scripts/tools/mkinitrd -i $< -o $@

$(src)/initrd: $(srctree)/scripts/tools/mkinitrd
$(src)/initrd: $(src)/initrd.minixfs FORCE
	$(call if_changed,gen_initrd)

targets += $(scripts) $(src)/sh

quiet_cmd_extract_scripts = EXTRACT $@
      cmd_extract_scripts = tar -C $(src) -xmjf $(srctree)/scripts/initrd-scripts.tar.bz2 $(notdir $@)

$(addprefix $(src)/,$(scripts)): $(srctree)/scripts/initrd-scripts.tar.bz2 FORCE
	$(call if_changed,extract_scripts)

$(src)/sh: $(srctree)/scripts/initrd-scripts.tar.bz2 FORCE
	$(call if_changed,extract_scripts)

ifeq ($(CONFIG_BUILTIN_INITRD),y)

# Handle builtin ramdisk
# ---------------------------------------------------------------------------
obj-y += initrd.o

__initrd_path := $(patsubst "%",%,$(CONFIG_INITRD_FSIMG_SOURCE))
initrd_path := $(strip $(if $(patsubst /%,%,$(__initrd_path)),$(__initrd_path), \
			    $(__initrd_path)))

targets += initrd.h

quiet_cmd_gen_initrd_h = GEN     $@
      cmd_gen_initrd_h = $(srctree)/scripts/tools/bintoc -o $@ -i $<

$(src)/initrd.h: $(srctree)/scripts/tools/bintoc
$(src)/initrd.h: $(initrd_path) FORCE
	$(call if_changed,gen_initrd_h)

# need to be mentioned explicitly
$(src)/initrd.o: $(src)/initrd.h

# Rebuild the memory driver
targets += .tmp.rebuild.memdrv

memory-driver := $(srctree)/drivers/memory

quiet_cmd_rebuild_memdrv = REBUILD $(patsubst $(srctree)/%,%,drivers/memory)
      cmd_rebuild_memdrv = echo 'cmd_$@ := $(MAKE) -f $(srctree)/scripts/mk/Makefile.build obj=$(memory-driver)' > $@

define rule_rebuild_memdrv
	$(echo-cmd) $(cmd_$(1)); \
	$(MAKE) -f $(srctree)/scripts/mk/Makefile.build obj=$(memory-driver); \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
endef

$(src)/.tmp.rebuild.memdrv: $(src)/initrd.o FORCE
	$(call if_changed_rule,rebuild_memdrv)
endif

clean-files := initrd initrd.minixfs initrd.h
clean-files += $(scripts) sh
