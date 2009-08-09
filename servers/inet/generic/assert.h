/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
assert.h

Copyright 1995 Philip Homburg
*/
#ifndef INET_ASSERT_H
#define INET_ASSERT_H

#if !NDEBUG

void bad_assertion(char *file, int line, char *what) __noreturn;
void bad_compare(char *file, int line, int lhs, char *what, int rhs) __noreturn;

#define assert(x)	((void)(!(x) ? bad_assertion(this_file, __LINE__, \
			#x),0 : 0))
#define compare(a,t,b)	(!((a) t (b)) ? bad_compare(this_file, __LINE__, \
				(a), #a " " #t " " #b, (b)) : (void) 0)

#else /* NDEBUG */

#define assert(x)		0
#define compare(a,t,b)		0

#endif /* NDEBUG */

#endif /* INET_ASSERT_H */
