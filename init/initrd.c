/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include <nucleos/stddef.h>

/* Compile ramdisk translated to array of chars. */
__section(".data.initrd, \"aw\"") unsigned char initrd[] =
{
	/* Automatically generated (see Makefile) */
	#include "initrd.h"
};

size_t initrd_size = sizeof(initrd);
