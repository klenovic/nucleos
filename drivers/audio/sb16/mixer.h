/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef SB16_MIXER_H
#define SB16_MIXER_H

_PROTOTYPE( int mixer_init, (void));
_PROTOTYPE( int mixer_ioctl, (int request, void *val, int *len));

_PROTOTYPE( int mixer_set, (int reg, int data));
_PROTOTYPE( int mixer_get, (int reg));

#endif
