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
 * @brief Unsigned converted to 64 bit
 */
u64 cvu64(unsigned i)
{
	return (u64)i;
}

/**
 * @brief Unsigned long converted to 64 bit
 */
u64 cvul64(unsigned long i)
{
	return (u64)i;
}
