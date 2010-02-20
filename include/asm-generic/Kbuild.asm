#
# The Kbuild.asm acccording to linux one. This file is included
# in architecture Kbuild
#
ifneq ($(wildcard $(srctree)/arch/$(SRCARCH)/include/asm/a.out.h \
      		  $(srctree)/include/asm-$(SRCARCH)/a.out.h),)
header-y += a.out.h
endif
header-y += auxvec.h
header-y += byteorder.h
header-y += bitsperlong.h
header-y += errno.h
header-y += fcntl.h
header-y += ioctl.h
header-y += ioctls.h
# n/a header-y += ipcbuf.h
header-y += mman.h
# n/a header-y += msgbuf.h
header-y += param.h
# n/a header-y += poll.h
header-y += posix_types.h
# n/a header-y += ptrace.h
# n/a header-y += resource.h
# n/a header-y += sembuf.h
# n/a header-y += setup.h
# n/a header-y += shmbuf.h
header-y += sigcontext.h
header-y += siginfo.h
header-y += signal.h
# n/a header-y += socket.h
# n/a header-y += sockios.h
header-y += stat.h
header-y += statfs.h
header-y += swab.h
header-y += termbits.h
header-y += termios.h
header-y += types.h
header-y += unistd.h
