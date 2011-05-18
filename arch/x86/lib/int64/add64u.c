/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>

/**
 * @brief Unsigned to 64 bit addition
 */
u64 add64u(u64 i, unsigned j)
{
	return (u64)(i+j);
}

/**
 * @brief Unsigned long to 64 bit addition
 */
u64 add64ul(u64 i, unsigned long j)
{
	return (u64)(i+j);
}
