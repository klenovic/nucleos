/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_KBDIO_H
#define __NUCLEOS_KBDIO_H

#include <nucleos/time.h>

typedef struct kio_bell
{
	unsigned kb_pitch;		/* Bell frequency in HZ */
	unsigned long kb_volume;	/* Volume in micro volts */
	struct timeval kb_duration;
} kio_bell_t;

typedef struct kio_leds
{
	unsigned kl_bits;
} kio_leds_t;

#define KBD_LEDS_NUM	0x1
#define KBD_LEDS_CAPS	0x2
#define KBD_LEDS_SCROLL	0x4

#endif /* __NUCLEOS_KBDIO_H */
