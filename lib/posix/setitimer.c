/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Copyright (c) 2009 Vrije Universiteit, Amsterdam.
 * See the copyright notice in file LICENSE.minix3.
 */
#include <nucleos/unistd.h>
#include <nucleos/time.h>
#include <asm/syscall.h>

int setitimer(int which, const struct itimerval *__restrict value,
	      struct itimerval *__restrict ovalue)
{
	return INLINE_SYSCALL(setitimer, 3, which, value, ovalue);
}
