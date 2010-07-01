/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/string.h> header contains prototypes for the string handling 
 * functions.
 */
#ifndef __NUCLEOS_STRING_H
#define __NUCLEOS_STRING_H

#if !defined(__KERNEL__) && !defined(__UKERNEL__)

#include <string.h>

#else

#include <nucleos/compiler.h>
#include <nucleos/types.h>
#include <nucleos/stddef.h>
#include <stdarg.h>

/* Function Prototypes. */
void *memchr(const void *_s, int _c, __kernel_size_t _n);
int memcmp(const void *_s1, const void *_s2, __kernel_size_t _n);
void *memcpy(void *_s1, const void *_s2, __kernel_size_t _n);
void *memmove(void *_s1, const void *_s2, __kernel_size_t _n);
void *memset(void *_s, int _c, __kernel_size_t _n);
char *strcat(char *_s1, const char *_s2);
char *strchr(const char *_s, int _c);
int strncmp(const char *_s1, const char *_s2, __kernel_size_t _n);
int strcmp(const char *_s1, const char *_s2);
int strcoll(const char *_s1, const char *_s2);
char *strcpy(char *_s1, const char *_s2);
__kernel_size_t strcspn(const char *_s1, const char *_s2);
char *strerror(int _errnum);
__kernel_size_t strlen(const char *_s);
char *strncat(char *_s1, const char *_s2, __kernel_size_t _n);
char *strncpy(char *_s1, const char *_s2, __kernel_size_t _n);
char *strpbrk(const char *_s1, const char *_s2);
char *strrchr(const char *_s, int _c);

/* Misc. extra functions */
int strcasecmp(const char *_s1, const char *_s2);
int strncasecmp(const char *_s1, const char *_s2, __kernel_size_t _len);
__kernel_size_t strnlen(const char *_s, __kernel_size_t _n);

#endif /*  defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_STRING_H */
