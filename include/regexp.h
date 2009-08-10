/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <regexp.h> header is used by the (V8-compatible) regexp(3) routines. */
/* NOTE: Obsoleted by the POSIX regex(3) library. */

#ifndef _REGEXP_H
#define _REGEXP_H

#define CHARBITS 0377
#define NSUBEXP  10
typedef struct regexp {
	const char *startp[NSUBEXP];
	const char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexp;

/* Keep these functions away from the POSIX versions. */
#define regcomp _v8_regcomp
#define regexec _v8_regexec
#define regsub _v8_regsub
#define regerror _v8_regerror

/* Function Prototypes. */
regexp *regcomp(const char *_exp);
int regexec(regexp *_prog, const char *_string, int _bolflag);
void regsub(regexp *_prog, char *_source, char *_dest);
void regerror(const char *_message) ;

#endif /* _REGEXP_H */
