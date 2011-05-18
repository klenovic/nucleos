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
 * @brief 64 bit compare
 */
int cmp64(u64 i, u64 j)
{
	return ((i>j) - (i<j));
}

/**
 * @brief Compare 64 bit value with unsigned
 */
int cmp64u(u64 i, unsigned j)
{
	return ((i>j) - (i<j));
}

/**
 * @brief Compare 64 bit value with unsigned long
 */
int cmp64ul(u64 i, unsigned long j)
{
	return ((i>j) - (i<j));
}
