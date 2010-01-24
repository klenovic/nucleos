# Create image from executables (kernel & servers)
# Note: Keep a watch of the correct order!
bin-y := arch/$(SRCARCH)/kernel/kernel
bin-y += servers/pm/pm
bin-y += servers/fs/vfs/vfs
bin-y += servers/rs/rs
bin-y += drivers/memory/memory
bin-y += drivers/log/log
bin-y += drivers/tty/tty
bin-y += servers/ds/ds
bin-y += servers/fs/mfs/mfs
bin-y += arch/$(SRCARCH)/servers/vm/vm
bin-y += servers/init/init

quiet_cmd_create_image = GEN     $@
      cmd_create_image = scripts/tools/mkimage $@ $(filter $(bin-y),$^)

define rule_create_image
	rm -f $@; \
	$(echo-cmd) $(cmd_$(1)); \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
endef

# create final image from kernel and servers
$(src)/image: scripts/tools/installboot scripts/tools/mkimage
$(src)/image: $(bin-y) FORCE
	$(call if_changed_rule,create_image)

targets += image
