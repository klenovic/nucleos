/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*      boot.h - Info between different parts of boot.  Author: Kees J. Bot
 */
#ifndef __NUCS_EDPARAMS_H
#define __NUCS_EDPARAMS_H

/* Constants describing the metal: */
#define SECTOR_SIZE      512
#define PARAMSEC           1    /* Sector containing boot parameters. */
#define ESC             '\33'   /* Escape key. */
#define MSEC_PER_TICK   1000    /* Clock does 18.2 ticks per second. */
#define TICKS_PER_DAY  86400L   /* Doesn't wrap, but that doesn't matter. */

typedef struct {                /* One chunk of free memory. */
        MNX(u32_t)      base;   /* Start byte. */
        MNX(u32_t)      size;   /* Number of bytes. */
} memory;

/* Sticky attributes. */
#define E_SPECIAL       0x01    /* These are known to the program. */
#define E_DEV           0x02    /* The value is a device name. */
#define E_RESERVED      0x04    /* May not be set by user, e.g. 'boot' */
#define E_STICKY        0x07    /* Don't go once set. */

/* Volatile attributes. */
#define E_VAR           0x08    /* Variable */
#define E_FUNCTION      0x10    /* Function definition. */

/* Variables, functions, and commands. */
typedef struct environment {
        struct environment *next;
        char    flags;
        char    *name;          /* name = value */
        char    *arg;           /* name(arg) {value} */
        char    *value;
        char    *defval;        /* Safehouse for default values. */
} environment;

extern environment *env;        /* Lists the environment. */

char *b_value(char *name);      /* Get/set the value of a variable. */
int b_setvar(int flags, char *name, char *value);

extern MNX(u32_t) lowsec;       /* Offset to the file system on the boot device. */
#endif /* !__NUCS_EDPARAMS_H */
