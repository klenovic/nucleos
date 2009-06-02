/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"
#include <stdlib.h>
#include <string.h>

/*=========================================================================*
 *				env_prefix				   *
 *=========================================================================*/
PUBLIC int env_prefix(env, prefix)
char *env;		/* environment variable to inspect */
char *prefix;		/* prefix to test for */
{
/* An environment setting may be prefixed by a word, usually "pci".  
 * Return TRUE if a given prefix is used.
 */
  char value[EP_BUF_SIZE];
  char punct[] = ":,;.";
  int s;
  size_t n;

  if ((s = env_get_param(env, value, sizeof(value))) != 0) {
  	if (s != ESRCH)		/* only error allowed */
  	printf("WARNING: env_get_param() failed in env_prefix(): %d\n", s);
  	return FALSE;
  }
  n = strlen(prefix);
  return(strncmp(value, prefix, n) == 0
	&& strchr(punct, value[n]) != NULL);
}

