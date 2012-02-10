/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
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

extern struct setup_header hdr;
extern void arch_copy_cmdline_params(char *cmdline_buf, struct boot_params *params);

/**
 * @brief Perform system initializations prior to calling main().
 * @param cs  kernel code segment
 * @param ds  kernel data segment
 * @param parmoff  boot parameters offset
 * @param parmsize  boot parameters length
 */
void prepare_kernel(u16 cs, u16 ds)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by loader.
 */
	register char *value;	/* value in key=value pair */
	int h;
	extern char _text, _etext;
	extern char _data, _end;

	/* Record where the kernel and the monitor are. */
	kinfo.code_base = seg2phys(cs);
	/* size of code segment (includes also the boot code) */
	kinfo.code_size = (phys_bytes)((char*)&_etext - (char*)&_text);
	kinfo.data_base = seg2phys(ds);
	/* size of data segment */
	kinfo.data_size = (phys_bytes)((char*)&_end - (char*)&_data);

	/* protection initialization. */
	prot_init();

	/* copy header from kernel image into boot params */
	memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

	/* Copy command line params into the local buffer. */
	arch_copy_cmdline_params(cmd_line_params, &boot_params);

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
	machine.processor=atoi(get_value(cmd_line_params, "processor"));

	/* XT, AT or MCA bus? */
	value = get_value(cmd_line_params, "bus");
	if (value == NIL_PTR || strcmp(value, "at") == 0) {
		machine.pc_at = TRUE;			/* PC-AT compatible hardware */
	} else if (strcmp(value, "mca") == 0) {
		machine.pc_at = machine.ps_mca = TRUE;	/* PS/2 with micro channel */
	}

	/* Type of VDU: */
	value = get_value(cmd_line_params, "video");	/* EGA or VGA video unit */

	if (strcmp(value, "ega") == 0)
		machine.vdu_ega = TRUE;

	if (strcmp(value, "vga") == 0)
		machine.vdu_vga = machine.vdu_ega = TRUE;

	/* Get clock tick frequency. */
	value = get_value(cmd_line_params, "hz");

	if(value)
		system_hz = atoi(value);

	/* sanity check */
	if(!value || system_hz < 2 || system_hz > 50000) {
		printk("Wrong value of HZ=0x%x, using default HZ=0x%x\n", system_hz, HZ);
		system_hz = HZ;
	}

	value = get_value(cmd_line_params, SERVARNAME);

	if(value && atoi(value) == 0)
		do_serial_debug=1;

#ifdef CONFIG_X86_LOCAL_APIC
	value = get_value(cmd_line_params, "no_apic");
	if(value)
		config_no_apic = atoi(value);
	else
		config_no_apic = 0;
#endif

#ifndef CONFIG_BUILTIN_INITRD
	/* Initial ramdisk */
	value = get_value(cmd_line_params, "initrdbase");

	if(value)
		boot_params.hdr.ramdisk_image = atoi(value);

	value = get_value(cmd_line_params, "initrdsize");

	if(value)
		boot_params.hdr.ramdisk_size = atoi(value);
#endif

	value = get_value(cmd_line_params, "aout_hdrs_addr");
	if (value)
		__kimage_aout_headers = (u32)atoi(value);
	else
		kernel_panic("AOUT headerrs address is not set", NO_NUM);

	/* Return to assembler code reload selectors and call main(). */
	intr_init(INTS_NUCLEOS, 0);
}
