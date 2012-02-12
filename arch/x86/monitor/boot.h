/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ARCH_X86_BOOT_BOOT_H
#define __ARCH_X86_BOOT_BOOT_H

#ifndef DEBUG
#define DEBUG 0
#endif

/* Constants describing the metal: */

#define SECTOR_SIZE	512
#define SECTOR_SHIFT	9
#define RATIO(b)		((b) / SECTOR_SIZE)

#define PARAMSEC	1	/* Sector containing boot parameters. */

#define DSKBASE		0x1E	/* Floppy disk parameter vector. */
#define DSKPARSIZE	11	/* There are this many bytes of parameters. */

#define ESC		'\33'	/* Escape key. */

#define HEADERPOS      0x00600L /* Place for an array of struct exec's. */

#define FREEPOS	       0x08000L /* Memory from FREEPOS to caddr is free to
                                 * play with. */
#define MSEC_PER_TICK        55 /* Clock does 18.2 ticks per second. */
#define TICKS_PER_DAY 0x1800B0L /* After 24 hours it wraps. */

#define BOOTPOS	       0x07C00L	/* Bootstraps are loaded here. */
#define SIGNATURE	0xAA55	/* Proper bootstraps have this signature. */
#define SIGNATOFF	510	/* Offset within bootblock. */

/* BIOS video modes. */
#define MONO_MODE	0x07	/* 80x25 monochrome. */
#define COLOR_MODE	0x03	/* 80x25 color. */

typedef struct vector { /* 8086 vector */
  u16_t offset;
  u16_t segment;
} vector;

vector rem_part;		/* Boot partition table entry. */

u32_t caddr, daddr; /* Code and data address of the boot program. */
u32_t runsize;   /* Size of this program. */

u16_t device;    /* Drive being booted from. */

struct memory mem[NR_MEMS];	/* List of available memory. */

typedef struct bios_env
{
  u16_t ax;
  u16_t bx;
  u16_t cx;
  u16_t flags;
} bios_env_t;

#define FL_CARRY	0x0001	/* carry flag */

/* halt cpu */
void halt_cpu(void);

/* Local monitor address to absolute address. */
u32_t mon2abs(void *ptr);

/* Vector to absolute address. */
u32_t vec2abs(vector *vec);

/* Copy bytes from anywhere to anywhere (extended memory too) */
void raw_copy(u32 dstaddr, u32 srcaddr, u32 count);

#define MAX_GETWORD_ADDR 0xfffff

/* Get a word from somewhere in the first 1MB of memory */
u16_t get_word(u32_t addr);

/* Put a word anywhere. */
void put_word(u32_t addr, u16 word);

/* Switch to a copy of this program. */
void relocate(void);

/* Open device and determine params / close device. */
int dev_open(void), dev_close(void);

 /* True if sector is on a track boundary. */
int dev_boundary(u32_t sector);

/* Read 1 or more sectors from "device". */
int readsectors(u32_t bufaddr, u32_t sector, u8 count);

/* Send a character to the screen. */
void putch(int c);

/* Enable copying console I/O to a serial line. */
void serial_init(int line);

/* Set video mode */
void set_mode(unsigned mode);

/* clear the screen. */
void clear_screen(void);

/* System bus type, XT, AT, or MCA. */
u16_t get_bus(void);

/* Display type, MDA to VGA. */
u16_t get_video(void);

/* Current value of the clock tick counter. */
u32_t get_tick(void);

/* Execute a bootstrap routine for a different O.S. */
void bootstrap(int device, struct part_entry *entry);

/* Start Minix. */
void minix(u32 koff, u32 kcs, u32 kds, u32 boot_params);

void int15(bios_env_t *);

/* Shared between boot.c and bootimage.c: */

/* Sticky attributes. */
#define E_SPECIAL  0x01 /* These are known to the program. */
#define E_DEV      0x02 /* The value is a device name. */
#define E_RESERVED 0x04 /* May not be set by user, e.g. 'boot' */
#define E_STICKY   0x07 /* Don't go once set. */

/* Volatile attributes. */
#define E_VAR       0x08 /* Variable */
#define E_FUNCTION  0x10 /* Function definition. */

/* Variables, functions, and commands. */
typedef struct environment {
  struct environment *next;
  char flags;
  char *name;   /* name = value */
  char *arg;    /* name(arg) {value} */
  char *value;
  char *defval; /* Safehouse for default values. */
} environment;

environment *env;/* Lists the environment. */

/* Get/set the value of a variable. */
char *b_value(char *name);
int b_setvar(int flags, char *name, char *value);
void b_unset(char *name);

extern int fsok;	/* True if the boot device contains an VFS_PROC_NR. */
struct boot_params boot_params;

u32_t lowsec;	/* Offset to the file system on the boot device. */

/* Called by boot.c: */
int boot_nucleos(void);		/* Load and start a Nucleos image. */

/* Called by bootimage.c: */
/* Report a read error. */
void readerr(off_t sec, int err);

/* Transform u32_t to ASCII at base b or base 10. */
char *ul2a(u32_t n, unsigned b), *ul2a10(u32_t n);

/* Cheap atol(). */
long a2l(char *a);

/* ASCII to hex. */
unsigned a2x(char *a);

/* Translate a device name to a device number. */
dev_t name2dev(char *name);

/* True for a numeric prefix. */
int numprefix(char *s, char **ps);

/* True for a numeric string. */
int numeric(char *s);

/* Give a descriptive text for some UNIX errors. */
char *unix_err(int err);

void readblock(off_t, char *, int);
void delay(char *);
#endif /* __ARCH_X86_BOOT_BOOT_H */
