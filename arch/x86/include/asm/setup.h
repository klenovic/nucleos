#ifndef _ASM_X86_SETUP_H
#define _ASM_X86_SETUP_H

#if defined(__KERNEL__) || defined(__UKERNEL__)

#define COMMAND_LINE_SIZE	2048

#define PARAM_SIZE 4096		/* sizeof(struct boot_params) */

#define OLD_CL_MAGIC		0xA33F
#define OLD_CL_ADDRESS		0x020	/* Relative to real mode data */
#define NEW_CL_POINTER		0x228	/* Relative to real mode data */

#endif  /* __KERNEL__ || __UKERNEL__ */

#endif /* _ASM_X86_SETUP_H */
