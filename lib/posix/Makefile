# Makefile for lib/posix.
lib-y := libnucc.a
libnucc.a-obj-y := alarm.o chdir.o close.o creat.o dup2.o dup.o execle.o execl.o execve.o \
		   _exit.o fcntl.o fcntl.o fork.o fstat.o getgid.o getopt.o getpid.o \
		   getpriority.o gettimeofday.o getuid.o ioctl.o isatty.o kill.o link.o \
		   lseek.o mknod.o mmap.o munmap.o munmap_text.o nanosleep.o nice.o opendir.o \
		   open.o pause.o pipe.o readdir.o read.o select.o setpriority.o setsid.o \
		   setuid.o sigaction.o sigprocmask.o sigreturn.o sleep.o stat.o stime.o \
		   tcgetattr.o time.o times.o umask.o unlink.o vm_getphys.o vm_getrefcount.o \
		   vm_remap.o vm_unmap.o wait.o waitpid.o waitpid.o write.o

ccflags-y := -D__UKERNEL__
