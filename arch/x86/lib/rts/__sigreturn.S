/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This routine is the low-level code for returning from signals.
 * It calls __sigreturn, which is the normal "system call" routine.
 * Both __sigreturn and sigreturn are needed.
 */
	.section ".text", "ax"

	.globl __sigreturn
__sigreturn:
	add $16, %esp
	jmp sigreturn
