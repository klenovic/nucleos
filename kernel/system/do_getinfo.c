/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_GETINFO
 *
 * The parameters for this kernel call are:
 *    m_data3:	I_REQUEST	(what info to get)	
 *    m_data4:	I_VAL_PTR 	(where to put it)	
 *    m_data1:	I_VAL_LEN 	(maximum length expected, optional)	
 *    m_data5:	I_VAL_PTR2	(second, optional pointer)	
 *    m_data2:	I_VAL_LEN2_E	(second length or process nr)	
 */
#include <nucleos/string.h>
#include <nucleos/endpoint.h>
#include <kernel/system.h>
#include <kernel/vm.h>
#include <asm/bootparam.h>

#if USE_GETINFO

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
int do_getinfo(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to request message */
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
	size_t length = 0;
	vir_bytes src_vir; 
	int proc_nr, nr_e, nr, r;
	struct proc *caller;
	int wipe_rnd_bin = -1;
	struct exec e_hdr;
	int len = 0;
	static struct k_randomness copy;	/* copy to keep counters */
	int i = 0;
	int bin = 0;

	caller = proc_addr(who_p);

	/* Set source address and length based on request type. */
	switch (m_ptr->I_REQUEST) {
	case GET_MACHINE:
		length = sizeof(struct machine);
		src_vir = (vir_bytes) &machine;
		break;

	case GET_KINFO:
		length = sizeof(struct kinfo);
		src_vir = (vir_bytes) &kinfo;
		break;

	case GET_LOADINFO:
		length = sizeof(struct loadinfo);
		src_vir = (vir_bytes) &kloadinfo;
		break;

	case GET_HZ:
		length = sizeof(system_hz);
		src_vir = (vir_bytes) &system_hz;
		break;

	case GET_IMAGE:
		length = sizeof(struct boot_image) * NR_BOOT_PROCS;
		src_vir = (vir_bytes) image;
		break;

	case GET_IRQHOOKS:
		length = sizeof(struct irq_hook) * NR_IRQ_HOOKS;
		src_vir = (vir_bytes) irq_hooks;
		break;

	case GET_PROCTAB:
		length = sizeof(struct proc) * (NR_PROCS + NR_TASKS);
		src_vir = (vir_bytes) proc;
		break;

	case GET_PRIVTAB:
		length = sizeof(struct priv) * (NR_SYS_PROCS);
		src_vir = (vir_bytes) priv;
		break;

	case GET_PROC:
		nr_e = (m_ptr->I_VAL_LEN2_E == ENDPT_SELF) ?
		who_e : m_ptr->I_VAL_LEN2_E;

		if(!isokendpt(nr_e, &nr))
			return -EINVAL; /* validate request */

		length = sizeof(struct proc);
		src_vir = (vir_bytes) proc_addr(nr);
		break;

	case GET_PRIV: {
		nr_e = (m_ptr->I_VAL_LEN2_E == ENDPT_SELF) ?
		who_e : m_ptr->I_VAL_LEN2_E;
		if(!isokendpt(nr_e, &nr)) return -EINVAL; /* validate request */
		length = sizeof(struct priv);
		src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
		break;
	}

	case GET_MONPARAMS:
		src_vir = (vir_bytes) params_buffer;
		length = sizeof(params_buffer);
		break;

	case GET_RANDOMNESS:

		copy = krandom;
		for (i= 0; i<RANDOM_SOURCES; i++) {
			krandom.bin[i].r_size = 0;	/* invalidate random data */
			krandom.bin[i].r_next = 0;
		}
		length = sizeof(copy);
		src_vir = (vir_bytes) &copy;
		break;

	case GET_RANDOMNESS_BIN:
		bin = m_ptr->I_VAL_LEN2_E;

		if(bin < 0 || bin >= RANDOM_SOURCES) {
			printk("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
			return -EINVAL;
		}

		if(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
			return -ENOENT;

		length = sizeof(krandom.bin[bin]);
	 	src_vir = (vir_bytes) &krandom.bin[bin];
		wipe_rnd_bin = bin;
		break;

	case GET_KMESSAGES:
		length = sizeof(struct kmessages);
		src_vir = (vir_bytes) &kmess;
		break;

#ifdef CONFIG_DEBUG_KERNEL_TIME_LOCKS
	case GET_LOCKTIMING:
		length = sizeof(timingdata);
		src_vir = (vir_bytes) timingdata;
		break;
#endif
	case GET_IRQACTIDS:
		length = sizeof(irq_actids);
		src_vir = (vir_bytes) irq_actids;
		break;

	case GET_IDLETSC:
#ifdef CONFIG_IDLE_TSC
		length = sizeof(idle_tsc);
		src_vir = (vir_bytes) &idle_tsc;
		break;
#else
		printk("do_getinfo: kernel not compiled with CONFIG_IDLE_TSC\n");
		return(-EINVAL);
#endif

	case GET_BOOTPARAM:
		length = sizeof(struct boot_param);
		src_vir = (vir_bytes) &boot_param;
		break;

	case GET_AOUTHEADER: {
		int hdrindex, index = m_ptr->I_VAL_LEN2_E;
		if(index < 0 || index >= NR_BOOT_PROCS) {
			return -EINVAL;
		}
		if (iskerneln(_ENDPOINT_P(image[index].endpoint))) {
			hdrindex = 0;
		} else {
			hdrindex = 1 + index-NR_TASKS;
		}
		arch_get_aout_headers(hdrindex, &e_hdr);
		length = sizeof(e_hdr);
		src_vir = (vir_bytes) &e_hdr;
		break;
	}

	default:
		printk("do_getinfo: invalid request %d\n", m_ptr->I_REQUEST);
		return(-EINVAL);
	}

	/* Try to make the actual copy for the requested data. */
	r = data_copy_vmcheck(SYSTEM, src_vir, who_e, (vir_bytes) m_ptr->I_VAL_PTR, length);

	if(r != 0) return r;

	if(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

	return 0;
}

#endif /* USE_GETINFO */
