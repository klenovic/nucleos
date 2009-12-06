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

#define ASM_SYSCALL_GET_EXITSTATUS(off, argnr)	"mov " #off "(%%esp), %" #argnr "\t\n"

pid_t waitpid(pid_t pid, int *status, int options)
{
	int resultvar = 0;
	int __status = 0;

	__asm__(ASM_SYSCALL_ALLOC_MESSAGE
		"mov %2, %%eax\t\n"
		ASM_SYSCALL_SAVE_CLOBBERED_REGS
		ASM_SYSCALL_CALL_SYSTEM
		ASM_SYSCALL_RESTORE_CLOBBERED_REGS
		ASM_SYSCALL_GET_MSGRC
		ASM_SYSCALL_GET_EXITSTATUS(8,1)
		ASM_SYSCALL_DEALLOC_MESSAGE
		: "=a" (resultvar), "=r"(__status)
		: "i" (__NNR_waitpid), "b"(pid), "c"(options)
		: "memory", "cc"
	);

	if (__builtin_expect(INTERNAL_SYSCALL_ERROR_P(resultvar, ), 0)) {
		__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));
		return -1;
	}

	if (status) {
		*status = __status;
	}

	return resultvar;
}
