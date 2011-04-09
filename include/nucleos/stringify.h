/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_STRINGIFY_H
#define __NUCLEOS_STRINGIFY_H

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

/** @brief Helper macro for __stringify */
#define __stringify_1(x...)	#x

/** @brief Turn preprocessor symbol definition \a sym into string. */
#define __stringify(x...)	__stringify_1(x)

#endif /* !__NUCLEOS_STRINGIFY_H */
