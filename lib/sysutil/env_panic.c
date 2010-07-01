/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"
#include <nucleos/string.h>

/*=========================================================================*
 *				env_panic				   *
 *=========================================================================*/
void env_panic(key)
char *key;		/* environment variable whose value is bogus */
{
  static char value[EP_BUF_SIZE] = "<unknown>";
  int s;
  if ((s=env_get_param(key, value, sizeof(value))) == 0) {
  	if (s != -ESRCH)		/* only error allowed */
  	printf("WARNING: env_get_param() failed in env_panic(): %d\n", s);
  }
  printf("Bad environment setting: '%s = %s'\n", key, value);
  panic("","", NO_NUM);
}

