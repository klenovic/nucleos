#
# Makefile for x86-compatible CPU details, features and quirks
#
obj-y := capflags.o powerflags.o

quiet_cmd_mkcapflags = MKCAP   $@
      cmd_mkcapflags = $(PERL) $(srctree)/$(src)/mkcapflags.pl $< $@

cpufeature = $(src)/../../include/asm/cpufeature.h

targets += capflags.c
#$(obj)/capflags.c: $(cpufeature) $(src)/mkcapflags.pl FORCE
$(src)/capflags.c: $(cpufeature) $(src)/mkcapflags.pl FORCE
	$(call if_changed,mkcapflags)
