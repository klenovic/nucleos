/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* @file include/servers/ds/ds.h
 * @brief Prototypes and definitions for DS interface.
 */
#ifndef __SERVERS_DS_DS_H
#define __SERVERS_DS_DS_H

#include <nucleos/types.h>

/* DS Flag values. */
#define DS_IN_USE       0x0001	/* Internal use only. */
#define DS_PUBLIC       0x0002	/* Publically retrievable value. */
#define DS_INITIAL      0x0004	/* On subscription, send initial contents. */

/* These type flags are mutually exclusive. Give as args to ds_subscribe. */
#define DS_TYPE_U32     0x0100
#define DS_TYPE_STR     0x0200
#define DS_TYPE_MASK    0xff00	/* All type info. */

/* DS constants. */
#define DS_MAX_KEYLEN 80        /* Max length for a key, including '\0'. */
#define DS_MAX_VALLEN 100	/* Max legnth for a str value, incl '\0'. */ 

/* ds.c */
int ds_subscribe(char *name_regexp, int type, int flags);

/* publish/update item */
int ds_publish_u32(char *name, u32_t val);
int ds_publish_str(char *name, char *val);

/* retrieve item by name + type */
int ds_retrieve_u32(char *name, u32_t *val);
int ds_retrieve_str(char *name, char *val, size_t len);

/* retrieve updates for item */
int ds_check_u32(char *n, size_t namelen, u32_t *val);
int ds_check_str(char *n, size_t namelen, char *v, size_t vlen);

/* lib/sysvipc/ds.c */
int mini_ds_retrieve_u32(char *name, u32_t *val);

#endif /* __SERVERS_DS_DS_H */
