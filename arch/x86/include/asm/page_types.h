#ifndef _ASM_X86_PAGE_TYPES_H
#define _ASM_X86_PAGE_TYPES_H

#include <nucleos/const.h>
#include <nucleos/types.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define PAGE_OFFSET	((unsigned long)__PAGE_OFFSET)

#ifdef CONFIG_X86_64
#include <asm/page_64_types.h>
#else
#include <asm/page_32_types.h>
#endif	/* CONFIG_X86_64 */

#endif	/* _ASM_X86_PAGE_TYPES_H */
