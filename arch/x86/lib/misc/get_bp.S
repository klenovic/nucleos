/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// get_bp.s
// return EBP in EAX
// based on code by Philip Homburg

.section .text

.globl get_bp
get_bp:
	mov	%ebp, %eax
	ret
