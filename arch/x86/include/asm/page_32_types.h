#ifndef _ASM_X86_PAGE_32_TYPES_H
#define _ASM_X86_PAGE_32_TYPES_H

#include <nucleos/const.h>

/*
 * This handles the memory map.
 *
 * A __PAGE_OFFSET of 0xC0000000 means that the kernel has
 * a virtual address space of one gigabyte, which limits the
 * amount of physical memory you can use to about 950MB.
 *
 * If you want more physical memory than this then see the CONFIG_HIGHMEM4G
 * and CONFIG_HIGHMEM64G options in the kernel configuration.
 */
#define __PAGE_OFFSET	_AC(CONFIG_PAGE_OFFSET, UL)

/*
 * Kernel image size is limited to 512 MB (see in arch/x86/kernel/head_32.S)
 */
#define KERNEL_IMAGE_SIZE	(512 * 1024 * 1024)

#endif /* _ASM_X86_PAGE_32_TYPES_H */
