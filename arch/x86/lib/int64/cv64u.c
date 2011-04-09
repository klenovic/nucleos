/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/kernel.h>
#include <nucleos/types.h>

/**
 * @brief 64 bit converted to unsigned.
 * @note If \a i is bigger then 0xffffffff then 
 *       ULONG_MAX is returned.
 */
unsigned cv64u(uint64_t i)
{
  // return ULONG_MAX if really big
  return ((i>>32) ? ULONG_MAX : (unsigned)i);
}

/**
 * @brief 64 bit converted to unsigned long.
 * @note If \a i is bigger then 0xffffffff then 
 *       ULONG_MAX is returned.
 */
unsigned long cv64ul(uint64_t i)
{
  // return ULONG_MAX if really big
  return ((i>>32) ? ULONG_MAX : (unsigned long)i);
}
