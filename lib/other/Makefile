# Makefile for lib/other.
lib-y := libnucc.a
libnucc.a-obj-y := _pm_findproc.o adddma.o brk.o deldma.o devctl.o getdma.o \
		   getnpid.o getnprocnr.o getpprocnr.o getprocnr.o getsigset.o \
		   getsysinfo.o lseek64.o mapdriver5.o mount.o sbrk.o svrctl.o \
		   vm_dmacalls.o asynchio.o basename.o configfile.o ctermid.o \
		   cuserid.o environ.o errno.o fslib.o getpagesize.o getttyent.o \
		   popen.o fdopen.o paramvalue.o putenv.o setenv.o reboot.o \
		   sys_eniop.o syslog.o getnuid.o itoa.o getdents.o getpwent.o \
		   getngid.o vm_set_priv.o vm_query_exit.o minix_rs.o umount.o

ccflags-y := -D__UKERNEL__
