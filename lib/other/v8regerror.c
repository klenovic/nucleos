/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	regerror() - Default regexp error report	Author: Kees J. Bot
 *								12 Jun 1999
 *
 * A better version of this routine should be supplied by the user in
 * the program using regexps.
 */
#include <stdio.h>
#define const		/* avoid "const poisoning" */
#include <regexp.h>
#undef const

void regerror(char *message)
{
	fprintf(stderr, "regexp error: %s\n", message);
}
