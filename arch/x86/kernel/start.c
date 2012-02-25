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

extern struct setup_header hdr;

extern void arch_copy_boot_params(struct boot_params *params, u32 real_mode_params);
extern void arch_copy_cmdline_params(char *cmdline_buf, struct boot_params *params);
extern void arch_copy_aout_headers(struct boot_params *params);

/**
 * @brief Perform system initializations prior to calling main().
 * @param cs  kernel code segment
 * @param ds  kernel data segment
 * @param param  physical address of boot parameters
 */
void prepare_kernel(u16 cs, u16 ds, u32 real_mode_params)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by loader.
 */
	char value[64];	/* value in key=value pair */
	int h;
	int ret;
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

	/* copy boot params */
	arch_copy_boot_params(&boot_params, real_mode_params);

	/* copy aout headers */
	arch_copy_aout_headers(&boot_params);

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
	machine.processor = boot_params.nucleos_kludge.processor;
	machine.pc_at = boot_params.nucleos_kludge.pc_at;	/* PC-AT compatible hardware */

	/* Type of VDU: */
	machine.vdu_ega = boot_params.nucleos_kludge.vdu_ega;
	machine.vdu_vga = boot_params.nucleos_kludge.vdu_vga;

	/* Get clock tick frequency. */
	system_hz = boot_params.nucleos_kludge.system_hz;

	/* sanity check */
	if(system_hz < 2 || system_hz > 50000) {
		printk("Wrong value of HZ=0x%x, using default HZ=0x%x\n", system_hz, HZ);
		system_hz = HZ;
	}

	ret = get_param_value(cmd_line_params, SERVARNAME, value);

	if (ret && atoi(value) == 0)
		do_serial_debug=1;

#ifdef CONFIG_X86_LOCAL_APIC
	ret = get_param_value(cmd_line_params, "no_apic", value);
	if(ret)
		config_no_apic = atoi(value);
	else
		config_no_apic = 0;
#endif

	/* clear the buffer */
	memset(cmd_line_params_str, 0, COMMAND_LINE_SIZE);

	/* Return to assembler code reload selectors and call main(). */
	intr_init(INTS_NUCLEOS, 0);
}
