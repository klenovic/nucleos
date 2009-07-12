/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __SERVER_VM_UTIL_H
#define __SERVER_VM_UTIL_H

#include <servers/vm/vm.h>
#include <servers/vm/glo.h>

#define ELEMENTS(a) (sizeof(a)/sizeof((a)[0]))

#if SANITYCHECKS
#define vm_assert(cond) {				\
	if(vm_sanitychecklevel > 0 && !(cond)) {	\
		printf("VM:%s:%d: assert failed: %s\n",	\
			__FILE__, __LINE__, #cond);	\
		panic("VM", "assert failed", NO_NUM);	\
	}						\
	}
#else
#define vm_assert(cond)	;
#endif

#define vm_panic(str, n) { char _pline[100]; \
	sprintf(_pline, "%s:%d: %s", __FILE__, __LINE__, (str));	\
	panic("VM", _pline, (n));					\
	}

#endif /* __SERVER_VM_UTIL_H */
