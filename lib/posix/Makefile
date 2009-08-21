# Makefile for lib/posix.
lib-y := libnucc.a
libnucc.a-obj-y := __exit.o _access.o _alarm.o _cfgetispeed.o _cfgetospeed.o _cfsetispeed.o _cfsetospeed.o \
	       _chdir.o _chmod.o _chown.o _chroot.o _close.o _closedir.o _creat.o _dup.o _dup2.o _execl.o _execle.o \
	       _execlp.o _execv.o _execve.o _execvp.o _fchmod.o _fchown.o _fcntl.o _fork.o _fpathconf.o _fstat.o _fstatfs.o _fsync.o \
	       _getcwd.o _getegid.o _geteuid.o _getgid.o _getgroups.o _getitimer.o _getpgrp.o _getpid.o _getppid.o _getuid.o \
	       _ioctl.o _isatty.o _kill.o _killpg.o _link.o _lseek.o _lstat.o _mkdir.o _mkfifo.o _mknod.o _mmap.o _nanosleep.o _open.o \
	       _opendir.o _pathconf.o _pause.o _pipe.o _ptrace.o _read.o _readdir.o _readlink.o _rename.o _rewinddir.o \
	       _rmdir.o _select.o _setgid.o _setitimer.o _setsid.o _setuid.o _sigaction.o _sigpending.o _sigprocmask.o _sigreturn.o \
	       _sigset.o _sigsuspend.o _sleep.o _stat.o _stime.o _symlink.o _sync.o _tcdrain.o _tcflow.o \
	       _tcflush.o _tcgetattr.o _tcsendbreak.o _tcsetattr.o _time.o _times.o _truncate.o _umask.o \
	       _uname.o _unlink.o _utime.o _wait.o _waitpid.o _write.o getloadavg.o getopt.o gettimeofday.o glob.o \
	       priority.o nice.o usleep.o

ccflags-y := -D__UKERNEL__
