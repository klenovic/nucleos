/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	minix/u64.h					Author: Kees J. Bot
 *								7 Dec 1995
 * Functions to manipulate 64 bit disk addresses.
 */
#ifndef __NUCLEOS_U64_H
#define __NUCLEOS_U64_H

#include <nucleos/types.h>

u64_t add64(u64_t i, u64_t j);
u64_t add64u(u64_t i, unsigned j);
u64_t add64ul(u64_t i, unsigned long j);
u64_t sub64(u64_t i, u64_t j);
u64_t sub64u(u64_t i, unsigned j);
u64_t sub64ul(u64_t i, unsigned long j);
unsigned diff64(u64_t i, u64_t j);
u64_t cvu64(unsigned i);
u64_t cvul64(unsigned long i);
unsigned cv64u(u64_t i);
unsigned long cv64ul(u64_t i);
unsigned long div64u(u64_t i, unsigned j);
unsigned rem64u(u64_t i, unsigned j);
u64_t mul64u(unsigned long i, unsigned j);
int cmp64(u64_t i, u64_t j);
int cmp64u(u64_t i, unsigned j);
int cmp64ul(u64_t i, unsigned long j);
unsigned long ex64lo(u64_t i);
unsigned long ex64hi(u64_t i);
u64_t make64(unsigned long lo, unsigned long hi);

#endif /* __NUCLEOS_U64_H */
