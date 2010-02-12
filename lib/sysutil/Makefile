lib-y := libsys.a
libsys.a-obj-y := asynsend.o kprintf.o kputc.o tickdelay.o get_randomness.o getuptime.o \
		  getuptime2.o env_get_prm.o env_parse.o env_panic.o env_prefix.o \
		  fkey_ctl.o micro_delay.o report.o read_tsc.o read_tsc_64.o \
		  ser_putc.o stacktrace.o sys_hz.o timing.o profile_extern.o profile.o

libsys.a-obj-$(CONFIG_IDLE_TSC) += getidle.o

ccflags-y := -D__UKERNEL__
