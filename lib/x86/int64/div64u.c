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
 * @brief 64 bit divided by unsigned giving unsigned long.
 */
unsigned long div64u(uint64_t i, unsigned j)
{
  return (unsigned long)(i/j);
}

/**
 * @brief 64 bit divided by unsigned giving unsigned reminder.
 */
unsigned rem64u(uint64_t i, unsigned j)
{
  return (unsigned)(i%j);
}
