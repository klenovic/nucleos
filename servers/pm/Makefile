# Makefile for Data Store Server (DS)
app-y := pm.elf32
pm.elf32-type := server
pm.elf32-obj-y := alarm.o break.o dma.o exec.o forkexit.o getset.o main.o \
	          misc.o profile.o signal.o table.o time.o timers.o \
	          ptrace.o utility.o version.o pm-syms.o

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

LDFLAGS_pm.elf32 := -L$(lib-arch) -Llib \
		    -lsys -ltimers -lnucc \
		    -Tservers/server_32.lds

$(src)/pm.elf32: servers/server_32.lds

# Create in minix3's aout format
pm_a_flags   := 0x00
pm_a_hdrlen  := 0x20
pm_a_cpu     := i386
pm_stackheap := 64k

e2a-y := pm.elf32,pm
