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
strings.h
*/

/* Open Group Base Specifications Issue 6 (not complete) */
_PROTOTYPE( char *index, (const char *_s, int _charwanted)		);
_PROTOTYPE( int strcasecmp, (const char *_s1, const char *_s2)		);
_PROTOTYPE( int strncasecmp, (const char *_s1, const char *_s2,
							size_t _len)	);
_PROTOTYPE( int ffs, (int i)						);
