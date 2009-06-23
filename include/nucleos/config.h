/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _CONFIG_H
#define _CONFIG_H

/* This file sets configuration parameters for the MINIX kernel, FS, and PM.
 * It is divided up into two main sections.  The first section contains
 * user-settable parameters.  In the second section, various internal system
 * parameters are set based on the user-settable parameters.
 */

/*===========================================================================*
 *              This section contains user-settable parameters               *
 *===========================================================================*/
/* Word size in bytes (a constant equal to sizeof(int)). */
#ifdef __GNUC__
#define _WORD_SIZE      _EM_WSIZE
#define _PTR_SIZE       _EM_WSIZE
#endif

#endif /* _CONFIG_H */
