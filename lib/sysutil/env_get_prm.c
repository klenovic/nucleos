/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"
#include <nucleos/string.h>
#include <asm/setup.h>

static int argc = 0;
static char **argv = NULL;

static char *find_key(const char *params, const char *key);

/*===========================================================================*
 *				env_setargs				     *
 *===========================================================================*/
void env_setargs(arg_c, arg_v)
int arg_c;
char *arg_v[];
{
	argc= arg_c;
	argv= arg_v;
}

/*===========================================================================*
 *				env_get_param				     *
 *===========================================================================*/
int env_get_param(key, value, max_len)
char *key;				/* which key to look up */
char *value;				/* where to store value */
int max_len;				/* maximum length of value */
{
  kipc_msg_t m;
  static char mon_params[COMMAND_LINE_SIZE];	/* copy parameters here */
  char *key_value;
  int i, s, keylen;

  if (key == NULL)
  	return -EINVAL;

  keylen= strlen(key);
  for (i= 1; i<argc; i++)
  {
  	if (strncmp(argv[i], key, keylen) != 0)
  		continue;
	if (strlen(argv[i]) <= keylen)
		continue;
	if (argv[i][keylen] != '=')
		continue;
	key_value= argv[i]+keylen+1;
	if (strlen(key_value)+1 > max_len)
	      return(-E2BIG);
	strcpy(value, key_value);
	return 0;
  }

  /* Get copy of boot monitor parameters. */
  m.m_type = SYS_GETINFO;
  m.I_REQUEST = GET_CMDLINE_PARAMS;
  m.I_ENDPT = ENDPT_SELF;
  m.I_VAL_LEN = sizeof(mon_params);
  m.I_VAL_PTR = mon_params;
  if ((s=ktaskcall(SYSTASK, SYS_GETINFO, &m)) != 0) {
	printk("SYS_GETINFO: %d (size %u)\n", s, sizeof(mon_params));
	return(s);
  }

  /* We got a copy, now search requested key. */
  if ((key_value = find_key(mon_params, key)) == NULL)
	return(-ESRCH);

  /* Value found, see if it fits in the client's buffer. Callers assume that
   * their buffer is unchanged on error, so don't make a partial copy.
   */
  if ((strlen(key_value)+1) > max_len) return(-E2BIG);

  /* Make the actual copy. */
  strcpy(value, key_value);

  return 0;
}


/*==========================================================================*
 *				find_key					    *
 *==========================================================================*/
static char *find_key(params,name)
const char *params;
const char *name;
{
  register const char *namep;
  register char *envp;

  for (envp = (char *) params; *envp != 0;) {
	for (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	if (*namep == '\0' && *envp == '=') 
		return(envp + 1);
	while (*envp++ != 0)
		;
  }
  return(NULL);
}

