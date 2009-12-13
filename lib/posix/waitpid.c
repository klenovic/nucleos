/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/wait.h>

pid_t waitpid(pid_t pid, int *status, int options)
{
	int resultvar = 0;
	int __msg[9] = {0, __NNR_waitpid};

	__asm__ __volatile__(ASM_SYSCALL_CALL_SYSTEM
		: "=a" (resultvar)
		: "0" (__msg), "b"(pid), "c"(options)
		: "memory", "cc"
	);

	resultvar = __msg[1];

	if (__builtin_expect(INTERNAL_SYSCALL_ERROR_P(resultvar, ), 0)) {
		__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));
		return -1;
	}

	if (status) {
		*status = __msg[2];
	}

	return resultvar;
}
