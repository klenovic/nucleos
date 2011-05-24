/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* First C file used by the kernel. */
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/utsrelease.h>
#include <nucleos/version.h>
#include <nucleos/compile.h>
#include <nucleos/param.h>
#include <kernel/proto.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>
#include <asm/bootparam.h>
#include <asm/kernel/const.h>

/**
 * @brief Get environment value
 * @param params  pointer to boot monitor parameters
 * @param name  key to look up (string)
 * @return pointer to value (constant string) on succes otherwise 0
 */
static char *get_value(const char *params, const char *name)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
	register const char *namep;
	register char *envp;

	for (envp = (char *) params; *envp != 0;) {
		for (namep = name; *namep != 0 && *namep == *envp; namep++, envp++);

		if (*namep == '\0' && *envp == '=')
			return(envp + 1);

		while (*envp++ != 0);
	}

	return 0;
}

/**
 * @brief Perform system initializations prior to calling main().
 * @param cs  kernel code segment
 * @param ds  kernel data segment
 * @param mds  monitor data segment
 * @param parmoff  boot parameters offset
 * @param parmsize  boot parameters length
 */
void prepare_kernel(u16 cs, u16 ds, u16 mds, u16 parmoff, u16 parmsize)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by loader.
 */
	register char *value;				/* value in key=value pair */
	int h;
	extern char __text, __etext;
	extern char __data, __end;

	/* Record where the kernel and the monitor are. */
	kinfo.code_base = seg2phys(cs);
	kinfo.code_size = (phys_bytes)((char*)&__etext - (char*)&__text); /* size of code segment */
	kinfo.data_base = seg2phys(ds);
	kinfo.data_size = (phys_bytes)((char*)&__end - (char*)&__data); /* size of data segment */

	/* protection initialization. */
	prot_init();

	/* Copy the boot parameters to the local buffer. */
	arch_get_params(params_buffer, sizeof(params_buffer));

	/* Record miscellaneous information for user-space servers. */
	kinfo.nr_procs = NR_PROCS;
	kinfo.nr_tasks = NR_TASKS;

	strncpy(kinfo.release, UTS_RELEASE, sizeof(kinfo.release));

	kinfo.release[sizeof(kinfo.release)-1] = '\0';

	strncpy(kinfo.version, UTS_VERSION, sizeof(kinfo.version));

	kinfo.version[sizeof(kinfo.version)-1] = '\0';
	kinfo.proc_addr = (vir_bytes) proc;

	/* Load average data initialization. */
	kloadinfo.proc_last_slot = 0;

	for(h = 0; h < _LOAD_HISTORY; h++)
		kloadinfo.proc_load_history[h] = 0;

	/* Processor? Decide if mode is protected for older machines. */
	machine.processor=atoi(get_value(params_buffer, "processor")); 

	/* XT, AT or MCA bus? */
	value = get_value(params_buffer, "bus");
	if (value == NIL_PTR || strcmp(value, "at") == 0) {
		machine.pc_at = TRUE;			/* PC-AT compatible hardware */
	} else if (strcmp(value, "mca") == 0) {
		machine.pc_at = machine.ps_mca = TRUE;	/* PS/2 with micro channel */
	}

	/* Type of VDU: */
	value = get_value(params_buffer, "video");	/* EGA or VGA video unit */

	if (strcmp(value, "ega") == 0)
		machine.vdu_ega = TRUE;

	if (strcmp(value, "vga") == 0)
		machine.vdu_vga = machine.vdu_ega = TRUE;

	/* Get clock tick frequency. */
	value = get_value(params_buffer, "hz");

	if(value)
		system_hz = atoi(value);

	/* sanity check */
	if(!value || system_hz < 2 || system_hz > 50000) {
		printk("Wrong value of HZ=0x%x, using default HZ=0x%x\n", system_hz, HZ);
		system_hz = HZ;
	}

	value = get_value(params_buffer, SERVARNAME);

	if(value && atoi(value) == 0)
		do_serial_debug=1;

#ifdef CONFIG_X86_LOCAL_APIC
	value = get_value(params_buffer, "no_apic");
	if(value)
		config_no_apic = atoi(value);
	else
		config_no_apic = 0;
#endif

#ifndef CONFIG_BUILTIN_INITRD
	/* Initial ramdisk */
	value = get_value(params_buffer, "initrdbase");

	if(value)
		boot_params.hdr.ramdisk_image = atoi(value);

	value = get_value(params_buffer, "initrdsize");

	if(value)
		boot_params.hdr.ramdisk_size = atoi(value);
#endif
	/* Return to assembler code to switch to protected mode (if 286), 
	 * reload selectors and call main().
	 */
	intr_init(INTS_NUCLEOS, 0);
}
