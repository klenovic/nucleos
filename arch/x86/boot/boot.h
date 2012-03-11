/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _ARCH_X86_BOOT_BOOT_H
#define _ARCH_X86_BOOT_BOOT_H

#define SECTOR_SIZE	512
#define STACK_SIZE	512	/* Minimum number of bytes for stack */

/* BIOS video modes. */
#define MONO_MODE	0x07	/* 80x25 monochrome. */
#define COLOR_MODE	0x03	/* 80x25 color. */

/* 8086 vector */
typedef struct vector {
	u16 offset;
	u16 segment;
} vector;

extern struct setup_header hdr;
extern struct boot_params boot_params;

vector rem_part;	/* Boot partition table entry. */
u32 caddr, daddr;	/* Code and data address of the boot program. */
u32 runsize;		/* Size of this program. */
u16 device;		/* BIOS device number. Drive being booted from. */

struct memory mem[NR_MEMS];	/* List of available memory. */

/* halt cpu */
void halt_cpu(void);

int printf(const char *fmt,...);

/* Local address to absolute address. */
u32 mon2abs(void *ptr);

/* Copy bytes from anywhere to anywhere (extended memory too) */
void raw_copy(u32 dstaddr, u32 srcaddr, u32 count);

void *sbrk(int incr);

/* Send a character to the screen. */
void putch(int c);

/* Enable copying console I/O to a serial line. */
void serial_init(int line);

/* Set video mode */
void set_mode(unsigned mode);

/* clear the screen. */
void clear_screen(void);

/* System bus type, XT, AT, or MCA. */
u16 get_bus(void);

/* Display type, MDA to VGA. */
u16 get_video(void);

/* Current value of the clock tick counter. */
u32 get_tick(void);

/* Start Nucleos */
void pm_nucleos(u32 koff, u32 kcs, u32 kds, u32 boot_params);

/* Called by boot.c: */
int boot_nucleos(void);	/* Load and start a Nucleos image. */

#endif /* _ARCH_X86_BOOT_BOOT_H */
