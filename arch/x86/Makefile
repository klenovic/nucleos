# select defconfig based on actual architecture
ifeq ($(ARCH),x86)
        KBUILD_DEFCONFIG := i386_defconfig
else
        KBUILD_DEFCONFIG := $(ARCH)_defconfig
endif

# BITS is used as extension for files which are available in a 32 bit
# and a 64 bit version to simplify shared Makefiles.
# e.g.: obj-y += foo_$(BITS).o
export BITS

ifeq ($(CONFIG_X86_32),y)
        BITS := 32
        UTS_MACHINE := i386
        CHECKFLAGS += -D__i386__

        biarch := $(call cc-option,-m32)
        KBUILD_AFLAGS += $(biarch)
        KBUILD_CFLAGS += $(biarch)

        # @nucleos: fp code will forbiden inside kernel (enable it then). Current code contains some
        #           fp code (see readclock.drv) thus the kernel won't compile with this option.
        # KBUILD_CFLAGS += -msoft-float

        # @nucleos: doesn't work with these
        # KBUILD_CFLAGS += -msoft-float -mregparm=3 -freg-struct-return

        # prevent gcc from keeping the stack 16 byte aligned
        KBUILD_CFLAGS += $(call cc-option,-mpreferred-stack-boundary=2)

        # include compiler stuffs
        include arch/$(SRCARCH)/Makefile_32.cpu

        KBUILD_CFLAGS += $(cflags-y)
endif

boot := arch/x86/boot/

# speed up the build
KBUILD_CFLAGS += -pipe

core-y += arch/$(SRCARCH)/kernel/
core-y += arch/$(SRCARCH)/servers/
core-y += arch/$(SRCARCH)/monitor/
core-y += arch/$(SRCARCH)/boot/
libs-y += arch/$(SRCARCH)/lib/

# tied with gcc
__gcc-version = $(shell echo "__GNUC__ __GNUC_MINOR__ __GNUC_PATCHLEVEL__" | gcc -E - | grep -v "\#" | tr ' ' .)
__cc-version = $(__gcc-version) 
__cc-version-str = $(shell echo __VERSION__ | gcc -E - | grep -v "\#")

# gcc include directory
__cc-install-dir = $(dir $(shell $(CC) -print-libgcc-file-name))
__cc-include-dir = $(__cc-install-dir)include
__cc-lib-dir = $(__cc-install-dir)

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)
