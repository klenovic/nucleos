# Makefile for lib/other.
lib-y := libnucc.a
libnucc.a-obj-y := __pm_findproc.o _adddma.o _brk.o _cprofile.o _deldma.o _devctl.o _getdents.o _getdma.o \
	       _getnpid.o _getnprocnr.o _getpprocnr.o _getprocnr.o _getsigset.o _getsysinfo.o _lseek64.o \
	       _mapdriver5.o _reboot.o _sbrk.o _seekdir.o _sprofile.o _svrctl.o _sysuname.o _vm_dmacalls.o \
	       asynchio.o basename.o bcmp.o bcopy.o bzero.o configfile.o crypt.o ctermid.o cuserid.o \
	       environ.o errno.o fdopen.o ffs.o flock.o fslib.o fts.o fsversion.o getgrent.o getlogin.o \
	       getpagesize.o getpass.o getpeuid.o getpwent.o getttyent.o getw.o hypot.o index.o itoa.o \
	       loadname.o lock.o lrand.o lsearch.o memccpy.o mstats.o mtab.o nlist.o paramvalue.o peekpoke.o \
	       popen.o putenv.o putw.o random.o rindex.o setenv.o setgroups.o settimeofday.o stderr.o strdup.o \
	       strlcat.o strlcpy.o strtok_r.o swab.o sys_eniop.o syscall.o sysconf.o syslog.o taskcall.o \
	       telldir.o termcap.o ttyname.o ttyslot.o v8regerror.o v8regexp.o v8regsub.o writev.o

ccflags-y := -O -D_MINIX -D_POSIX_SOURCE -I$(srctree)/servers/
