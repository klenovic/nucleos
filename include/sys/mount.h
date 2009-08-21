/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Copyright (c) 2009 Vrije Universiteit, Amsterdam.
 * See the copyright notice in file LICENSE.minix3.
 */
/* <sys/mount.h>
 * definitions for mount(2) 
 */

#ifndef _MOUNT_H
#define _MOUNT_H

#define MS_RDONLY	0x001	/* Mount device read only */
#define MS_REUSE	0x002	/* Tell RS to try reusing binary from memory */

int mount(char *_spec, char *_name, int _mountflags, char *type, char *args);
int umount(const char *_name);

#endif /* _MOUNT_H */
