lib-y := libsys.a
libsys.a-obj-y := asynsend.o kmalloc.o kprintf.o kputc.o tickdelay.o get_randomness.o getuptime.o \
		  getuptime2.o env_get_prm.o env_parse.o env_panic.o env_prefix.o \
		  fkey_ctl.o micro_delay.o report.o taskcall.o read_tsc.o read_tsc_64.o \
		  ser_putc.o stacktrace.o sys_hz.o timing.o profile_extern.o profile.o

ccflags-y := -D__UKERNEL__
