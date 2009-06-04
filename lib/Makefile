# this directory implements these libraries
lib-y := libsys.a libdriver.a libdriver_asyn.a libtimers.a libnucc.a \
	 libnucc_16.a libcurses.a

libsys.a-obj-y := syslib/ sysutil/
libdriver.a-obj-y := libdriver/
libdriver_asyn.a-obj-y := libdriver_asyn/
libtimers.a-obj-y := timers/
libnucc.a-obj-y := $(SRCARCH) ansi/ editline/ gnu/ gnu/ ip/ math/ other/ \
	           posix/ regex/ syscall/ stdio/ stdtime/
libnucc_16.a-obj-y := $(SRCARCH)/
libcurses.a-obj-y := curses/
