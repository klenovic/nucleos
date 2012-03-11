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
#include <nucleos/math64.h>

/**
 * @brief 64 bit divided by unsigned giving unsigned long.
 */
unsigned long div64u(u64 i, unsigned j)
{
	return (unsigned long)div_u64(i, j);
}

/**
 * @brief 64 bit divided by unsigned giving unsigned reminder.
 */
unsigned rem64u(u64 i, unsigned j)
{
	u32 rem;

	div_u64_rem(i, j, &rem);

	return (unsigned)rem;
}
