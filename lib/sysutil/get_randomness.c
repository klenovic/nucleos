/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nucleos/profile.h>
#include <nucleos/syslib.h>
#include <nucleos/type.h>
#include <nucleos/sysutil.h>

/*===========================================================================*
 *                              get_randomness                               *
 *===========================================================================*/
void get_randomness(rand, source)
struct k_randomness *rand;
int source;
{
/* Use architecture-dependent high-resolution clock for
 * raw entropy gathering.
 */
  int r_next;
  unsigned long tsc_high, tsc_low;

  source %= RANDOM_SOURCES;
  r_next= rand->bin[source].r_next;
  read_tsc((u32_t*)&tsc_high, (u32_t*)&tsc_low);
  rand->bin[source].r_buf[r_next] = tsc_low;
  if (rand->bin[source].r_size < RANDOM_ELEMENTS) {
        rand->bin[source].r_size ++;
  }
  rand->bin[source].r_next = (r_next + 1 ) % RANDOM_ELEMENTS;
}


