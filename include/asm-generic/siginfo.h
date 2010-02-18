#ifndef __ASM_GENERIC_SIGINFO_H
#define __ASM_GENERIC_SIGINFO_H

/* @nucleos: Change according to linux, introduce siginfo_t.  */

#include <nucleos/compiler.h>
#include <nucleos/types.h>

/*
 * SIGFPE si_codes
 */
#define FPE_INTDIV	1	/* integer divide by zero */
#define FPE_INTOVF	2	/* integer overflow */
#define FPE_FLTDIV	3	/* floating point divide by zero */
#define FPE_FLTOVF	4	/* floating point overflow */
#define FPE_FLTUND	5	/* floating point underflow */
#define FPE_FLTRES	6	/* floating point inexact result */
#define FPE_FLTINV	7	/* floating point invalid operation */
#define FPE_FLTSUB	8	/* subscript out of range */
#define NSIGFPE		8

#endif /* __ASM_GENERIC_SIGINFO_H */
