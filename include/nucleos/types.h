/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @dir include/nucleos Architecturally independent declarations and definitions
 * @file include/nucleos/types.h
 * @brief Data types (architecturally independent)
 */
#ifndef __NUCLEOS_TYPES_H
#define __NUCLEOS_TYPES_H

#include <asm/types.h>
#include <nucleos/posix_types.h>

#ifndef _ANSI_H
#include <ansi.h>
#endif

#if defined(__KERNEL__) || defined(__UKERNEL__)

typedef __u32	__kernel_zone_t;	/* zone number */
typedef __u32	__kernel_block_t;	/* block number */
typedef __u32	__kernel_bit_t;		/* bit number in a bit map */
typedef __u16	__kernel_zone1_t;	/* zone number for V1 file systems */
typedef __u16	__kernel_bitchunk_t;	/* collection of bits in a bitmap */
typedef __s16	__kernel_dev_t;

/* Open Group Base Specifications Issue 6 (not complete) */
typedef __kernel_useconds_t	useconds_t;	/* Time in microseconds */

typedef __kernel_fd_set		fd_set;
typedef __kernel_dev_t		dev_t;		/* holds (major|minor) device pair */
typedef __kernel_uid_t		uid_t;		/* user id */
typedef __kernel_gid_t		gid_t;		/* group id */
typedef __kernel_pid_t		pid_t;		/* process id (must be signed) */
typedef __kernel_ino_t		ino_t; 		/* i-node number (V3 filesystem) */
typedef __kernel_mode_t		mode_t;		/* file type and permissions bits */
typedef __kernel_nlink_t	nlink_t;	/* number of links to a file */
typedef __kernel_off_t		off_t;		/* offset within a file */

#if defined(__GNUC__)
typedef __kernel_loff_t		loff_t;
#endif

typedef __kernel_zone_t		zone_t;		/* zone number */
typedef __kernel_block_t	block_t;	/* block number */
typedef __kernel_bit_t		bit_t;		/* bit number in a bit map */
typedef __kernel_zone1_t	zone1_t;	/* zone number for V1 file systems */
typedef __kernel_bitchunk_t	bitchunk_t;	/* collection of bits in a bitmap */

typedef _Bool			bool;

/* Integer type capable of holding a pointer and the largest integer type. */
typedef unsigned long	uintptr_t;
typedef int		intptr_t;
typedef unsigned long	uintmax_t;
typedef long		intmax_t;

/* The following types are needed because MINIX uses K&R style function
 * definitions (for maximum portability).  When a short, such as dev_t, is
 * passed to a function with a K&R definition, the compiler automatically
 * promotes it to an int.  The prototype must contain an int as the parameter,
 * not a short, because an int is what an old-style function definition
 * expects.  Thus using dev_t in a prototype would be incorrect.  It would be
 * sufficient to just use int instead of dev_t in the prototypes, but Dev_t
 * is clearer.
 */
typedef __s32	__kernel_Dev_t;
typedef __s32	__kernel__mnx_Gid_t;
typedef __s32	__kernel_Nlink_t;
typedef __s32	__kernel__mnx_Uid_t;
typedef __s32	__kernel_U8_t;
typedef __u32	__kernel_U32_t;
typedef __s32	__kernel_I8_t;
typedef __s32	__kernel_I16_t;
typedef __s32	__kernel_I32_t;

/* ANSI C makes writing down the promotion of unsigned types very messy.  When
 * sizeof(short) == sizeof(int), there is no promotion, so the type stays
 * unsigned.  When the compiler is not ANSI, there is usually no loss of
 * unsignedness, and there are usually no prototypes so the promoted type
 * doesn't matter.  The use of types like Ino_t is an attempt to use ints
 * (which are not promoted) while providing information to the reader.
 */
typedef __u32	__kernel_Ino_t;
typedef __s32	__kernel_Zone1_t;
typedef __s32	__kernel_Bitchunk_t;
typedef __s32	__kernel_U16_t;
typedef __s32	__kernel__mnx_Mode_t;

typedef __kernel_Dev_t		Dev_t;
typedef __kernel__mnx_Gid_t	_mnx_Gid_t;
typedef __kernel_Nlink_t	Nlink_t;
typedef __kernel__mnx_Uid_t	_mnx_Uid_t;
typedef __kernel_U8_t		U8_t;
typedef __kernel_U32_t		U32_t;
typedef __kernel_I8_t		I8_t;
typedef __kernel_I16_t		I16_t;
typedef __kernel_I32_t		I32_t;

typedef __kernel_Ino_t  Ino_t;
typedef __kernel_Zone1_t	Zone1_t;
typedef __kernel_Bitchunk_t	Bitchunk_t;
typedef __kernel_U16_t		U16_t;
typedef __kernel__mnx_Mode_t	_mnx_Mode_t;

/* Signal handler type, e.g. SIG_IGN */
typedef void (*sighandler_t)(int);

/* Devices. */
#define MAJOR              8    /* major device = (dev>>MAJOR) & 0377 */
#define MINOR              0    /* minor device = (dev>>MINOR) & 0377 */

#ifndef minor
#define minor(dev)      (((dev) >> MINOR) & 0xff)
#endif

#ifndef major
#define major(dev)      (((dev) >> MAJOR) & 0xff)
#endif

#ifndef makedev
#define makedev(major, minor)   \
                        ((dev_t) (((major) << MAJOR) | ((minor) << MINOR)))
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t		size_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef __kernel_ptrdiff_t	ptrdiff_t;	/* result of subtracting two pointers */
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __kernel_ssize_t	ssize_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef __kernel_time_t		time_t;		/* time in sec since 1 Jan 1970 0000 GMT */
#endif

#ifndef _CLOCK_T
#define _CLOCK_T
typedef __kernel_clock_t	clock_t;	/* unit for system accounting */
#endif

#ifndef _SIGSET_T
#define _SIGSET_T
typedef __kernel_sigset_t	sigset_t;
#endif

#ifndef _CADDR_T
#define _CADDR_T
typedef __kernel_caddr_t	*caddr_t;
#endif

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__

typedef __u8		u_int8_t;
typedef __s8		int8_t;
typedef __u16		u_int16_t;
typedef __s16		int16_t;
typedef __u32		u_int32_t;
typedef __s32		int32_t;

#endif /* !(__BIT_TYPES_DEFINED__) */

typedef __u8		uint8_t;
typedef __u16		uint16_t;
typedef __u32		uint32_t;

#if defined(__GNUC__)
typedef __u64		uint64_t;
typedef __u64		u_int64_t;
typedef __s64		int64_t;
#endif

/* this is a special 64bit data type that is 8-byte aligned */
#define aligned_u64 __u64 __attribute__((aligned(8)))
#define aligned_be64 __be64 __attribute__((aligned(8)))
#define aligned_le64 __le64 __attribute__((aligned(8)))

/* @klenovic: don't use these below (will be removed) */
typedef u8  u8_t;	/* 8 bit type */
typedef u16 u16_t;	/* 16 bit type */
typedef u32 u32_t;	/* 32 bit type */

typedef s8  i8_t;	/* 8 bit signed type */
typedef s16 i16_t;	/* 16 bit signed type */
typedef s32 i32_t;	/* 32 bit signed type */
typedef u64 u64_t;

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */
#endif /* __NUCLEOS_TYPES_H */
