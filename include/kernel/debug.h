#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

#include <nucleos/debug.h>

/* Runtime sanity checking. */
#ifdef CONFIG_DEBUG_KERNEL_TRACE

#define VF_SCHEDULING		(1L << 1)
#define VF_PICKPROC		(1L << 2)

#define TRACE(code, statement)				\
	if (verboseflags & code) {			\
		printk("%s:%d: ", __FILE__, __LINE__);	\
		statement				\
	}

#else
#define TRACE(code, statement)
#endif /* CONFIG_DEBUG_KERNEL_TRACE */

#ifdef CONFIG_DEBUG_KERNEL_VMASSERT
#define vmassert(t) { \
	if (!(t)) { kernel_panic("vm: assert " #t " failed in " __FILE__, __LINE__); } }
#else
#define vmassert(t)
#endif /* CONFIG_DEBUG_KERNEL_VMASSERT */

#define NOT_REACHABLE(__x) do {						\
	printk("NOT_REACHABLE at %s:%d\n", __FILE__, __LINE__);	\
	kernel_panic("execution at an unexpected location\n", NO_NUM);	\
	for(;;);							\
} while(0)

#define NOT_IMPLEMENTED do {	\
		printk("NOT_IMPLEMENTED at %s:%d\n", __FILE__, __LINE__); \
		kernel_panic("NOT_IMPLEMENTED", NO_NUM); \
} while(0)

#ifdef CONFIG_BOOT_VERBOSE
#define BOOT_VERBOSE(x)	x
#else
#define BOOT_VERBOSE(x)
#endif

#endif /* __KERNEL_DEBUG_H */
