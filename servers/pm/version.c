/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/compile.h>
#include <nucleos/uts.h>
#include <nucleos/types.h>
#include <nucleos/utsname.h>
#include <nucleos/utsrelease.h>
#include <nucleos/version.h>

#ifndef CONFIG_KALLSYMS
#define version(a) Version_ ## a
#define version_string(a) version(a)

extern int version_string(NUCLEOS_VERSION_CODE);
int version_string(NUCLEOS_VERSION_CODE);
#endif

struct utsname uts_val = {
	.sysname	= UTS_SYSNAME,
	.nodename	= UTS_NODENAME,
	.release	= UTS_RELEASE,
	.version	= UTS_VERSION,
	.machine	= UTS_MACHINE,
	.domainname	= UTS_DOMAINNAME,
};

/* FIXED STRINGS! Don't touch! */
const char nucleos_banner[] =
	"Nucleos version " UTS_RELEASE " (" NUCLEOS_COMPILE_BY "@"
	NUCLEOS_COMPILE_HOST ") (" NUCLEOS_COMPILER ") " UTS_VERSION "\n";

const char nucleos_proc_banner[] =
	"%s version %s"
	" (" NUCLEOS_COMPILE_BY "@" NUCLEOS_COMPILE_HOST ")"
	" (" NUCLEOS_COMPILER ") %s\n";
