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
setgroups.c
*/

#include <nucleos/errno.h>
#include <nucleos/unistd.h>
#include <nucleos/string.h>
#include <grp.h>

int setgroups(int ngroups, const gid_t *gidset)
{
	if(ngroups > 1) {
		/* Supplementary groups not implemented */
		errno= EINVAL;
		return -1;
	}

	if(ngroups == 1)
		return setgid(gidset[0]);

	return 0;
}

int initgroups(const char *name, gid_t basegid)
{
	struct group *gr;
	int r, found = 0;
	if((r = setgid(basegid)) < 0)
		return r;

	setgrent();
	while (!found && (gr = getgrent()) != NULL) {
		char **mem;
		for(mem = gr->gr_mem; mem && *mem; mem++) {
			if(!strcmp(name, *mem)) {
				found = 1;
				break;
			}
		}
	}
	endgrent();

	/* Because supplemental groups aren't implemented, this call
	 * should fail if the user is in any supplemental groups.
	 */
	if(found) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

