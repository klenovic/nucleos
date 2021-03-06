/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _NUCLEOS_STRINGIFY_H
#define _NUCLEOS_STRINGIFY_H

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

/** @brief Helper macro for __stringify */
#define __stringify_1(x...)	#x

/** @brief Turn preprocessor symbol definition \a sym into string. */
#define __stringify(x...)	__stringify_1(x)

#endif /* !_NUCLEOS_STRINGIFY_H */
