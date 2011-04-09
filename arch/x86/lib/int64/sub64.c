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
 * @brief 64 bit subtraction.
 * @note Always returns a positive value |i-j|
 */
uint64_t sub64(uint64_t i, uint64_t j)
{
 return (uint64_t)((i>=j) ? (i-j):(j-i));
}
