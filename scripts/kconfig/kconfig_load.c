/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "lkc.h"

#define P(name,type,arg)	type (*name ## _p) arg
#include "lkc_proto.h"
#undef P

void kconfig_load(void)
{
	void *handle;
	char *error;

	handle = dlopen("./libkconfig.so", RTLD_LAZY);
	if (!handle) {
		handle = dlopen("./scripts/kconfig/libkconfig.so", RTLD_LAZY);
		if (!handle) {
			fprintf(stderr, "%s\n", dlerror());
			exit(1);
		}
	}

#define P(name,type,arg)			\
{						\
	name ## _p = dlsym(handle, #name);	\
        if ((error = dlerror()))  {		\
                fprintf(stderr, "%s\n", error);	\
		exit(1);			\
	}					\
}
#include "lkc_proto.h"
#undef P
}
