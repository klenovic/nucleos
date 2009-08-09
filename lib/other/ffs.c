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
/*  ffs(3)
 *
 *  Author: Terrence W. Holm          Sep. 1988
 */
int ffs(int word);

int ffs(word)
int word;
{
  int i;

  if (word == 0) return(0);

  for (i = 1;; ++i, word >>= 1)
	if (word & 1) return(i);
}
