/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>

/**
 * @brief 64 bit subtraction.
 * @note Always returns a positive value |i-j|
 */
u64 sub64(u64 i, u64 j)
{
	return (u64)((i>=j) ? (i-j):(j-i));
}
