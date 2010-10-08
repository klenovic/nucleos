/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains a collection of miscellaneous procedures:
 *   kernel_panic: abort Nucleos due to a fatal error
 */
#include <nucleos/unistd.h>
#include <nucleos/signal.h>
#include <nucleos/string.h>
#include <nucleos/com.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>

void kernel_panic(char *mess, int nr)
{
	/* The system has run aground of a fatal kernel error.
	   Terminate execution. */
	if (kernel_in_panic++)
		arch_monitor();

	if (mess != NULL) {
		printk("kernel panic: %s", mess);

		if(nr != NO_NUM)
			printk(" %d", nr);

		printk("\n");
	}

	printk("kernel: ");
	util_stacktrace();

	/* Abort Nucleos. */
	nucleos_shutdown(NULL);
}
