/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_tty.h - Terminal ioctl() command codes.
 *							Author: Kees J. Bot
 *								23 Nov 2002
 *
 */

#ifndef __ASM_GENERIC_IOCTLS_TTY_H
#define __ASM_GENERIC_IOCTLS_TTY_H

#include <nucleos/ioctl.h>

/* Terminal ioctls. */
#define TCGETS		_IOR('T',  8, struct termios) /* tcgetattr */
#define TCSETS		_IOW('T',  9, struct termios) /* tcsetattr, TCSANOW */
#define TCSETSW		_IOW('T', 10, struct termios) /* tcsetattr, TCSADRAIN */
#define TCSETSF		_IOW('T', 11, struct termios) /* tcsetattr, TCSAFLUSH */
#define TCSBRK		_IOW('T', 12, int)	      /* tcsendbreak */
#define TCDRAIN		_IO ('T', 13)		      /* tcdrain */
#define TCFLOW		_IOW('T', 14, int)	      /* tcflow */
#define TCFLSH		_IOW('T', 15, int)	      /* tcflush */
#define	TIOCGWINSZ	_IOR('T', 16, struct winsize)
#define	TIOCSWINSZ	_IOW('T', 17, struct winsize)
#define	TIOCGPGRP	_IOW('T', 18, int)
#define	TIOCSPGRP	_IOW('T', 19, int)
#define TIOCSFON_OLD	_IOW('T', 20, u8_t [8192])
#define TIOCSFON	_IOW_BIG(1, u8_t [8192])

/* Legacy <sgtty.h> */
#define TIOCGETP	_IOR('t',  1, struct sgttyb)
#define TIOCSETP	_IOW('t',  2, struct sgttyb)
#define TIOCGETC	_IOR('t',  3, struct tchars)
#define TIOCSETC	_IOW('t',  4, struct tchars)

/* Keyboard ioctls. */
#define KIOCBELL        _IOW('k', 1, struct kio_bell)
#define KIOCSLEDS       _IOW('k', 2, struct kio_leds)
#define KIOCSMAP	_IOW('k', 3, keymap_t)

/* /dev/video ioctls. */
#define TIOCMAPMEM	_IORW('v', 1, struct mapreqvm)
#define TIOCUNMAPMEM	_IORW('v', 2, struct mapreqvm)

#endif /* __ASM_GENERIC_IOCTLS_TTY_H */
