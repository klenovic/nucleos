/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/termios.h> header is used for controlling tty modes. */

#ifndef __ASM_X86_TERMIOS_H
#define __ASM_X86_TERMIOS_H

#include <asm/termbits.h>

/* Window size. This information is stored in the TTY driver but not used.
 * This can be used for screen based applications in a window environment.
 * The ioctls TIOCGWINSZ and TIOCSWINSZ can be used to get and set this
 * information.
 */
struct winsize {
	unsigned short ws_row;         /* rows, in characters */
	unsigned short ws_col;         /* columns, in characters */
	unsigned short ws_xpixel;      /* horizontal size, pixels */
	unsigned short ws_ypixel;      /* vertical size, pixels */
};

#endif /* __ASM_X86_TERMIOS_H */
