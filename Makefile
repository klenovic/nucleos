##############################################################################
#  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
#
#  This file is part of Nucleos kernel.
#
#  Nucleos kernel is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 2 of the License.
##############################################################################

# Master Makefile
# increments in case of significant changes like architecture 
# (e.g. new architecture was added ...)
VERSION = 1
# grows in case of new functionalities (current VERSION )
PATCHLEVEL = 1
# grows if in case of bug fixes (current VERSION and PATCHLEVEL)
SUBLEVEL = 0
# extraversion string e.g. alpha, beta, -rc1, ...
EXTRAVERSION = -alpha1
# name/motto/quote for release
NAME = Get the ball rolling!

# default goal
PHONY := __all
__all:

# put scripts' path into searchpath and don't print "Entering directory"
# be or not to be verbose (V=1 be)
ifdef V
  ifeq ("$(origin V)","command line")
    KBUILD_VERBOSE = $(V)
  endif
endif

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

# Build external stuff
ifdef M
  ifeq ("$(origin M)", "command line")
    KBUILD_EXTMOD := $(M)
  endif
endif

export KBUILD_EXTMOD

# prepare build environment
srctree	:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
objtree	:= $(CURDIR)
src	:= $(srctree)
obj	:= $(objtree)
VPATH	:= $(srctree)$(if $(KBUILD_EXTMOD),:$(KBUILD_EXTMOD))

MAKE ?= make

# no built-in implicit rules variables
# Do not print "Entering directory..."
MAKEFLAGS += -rR --no-print-directory

AS      = $(CROSS_COMPILE)as
AR      = $(CROSS_COMPILE)ar
LD      = $(CROSS_COMPILE)ld
CXX     = $(CROSS_COMPILE)g++
CC      = $(CROSS_COMPILE)gcc
CPP     = $(CC) -E
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB  = $(CROSS_COMPILE)ranlib
READELF = $(CROSS_COMPILE)readelf
OBJCOPY = $(CROSS_COMPILE)objcopy

# SUBARCH tells the usermode build what the underlying arch is.  That is set
# first, and if a usermode build is happening, the "ARCH=um" on the command
# line overrides the setting of ARCH below.  If a native build is happening,
# then ARCH is assigned, getting whatever value it gets normally, and
# SUBARCH is subsequently ignored.
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
				  -e s/sh.*/sh/ )

# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# When performing cross compilation for other architectures ARCH shall be set
# to the target architecture. (See arch/* for the possibilities).
# ARCH can be set during invocation of make:
# make ARCH=ia64
# Another way is to have ARCH set in the environment.
# The default ARCH is the host where make is executed.

# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
# Default value for CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CROSS_COMPILE in their arch/*/Makefile
export KBUILD_BUILDHOST := $(SUBARCH)
ARCH            ?= $(SUBARCH)
CROSS_COMPILE   ?=

# Architecture as present in compile.h
UTS_MACHINE := $(ARCH)
SRCARCH := $(ARCH)

# Additional ARCH settings for x86
ifeq ($(ARCH),i386)
  SRCARCH := x86
endif

# Where to locate arch specific headers
hdr-arch  := $(SRCARCH)

# Read KERNELRELEASE from include/config/kernel.release (if it exists)
KERNELRELEASE = $(shell cat include/config/kernel.release 2> /dev/null)
KERNELVERSION = $(strip $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION))

export KERNELRELEASE KERNELVERSION

KBUILD_CPPFLAGS := -D__KERNEL__ 
KBUILD_CPPFLAGS += -D__ARCH__=$(SRCARCH) -D__KERNELVERSION__=$(KERNELVERSION) \
	           -D__SRCROOT__=$(srctree)

# general build options for asm
KBUILD_AFLAGS :=

# general build options for c
KBUILD_CFLAGS := -Wall -Wstrict-prototypes -fno-builtin -nostdinc -fno-exceptions \
	         -ffreestanding

# general build options for c++
KBUILD_CXXFLAGS := -fno-builtin -fno-rtti -nostdinc++ \
	           -fno-exceptions -Wall 

# common linker flags
LDFLAGS := -nostdlib
KBUILD_ARFLAGS := rcs

# includes only common headers files for all architectures,
#  specific architecture headers are done by macros
NUCLEOSINCLUDE := -Iinclude \
	          -Iarch/$(SRCARCH)/include \
	          -imacros include/nucleos/macros.h \
	          -include include/nucleos/autoconf.h

# shell used by build system
CONFIG_SHELL := $(shell if [ -x "$BASH" ]; then echo $BASH; \
		  else if [ -x /bin/bash ]; then echo /bin/bash; \
		  else echo sh; fi ; fi)

HOSTAR      = ar
HOSTCC      = gcc
HOSTAS      = $(HOSTCC)
HOSTCXX     = g++
HOSTCPP     = $(HOSTCC) -E
HOSTLD      = gcc
HOSTNM      = nm
HOSTOBJCOPY = objcopy
HOSTOBJDUMP = objdump
HOSTRANLIB  = ranlib

export HOSTAR HOSTAR HOSTCC HOSTAS HOSTCXX HOSTCPP HOSTLD \
	HOSTNM HOSTOBJCOPY HOSTOBJDUMP HOSTRANLIB
export UTS_MACHINE

HOST_CFLAGS := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
HOST_PPFLAGS := -D__HOST__ -D__SRCROOT__=$(srctree)
HOST_PPFLAGS += -imacros include/nucleos/macros.h

export HOST_CFLAGS HOST_PPFLAGS

# variables to export
export AR ARCH AS AS16 AS64 KBUILD_AFLAGS CC CC16 CC64 CONFIG_SHELL CPP \
	CXX CXX16 CXX64 CXXFLAGS EXTRA_CFLAGS INSTALL_PATH LD LDFLAGS \
	MAKE NUCLEOSINCLUDE OBJCOPY  RANLIB READELF SRCARCH \
	srctree

export KBUILD_ARFLAGS KBUILD_CFLAGS KBUILD_CPPFLAGS

# Files to ignore in find ... statements

RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS -o -name .pc -o -name .hg -o -name .git \) -prune -o

### Configuration
ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet = quiet_
  Q = @
endif

ifneq ($(findstring s,$(MAKEFLAGS)),)
  quiet = silent_
endif

export quiet Q KBUILD_VERBOSE

# Makefiles relative to root
MAKEFLAGS += --include-dir=$(srctree)

# Include some helper functions
$(srctree)/scripts/mk/Kbuild.include: ;
include $(srctree)/scripts/mk/Kbuild.include

### Preparation for build (common for all architecture)
KCONFIG_CONFIG ?= .config

ifeq ($(KBUILD_EXTMOD),)
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=scripts/basic

# To avoid any implicit rule to kick in, define an empty command.
scripts/basic/%: scripts_basic ;

scripts_tools:
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=scripts/tools
endif

# goals that don't need .config
no-dot-config-targets := clean distclean help mrproper tools \
			 include/nucleos/version.h

# possible %config goals
config-goals := config menuconfig xconfig gconfig \
	        randconfig allyesconfig allnoconfig \
	        silentoldconfig

config-targets := 0
mixed-targets  := 0
dot-config     := 1

ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
		dot-config := 0
	endif
endif

ifeq ($(KBUILD_EXTMOD),)
    ifneq ($(filter $(config-goals),$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(filter-out $(config-goals),$(MAKECMDGOALS)),)
            mixed-targets := 1
        endif
    endif
endif

ifeq ($(mixed-targets),1)
# ===========================================================================
# We're called with mixed targets (*config and build targets).
# Handle them one by one.
$(MAKECMDGOALS): FORCE
	$(Q)$(MAKE) -f $(srctree)/Makefile $@

else
ifeq ($(config-targets),1)
# include architecture goals, vars,...
include $(srctree)/arch/$(SRCARCH)/Makefile

config: scripts_basic FORCE
	$(Q)mkdir -p include/nucleos include/config
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=scripts/kconfig $@

%config: scripts_basic FORCE
	$(Q)mkdir -p include/nucleos include/config
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=scripts/kconfig $@

else # $(config-targets) == 1
# ===========================================================================
# Build targets only. In general all targets except *config targets.

ifeq ($(KBUILD_EXTMOD),)
# directories we have to visit
drivers-y := drivers/
servers-y := servers/
core-y := kernel/
libs-y := lib/
endif

# we need .config
ifeq ($(dot-config),1)

# Read in config
-include include/config/auto.conf

ifeq ($(KBUILD_EXTMOD),)
# Read in dependencies to all Kconfig* files, make sure to run
#  oldconfig if changes are detected.
-include include/config/auto.conf.cmd

# To avoid any implicit rule to kick in, define an empty command
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

# If .config is newer than include/config/auto.conf, someone tinkered
# with it and forgot to run make oldconfig.
# if auto.conf.cmd is missing then we are probably in a cleaned tree so
# we execute the config step to be sure to catch updated Kconfig files
include/config/auto.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig
else
# external modules needs include/nucleos/autoconf.h and include/config/auto.conf
# but do not care if they are up-to-date. Use auto.conf to trigger the test
PHONY += include/config/auto.conf

include/config/auto.conf:
	$(Q)test -e include/nucleos/autoconf.h -a -e $@ || (              \
	echo;                                                           \
	echo "  ERROR: Kernel configuration is invalid.";               \
	echo "         include/nucleos/autoconf.h or $@ are missing.";    \
	echo "         Run 'make oldconfig && make prepare' on kernel src to fix it.";  \
	echo;                                                           \
	/bin/false)
endif # KBUILD_EXTMOD

else # $(dot-config) == 0
# Dummy target needed, because used as prerequisite
include/config/auto.conf: ;
endif # $(dot-config)

ifdef CONFIG_OPTIMIZE_FOR_SIZE
KBUILD_CFLAGS += -Os
# for .cc files
KBUILD_CXXFLAGS += -Os
else
KBUILD_CFLAGS += -O2
# for .cc files
KBUILD_CXXFLAGS += -O2
endif

ifdef CONFIG_FRAME_POINTER
KBUILD_CFLAGS += -fno-omit-frame-pointer -fno-optimize-sibling-calls
KBUILD_CXXFLAGS += -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
KBUILD_CFLAGS += -fomit-frame-pointer
KBUILD_CXXFLAGS += -fomit-frame-pointer
endif

ifeq ($(KBUILD_EXTMOD),)
# Generate some files
# ---------------------------------------------------------------------------
# KERNELRELEASE can change from a few different places, meaning version.h
# needs to be updated, so this check is forced on all builds

localver = $(subst $(space),, $(patsubst "%",%,$(CONFIG_LOCALVERSION)))

kernelrelease = $(KERNELVERSION)$(localver)
include/config/kernel.release: include/config/auto.conf FORCE
	$(Q)rm -f $@
	$(Q)echo $(kernelrelease) > $@

uts_len := 64
define filechk_utsrelease.h
	if [ `echo -n "$(KERNELRELEASE)" | wc -c ` -gt $(uts_len) ]; then \
	  echo '"$(KERNELRELEASE)" exceeds $(uts_len) characters' >&2;    \
	  exit 1;                                                         \
	fi;                                                               \
	(echo \#define UTS_RELEASE \"$(KERNELRELEASE)\";)
endef

define filechk_version.h
	(echo \#define NUCLEOS_VERSION_CODE $(shell                            \
	expr $(VERSION) \* 65536 + $(PATCHLEVEL) \* 256 + $(SUBLEVEL));        \
	echo '#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))';)
endef

# for drivers
include/nucleos/version.h: $(srctree)/Makefile FORCE
	$(call filechk,version.h)

include/nucleos/utsrelease.h: include/config/kernel.release FORCE
	$(call filechk,utsrelease.h)

# ---------------------------------------------------------------------------

# Things we need to do before we recursively start building the kernel
# or the modules are listed in "prepare".
# A multi level approach is used. prepareN is processed before prepareN-1.
# archprepare is used in arch Makefiles and when processed asm symlink,
# version.h and scripts_basic is processed / created.

# Listed in order dependency
PHONY += prepare prepare0 prepare1 prepare2

prepare2: include/config/kernel.release

prepare1: prepare2 include/nucleos/version.h include/nucleos/utsrelease.h \
	  include/config/auto.conf

archprepare: prepare1 scripts_basic

prepare0: archprepare FORCE

# All the preparing
prepare: prepare0
else # KBUILD_EXTMOD
# dummy targets
prepare: ;
scripts_tools: ;
endif

# default goal (build everything)
__all: __build

# Cancel implicit rules on top Makefile
$(CURDIR)/Makefile Makefile: ;

# include architecture goals, vars,...
# expand others if needed
include arch/$(SRCARCH)/Makefile

# compiler directories (includes & libs)
cc-install-dir = $(__cc-install-dir)
cc-include-dir = $(__cc-include-dir)
cc-lib-dir = $(__cc-lib-dir)
cc-version = $(__cc-version)
cc-version-str = $(__cc-version-str)

# export these values
export cc-install-dir cc-include-dir \
	cc-lib-dir cc-version cc-version-str
#! compiler directories (includes & libs)

ifeq ($(KBUILD_EXTMOD),)
# Here we have setup in this Makefile plus expanded in arch Makefile
# @devel: keep the libs as first for now
nucleos-dirs := $(libs-y) $(drivers-y) $(servers-y) $(core-y)
nucleos-alldirs := $(libs-y) $(drivers-y) $(servers-y) $(core-y) \
		   $(libs-) $(drivers-) $(servers-) $(core-)

# build object files
__build: $(nucleos-dirs)

# create kernel
kernel: __all

# build libraries
lib: __all $(libs-y)

# explicit goal to create tools
# Note that we must compile the `fixdep' too
tools: scripts_basic scripts_tools

# dirs to build
PHONY += $(nucleos-dirs)
$(nucleos-dirs): prepare scripts_tools
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=$@

###
# Cleaning is done on three levels.
# make clean     Delete most generated files
#                Leave enough to build external modules
# make mrproper  Delete the current configuration, and all generated files
# make distclean Remove editor backup files, patch leftover files and the like

# Directories & files removed with 'make clean'
CLEAN_DIRS  +=
CLEAN_FILES +=

# Directories & files removed with 'make mrproper'
MRPROPER_DIRS  += include/config
MRPROPER_FILES += .config .config.old include/asm .version .old_version \
		  include/nucleos/autoconf.h include/nucleos/version.h \
		  include/nucleos/utsrelease.h cscope*

# clean - Delete most, but leave enough to build external modules
#
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_,$(srctree) $(nucleos-alldirs))

PHONY += $(clean-dirs) clean archclean

$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: archclean $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' \
		-o -name '.tmp.*' -o -name '*.mri' \) \
		-type f -print | xargs rm -f

# mrproper - Delete all generated files, including .config
#
mrproper: rm-dirs  := $(wildcard $(MRPROPER_DIRS))
mrproper: rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs      := $(addprefix _mrproper_, scripts)

PHONY += $(mrproper-dirs) mrproper archmrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean archmrproper $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

# distclean
#
PHONY += distclean

distclean: mrproper
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -size 0 \
		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
		-type f -print | xargs rm -f

else # KBUILD_EXTMOD
module-dirs := $(KBUILD_EXTMOD)

__build: $(module-dirs)

PHONY += $(module-dirs)
$(module-dirs):
	$(Q)$(MAKE) -f scripts/mk/Makefile.build obj=$@

clean-dirs := $(addprefix _clean_,$(KBUILD_EXTMOD))

PHONY += $(clean-dirs) clean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: rm-dirs :=
clean: rm-files :=
clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find $(KBUILD_EXTMOD) $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '.tmp.*' \) \
		-type f -print | xargs rm -f

endif # KBUILD_EXTMOD


endif # ifeq ($(config-targets),1)
endif # ifeq ($(mixed-targets),1)

# Shorthand for $(Q)$(MAKE) -f scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/mk/Makefile.clean obj

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
