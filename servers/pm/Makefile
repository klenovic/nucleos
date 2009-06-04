# Makefile for Data Store Server (DS)
app-y := pm.elf32
pm.elf32-obj-y := break.o dma.o exec.o forkexit.o getset.o main.o \
	          misc.o profile.o signal.o table.o time.o timers.o \
	          trace.o utility.o version.o
pm.elf32-extobj-y := lib/x86/rts/crtso.o

# files to be removed upon make clean
clean-files := ../../include/nucleos/compile.h

# dependencies on generated files need to be listed explicitly
$(src)/version.o: include/nucleos/compile.h

# compile.h changes depending on hostname, generation number, etc,
# so we regenerate it always.
# mkcompile_h will make sure to only update the
# actual file if its content has changed.

       chk_compile.h = :
 quiet_chk_compile.h = echo '  CHK     $@'
silent_chk_compile.h = :

include/nucleos/compile.h: FORCE
	@$($(quiet)chk_compile.h)
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkcompile_h $@ \
	"$(UTS_MACHINE)" "$(CONFIG_SMP)" "$(CONFIG_PREEMPT)" "$(CC) $(KBUILD_CFLAGS)"

ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

LDFLAGS_pm.elf32 := -Llib -lsys -ltimers -lnucc \
	            -L$(cc-lib-dir) -lgcc \
	            -Tservers/server_32.lds

$(src)/pm.elf32: servers/server_32.lds

# Create in minix3's aout format
pm_a_flags   := $(CONFIG_SERVERS_LINKAGE_TYPE)
pm_a_hdrlen  := 0x20
pm_a_cpu     := i386
pm_stackheap := 0x8000

e2a-y := pm.elf32,pm
