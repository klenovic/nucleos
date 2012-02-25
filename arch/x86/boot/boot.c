/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include <nucleos/string.h>
#include <nucleos/minlib.h>
#include <nucleos/param.h>
#include <asm/bootparam.h>
#include "boot.h"

struct boot_params boot_params;

void get_parameters(void)
{
	int processor;
	int vid;

	/* HZ */
	boot_params.nucleos_kludge.system_hz = HZ;

	/* processor */
	processor = getprocessor();

	if (processor == 1586)
		processor = 686;

	boot_params.nucleos_kludge.processor = processor;

	/* bus */
	if (get_bus())
		boot_params.nucleos_kludge.pc_at = 1;

	/* video */
	vid = get_video();
	if (vid == 2 || vid == 3)
		boot_params.nucleos_kludge.vdu_ega = 1;
	else
		boot_params.nucleos_kludge.vdu_vga = 1;

	/* memory */
	memcpy(boot_params.nucleos_kludge.mem, mem, sizeof(boot_params.nucleos_kludge.mem));

	return;
}

void boot(void)
{
	memset(&boot_params, 0, sizeof(boot_params));

	/* Get environment variables from the parameter sector. */
	get_parameters();

	if (boot_nucleos() < 0)
		printf("Error while booting kernel\n");

	/* @nucleos: only in case of error */
	while (1) halt_cpu();
}
