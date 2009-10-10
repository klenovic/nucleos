#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

#include <nucleos/debug.h>

/* Runtime sanity checking. */
#ifdef CONFIG_DEBUG_KERNEL_TRACE

#define VF_SCHEDULING		(1L << 1)
#define VF_PICKPROC		(1L << 2)

#define TRACE(code, statement)				\
	if (verboseflags & code) {			\
		printf("%s:%d: ", __FILE__, __LINE__);	\
		statement				\
	}

#else
#define TRACE(code, statement)
#endif /* CONFIG_DEBUG_KERNEL_TRACE */


#define ENTERED		0xBA5E1514
#define NOTENTERED	0x1415BEE1

#define NOREC_ENTER(varname) 					\
	static int varname = NOTENTERED;			\
	int mustunlock = 0; 					\
	if(!intr_disabled()) { lock; mustunlock = 1; }		\
	vmassert(varname == ENTERED || varname == NOTENTERED);	\
	vmassert(magictest == MAGICTEST);			\
	vmassert(varname != ENTERED);				\
	varname = ENTERED;

#define NOREC_RETURN(varname, v) do {				\
	vmassert(intr_disabled());				\
	vmassert(magictest == MAGICTEST);			\
	vmassert(varname == ENTERED || varname == NOTENTERED);	\
	varname = NOTENTERED;					\
	if(mustunlock)	{ unlock;	} 			\
	return v;						\
} while(0)

#ifdef CONFIG_DEBUG_KERNEL_VMASSERT
#define vmassert(t) { \
	if (!(t)) { minix_panic("vm: assert " #t " failed\n", __LINE__); } }
#else
#define vmassert(t)
#endif /* CONFIG_DEBUG_KERNEL_VMASSERT */

#define NOT_REACHABLE(__x) do {						\
	kprintf("NOT_REACHABLE at %s:%d\n", __FILE__, __LINE__);	\
	minix_panic("execution at an unexpected location\n", NO_NUM);	\
	for(;;);							\
} while(0)

#endif /* __KERNEL_DEBUG_H */
