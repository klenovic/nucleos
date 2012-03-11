/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	paramvalue() - decode kernel parameter values	Author: Kees J. Bot
 *								7 May 1994
 * The kernel returns the results of parameter queries
 * by the XXQUERYPARAM svrctl calls as an array of hex digits, like this:
 * "75020000,080C0000".  These are the values of two four-byte variables.
 * Paramvalue() decodes such a string.
 */
#define nil 0
#include <nucleos/stddef.h>
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/types.h>
#include <nucleos/queryparam.h>

size_t paramvalue(char **value, void *address, size_t size)
/* Decode the string *value storing the result in the object at address with
 * the given size.  *value is left at the next parameter, *address is padded
 * with zeros if needed, and the actual size of the value is returned.
 */
{
	unsigned char *addr= address;
	char *v= *value;
	int nibble;
	size_t n;

	n= 0;

	while (*v != 0 && *v != ',') {
		nibble= *v++ - '0';
		if (nibble > 0x9) nibble= nibble + '0' - 'A' + 0xA;
		if (nibble > 0xF) nibble= nibble + 'A' - 'a';
		if (size > 0) {
			if (n % 2 == 0) {
				*addr= nibble << 4;
			} else {
				*addr++|= nibble;
				size--;
			}
			n++;
		}
	}
	while (size > 0) { *addr++= 0; size--; }
	while (*v != 0 && *v++ != ',') {}
	*value= v;
	return n / 2;
}
