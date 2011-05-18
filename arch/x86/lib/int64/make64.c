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
 * @brief Make a 64 bit number from two 32 bit halves
 */
u64 make64(unsigned long lo, unsigned long hi)
{
	u64 tmp_hi = hi;

	return (u64)((tmp_hi<<32)|lo);
}
