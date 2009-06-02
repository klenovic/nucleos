/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <stdarg.h> header is ANSI's way to handle variable numbers of params.
 * Some programming languages require a function that is declared with n
 * parameters to be called with n parameters.  C does not.  A function may
 * called with more parameters than it is declared with.  The well-known
 * printf function, for example, may have arbitrarily many parameters.
 * The question arises how one can access all the parameters in a portable
 * way.  The C standard defines three macros that programs can use to
 * advance through the parameter list.  The definition of these macros for
 * MINIX are given in this file.  The three macros are:
 *
 *	va_start(ap, parmN)	prepare to access parameters
 *	va_arg(ap, type)	get next parameter value and type
 *	va_end(ap)		access is finished
 *
 * Ken Thompson's famous line from V6 UNIX is equally applicable to this file:
 *
 *	"You are not expected to understand this"
 *
 */

#ifndef _STDARG_H
#define _STDARG_H

#ifdef __GNUC__
/**
 * @brief Variable argument list type.
 */
typedef __builtin_va_list va_list;

/**
 * @brief Initializes \a ap for subsequent use by va_arg() and va_end(), and must be called first.
 * The parameter \a last is the name of the last parameter before the variable argument list.
 */
#define va_start(ap, last) __builtin_va_start((ap), (last))

/**
 * @brief Expands to an expression that has the type and value of the next argument in the call.
 * The  parameter  \a type  is  a  type  name  specified so that the type of a pointer to an object
 * that has the specified type can be obtained simply by adding a * to \a type.
 */
#define va_arg(ap, type) __builtin_va_arg((ap), type)

/**
 * @brief After the call va_end(ap)  the variable \a ap is undefined.
 */
#define va_end(ap) __builtin_va_end(ap)

/**
 * @brief Variable argument copy.
 */
#define va_copy(dest, src) __builtin_va_copy((dest), (src))
#endif /* __GNUC_ */

#endif /* _STDARG_H */
