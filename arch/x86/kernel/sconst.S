/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Miscellaneous constants used in assembler code. */
	.set W,		4		// Machine word size.

/* Offsets in struct proc. They MUST match proc.h. */
	.set P_STACKBASE,	0
	.set GSREG,	P_STACKBASE
	.set FSREG,	GSREG + 2	// 386 introduces FS and GS segments
	.set ESREG,	FSREG + 2
	.set DSREG,	ESREG + 2
	.set DIREG,	DSREG + 2
	.set SIREG,	DIREG + W
	.set BPREG,	SIREG + W
	.set STREG,	BPREG + W	// hole for another SP
	.set BXREG,	STREG + W
	.set DXREG,	BXREG + W
	.set CXREG,	DXREG + W
	.set AXREG,	CXREG + W
	.set RETADR,	AXREG + W	// return address for save() call
	.set PCREG,	RETADR + W
	.set CSREG,	PCREG + W
	.set PSWREG,	CSREG + W
	.set SPREG,	PSWREG + W
	.set SSREG,	SPREG + W
	.set P_STACKTOP,SSREG + W
	.set P_LDT_SEL,	P_STACKTOP
	.set P_CR3,	P_LDT_SEL + W
	.set P_LDT,	P_CR3 + W
	.set Msize,	9		// size of a message in 32-bit words
