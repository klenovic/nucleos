/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_MINLIB_H
#define __NUCLEOS_MINLIB_H

/* Miscellaneous arch-dependent Nucleos. */
#include <asm/minlib.h>

/* Miscellaneous BSD. */
void swab(char *_from, char *_to, int _count);
char *itoa(int _n);
char *getpass(const char *_prompt);

/* Miscellaneous MINIX. */
void std_err(char *_s);
void prints(const char *_s, ...);
int fsversion(char *_dev, char *_prog);
int getprocessor(void);
int load_mtab(char *_prog_name);
int rewrite_mtab(char *_prog_name);
int get_mtab_entry(char *_s1, char *_s2, char *_s3, char *_s4);
int put_mtab_entry(char *_s1, char *_s2, char *_s3, char *_s4);

#endif /* __NUCLEOS_MINLIB_H */
