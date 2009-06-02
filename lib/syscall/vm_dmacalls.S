/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
.section .text

.globl	vm_adddma
.globl	vm_deldma
.globl	vm_getdma
.p2align 1
vm_adddma:
	jmp	_vm_adddma
vm_deldma:
	jmp	_vm_deldma
vm_getdma:
	jmp	_vm_getdma

