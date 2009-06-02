/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	alloca.h - The dreaded alloca() function.
 */

#ifndef _ALLOCA_H
#define _ALLOCA_H

#ifndef _TYPES_H
#include <sys/types.h>
#endif

#if __GNUC__

/* The compiler recognizes this special keyword, and inlines the code. */
#define alloca(size)	__builtin_alloca(size)

#endif /* __GCC__ */

#if __ACK__ || __CCC__

_PROTOTYPE(void *alloca, (size_t _size)					);

#endif /* __ACK__ || __CCC__ */

#endif /* _ALLOCA_H */
