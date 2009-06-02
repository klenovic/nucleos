/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
uint64_t add64u(uint64_t i, unsigned j)
{
  return (uint64_t)(i+j);
}

/**
 * @brief Unsigned long to 64 bit addition
 */
uint64_t add64ul(uint64_t i, unsigned long j)
{
  return (uint64_t)(i+j);
}
