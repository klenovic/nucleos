# Makefile for lib/posix.
lib-y := libnucc.a
libnucc.a-obj-y := _exit.o access.o alarm.o cfgetispeed.o cfgetospeed.o cfsetispeed.o cfsetospeed.o \
		   chdir.o chmod.o chown.o chroot.o close.o closedir.o creat.o dup.o dup2.o execl.o \
		   execle.o execlp.o execv.o execve.o execvp.o fchdir.o fchmod.o fchown.o fcntl.o fork.o \
		   fpathconf.o fstat.o fstatfs.o fsync.o ftruncate.o getcwd.o getegid.o geteuid.o getgid.o \
		   getgroups.o getitimer.o getpgrp.o getpid.o getppid.o getpriority.o getuid.o ioctl.o isatty.o \
		   kill.o killpg.o link.o lseek.o lstat.o mkdir.o mkfifo.o mknod.o mmap.o nanosleep.o \
		   open.o opendir.o pathconf.o pause.o pipe.o ptrace.o read.o readdir.o readlink.o \
		   rename.o rewinddir.o rmdir.o select.o setgid.o setitimer.o setpriority.o setsid.o setuid.o sigaction.o \
		   sigpending.o sigprocmask.o sigreturn.o sigset.o sigsuspend.o sleep.o stat.o stime.o \
		   symlink.o sync.o tcdrain.o tcflow.o tcflush.o tcgetattr.o tcsendbreak.o tcsetattr.o \
		   time.o times.o truncate.o umask.o uname.o unlink.o utime.o wait.o waitpid.o write.o \
		   getloadavg.o getopt.o gettimeofday.o glob.o nice.o usleep.o

ccflags-y := -D__UKERNEL__
