/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* For controlling tty modes. */
#ifndef __NUCLEOS_TERMIOS_H
#define __NUCLEOS_TERMIOS_H

#include <asm/termios.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)

/* Function Prototypes. */
int tcsendbreak(int _fildes, int _duration);
int tcdrain(int _filedes);
int tcflush(int _filedes, int _queue_selector);
int tcflow(int _filedes, int _action);
speed_t cfgetispeed(const struct termios *_termios_p);
speed_t cfgetospeed(const struct termios *_termios_p);
int cfsetispeed(struct termios *_termios_p, speed_t _speed);
int cfsetospeed(struct termios *_termios_p, speed_t _speed);
int tcgetattr(int _filedes, struct termios *_termios_p);
int tcsetattr (int _filedes, int _opt_actions, const struct termios *_termios_p);

//#define cfgetispeed(termios_p)          ((termios_p)->c_ispeed)
//#define cfgetospeed(termios_p)          ((termios_p)->c_ospeed)
//#define cfsetispeed(termios_p, speed)   ((termios_p)->c_ispeed = (speed), 0)
//#define cfsetospeed(termios_p, speed)   ((termios_p)->c_ospeed = (speed), 0)

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */ 

#endif /* __NUCLEOS_TERMIOS_H */
