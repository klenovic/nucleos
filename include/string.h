/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <string.h> header contains prototypes for the string handling 
 * functions.
 */

#ifndef _STRING_H
#define _STRING_H

#define NULL    ((void *)0)

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;	/* type returned by sizeof */
#endif /*_SIZE_T */

/* Function Prototypes. */
void *memchr(const void *_s, int _c, size_t _n);
int memcmp(const void *_s1, const void *_s2, size_t _n);
void *memcpy(void *_s1, const void *_s2, size_t _n);
void *memmove(void *_s1, const void *_s2, size_t _n);
void *memset(void *_s, int _c, size_t _n);
char *strcat(char *_s1, const char *_s2);
char *strchr(const char *_s, int _c);
int strncmp(const char *_s1, const char *_s2, size_t _n);
int strcmp(const char *_s1, const char *_s2);
int strcoll(const char *_s1, const char *_s2);
char *strcpy(char *_s1, const char *_s2);
size_t strcspn(const char *_s1, const char *_s2);
char *strerror(int _errnum);
size_t strlen(const char *_s);
char *strncat(char *_s1, const char *_s2, size_t _n);
char *strncpy(char *_s1, const char *_s2, size_t _n);
char *strpbrk(const char *_s1, const char *_s2);
char *strrchr(const char *_s, int _c);
size_t strspn(const char *_s1, const char *_s2);
char *strstr(const char *_s1, const char *_s2);
char *strtok(char *_s1, const char *_s2);
size_t strxfrm(char *_s1, const char *_s2, size_t _n);

/* Open Group Base Specifications Issue 6 (not complete) */
char *strdup(const char *_s1);

/* For backward compatibility. */
char *index(const char *_s, int _charwanted);
char *rindex(const char *_s, int _charwanted);
void bcopy(const void *_src, void *_dst, size_t _length);
int bcmp(const void *_s1, const void *_s2, size_t _length);
void bzero(void *_dst, size_t _length);
void *memccpy(char *_dst, const char *_src, int _ucharstop, size_t _size);

/* Misc. extra functions */
int strcasecmp(const char *_s1, const char *_s2);
int strncasecmp(const char *_s1, const char *_s2, size_t _len);
size_t strnlen(const char *_s, size_t _n);
size_t strlcat(char *_dst, const char *_src, size_t _siz);
size_t strlcpy(char *_dst, const char *_src, size_t _siz);

#endif /* _STRING_H */
