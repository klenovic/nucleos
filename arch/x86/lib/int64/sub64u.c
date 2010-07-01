/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>

/**
 * @brief Unsigned from 64 bit subtraction.
 * @note Assumes that i>j.
 */
uint64_t sub64u(uint64_t i, unsigned j)
{
  return (uint64_t)(i-j);
}

/**
 * @brief Unsigned long from 64 bit subtraction.
 * @note Assumes that i>j.
 */
uint64_t sub64ul(uint64_t i, unsigned long j)
{
  return (uint64_t)(i-j);
}
