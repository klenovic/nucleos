/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>

#ifdef CONFIG_BUILTIN_INITRD
#include <nucleos/stddef.h>
#endif

/* Compile ramdisk translated to array of chars. */
#ifndef CONFIG_BUILTIN_INITRD
__attribute__((used,section(".data"))) static unsigned char initrd[] =
#else
unsigned char initrd[] =
#endif
{
  /* Automatically generated (see Makefile) */
  #include "initrd.h"
};

#ifdef CONFIG_BUILTIN_INITRD
size_t initrd_size = sizeof(initrd);
#endif
