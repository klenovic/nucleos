/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * assert.c - diagnostics
 */

#include <assert.h>
#include <stdio.h>
#include <nucleos/const.h>
#include <nucleos/sysutil.h>

void __bad_assertion(const char *mess) {
	printk("%s", mess);
	panic(NULL, NULL, NO_NUM);
}
