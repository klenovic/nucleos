# this directory implements these libraries
lib-y := libsys.a libdriver.a libtimers.a libnucc.a libkernel.a

libkernel.a-obj-y += kernel/
libsys.a-obj-y := syslib/ sysutil/
libdriver.a-obj-y := libdriver/
libtimers.a-obj-y := timers/
libnucc.a-obj-y := ansi/ ip/ math64/ other/ posix/ regex/ stdio/ sysvipc/

libnucc.a-obj-y += ctype.o kstrtox.o string.o get_param_value.o
CFLAGS_string.o += -D__KERNEL__
CFLAGS_kstrtox.o += -D__KERNEL__
CFLAGS_get_param_value.o += -D__KERNEL__
