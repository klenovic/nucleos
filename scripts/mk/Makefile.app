##############################################################################
#  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
#
#  This file is part of Nucleos kernel.
#
#  Nucleos kernel is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 2 of the License.
##############################################################################

# build applications (mainly servers)
# target's applications
app-y := $(sort $(addprefix $(src)/,$(app-y)))

# rule to create app
define rule_link_app

ifneq ($$(filter server driver,$$($$(notdir $(1))-type)),)
# add startup code to servers and drivers
$$(notdir $(1))-extobj-y += $$(lib-arch)/rts/crtso.o

# whatever it is, it is user part of kernel
$$(foreach o,$$(filter %.o,$$($$(notdir $(1))-obj-y)), \
	$$(eval CFLAGS_$$(o) += -D__UKERNEL__))

ifeq ($$($$(notdir $(1))-type),server)
# add `__SERVER__' into objects' cflags of this server
$$(foreach o,$$(filter %.o,$$($$(notdir $(1))-obj-y)), \
	$$(eval CFLAGS_$$(o) += -D__SERVER__))
else
ifeq ($$($$(notdir $(1))-type),driver)
# add `__DRIVER__' into objects' cflags of this driver
$$(foreach o,$$(filter %.o,$$($$(notdir $(1))-obj-y)), \
	$$(eval CFLAGS_$$(o) += -D__DRIVER__))
endif # driver
endif # server

endif # type

quiet_cmd_link_app_$(1) = LD      $$@
      cmd_link_app_$(1) = $$(LD) -o $$@ $$(filter $$($(1)-obj-y),$$^) $$(extobj-y) \
				 $$($$(notdir $(1))-extobj-y) $$(ld_flags)

# This handles also the case if host. app. is not in the same directory
# as this Makefiel is.
#
# e.g. progs-y := subdir/appl
#      subdir/appl-obj-y :=  subdir/obj1.o subdir/obj2.o
$(1)-obj-y := $$(filter %.o,$$(addprefix $$(src)/,$$($$(subst $$(src)/,,$(1))-obj-y)))

# add to object list
objs-y += $$($(1)-obj-y)

# add to directory list
subdir-y += $$(filter-out %.o,$$(addprefix $$(src)/,$$($$(subst $$(src)/,,$(1))-obj-y)))

# flags for library
# Note that this is used only on prerequisite side
ld_flags_$(1) := $$(LDFLAGS) $$(ldflags-y) $$(LDFLAGS_$$(notdir $(1)))

# depends on external object but doesn't build it
$(1): $$($(1)-obj-y) $$(extobj-y) $$($$(notdir $(1))-extobj-y) \
	$$(call get_libs,$$(ld_flags_$(1))) FORCE
	$$(call if_changed,link_app_$(1))

# add into target list
targets += $(1)
endef

# generate app-y goals
$(foreach app,$(app-y),$(eval $(call rule_link_app,$(app))))

# Create aout binary from elf32
# param input elf32 file
# param output aout file
# -------------------------------------------------------------------------------------

define rule_create_aoutbin
# create a header from ELF32 boot
quiet_cmd_create_header_$(2) = GEN     $$@
      cmd_create_header_$(2) = scripts/tools/ehdr2ahdr \
					-c $$($(2)_a_cpu) \
					-f $$($(2)_a_flags) \
					-s $$($(2)_stackheap) \
					-l $$($(2)_a_hdrlen) \
					-i $$< -o $$@

$$(src)/.tmp.$(2).ahdr: scripts/tools/ehdr2ahdr
$$(src)/.tmp.$(2).ahdr: $$(src)/$(1) FORCE
	$$(call if_changed,create_header_$(2))

targets += $$(src)/.tmp.$(2).ahdr

OBJCOPYFLAGS_.tmp.$(2).rawbin := -O binary
OBJCOPYFLAGS_.tmp.$(2).rawbin += $$(if $$(strip $$($(2)_keepsyms)),--keep-file-symbols,-S)

# create a raw binary from ELF32 boot
$$(src)/.tmp.$(2).rawbin: $$(src)/$(1) FORCE
	$$(call if_changed,objcopy)

targets += $$(src)/.tmp.$(2).rawbin

quiet_cmd_create_aout_$(2) = GEN     $$@
      cmd_create_aout_$(2) = cat $$(src)/.tmp.$(2).ahdr $$(src)/.tmp.$(2).rawbin > $$@

# create aout binary by adding aout header at begin
$$(src)/$(2): $$(src)/.tmp.$(2).ahdr $$(src)/.tmp.$(2).rawbin FORCE
	$$(call if_changed,create_aout_$(2))

targets += $$(src)/$(2)
endef

# Create aout from elf. Input must be in the following 
# format and order: 
#   e2a-y := ELF32,AOUT 
# Note: Binaries are separated by comma.
# -------------------------------------------------------------------------------------

ifneq ($(strip $(e2a-y)),)
# grep the src. and dst. names
elfsrc = $(firstword $(subst $(comma), ,$(e2a)))
aoutdst = $(lastword $(subst $(comma), ,$(e2a)))

$(foreach e2a,$(e2a-y),$(eval $(call rule_create_aoutbin,$(elfsrc),$(aoutdst))))
endif
