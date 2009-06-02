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
 * @brief Unsigned converted to 64 bit
 */
uint64_t cvu64(unsigned i)
{
  return (uint64_t)i;
}

/**
 * @brief Unsigned long converted to 64 bit
 */
uint64_t cvul64(unsigned long i)
{
  return (uint64_t)i;
}
