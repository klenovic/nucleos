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
 * @brief Extract low 32 bits of a 64 bit number
 */
unsigned long ex64lo(u64 i)
{
	return (unsigned long)i;
}

/**
 * @brief Extract high 32 bits of a 64 bit number
 */
unsigned long ex64hi(u64 i)
{
	return (unsigned long)(i>>32);
}
