/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <sys/types.h> header contains important data type definitions.
 * It is considered good programming practice to use these definitions, 
 * instead of the underlying base type.  By convention, all type names end 
 * with _t.
 */

#ifndef _TYPES_H
#define _TYPES_H

/* The type size_t holds all results of the sizeof operator.  At first glance,
 * it seems obvious that it should be an unsigned int, but this is not always 
 * the case. For example, MINIX-ST (68000) has 32-bit pointers and 16-bit
 * integers. When one asks for the size of a 70K struct or array, the result 
 * requires 17 bits to express, so size_t must be a long type.  The type 
 * ssize_t is the signed version of size_t.
 */
#ifndef _MNX_SIZE_T
#define _MNX_SIZE_T
typedef unsigned int MNX(size_t);
#endif

#ifndef _MNX_SSIZE_T
#define _MNX_SSIZE_T
typedef int MNX(ssize_t);
#endif

#ifndef _MNX_TIME_T
#define _MNX_TIME_T
typedef long MNX(time_t);    /* time in sec since 1 Jan 1970 0000 GMT */
#endif

#ifndef _MNX_CLOCK_T
#define _MNX_CLOCK_T
typedef long MNX(clock_t);   /* unit for system accounting */
#endif

#ifndef _MNX_SIGSET_T
#define _MNX_SIGSET_T
typedef unsigned long MNX(sigset_t);
#endif

/* Open Group Base Specifications Issue 6 (not complete) */
typedef long MNX(useconds_t);           /* Time in microseconds */

/* Types used in disk, inode, etc. data structures. */
typedef short          MNX(dev_t);      /* holds (major|minor) device pair */
typedef char           MNX(gid_t);      /* group id */
typedef unsigned long  MNX(ino_t);      /* i-node number (V3 filesystem) */
typedef unsigned short MNX(mode_t);     /* file type and permissions bits */
typedef short          MNX(nlink_t);    /* number of links to a file */
typedef long	       MNX(off_t);	   /* offset within a file */
typedef int            MNX(pid_t);      /* process id (must be signed) */
typedef short          MNX(uid_t);      /* user id */
typedef unsigned long  MNX(zone_t);     /* zone number */
typedef unsigned long  MNX(block_t);    /* block number */
typedef unsigned long  MNX(bit_t);      /* bit number in a bit map */
typedef unsigned short MNX(zone1_t);    /* zone number for V1 file systems */
typedef unsigned short MNX(bitchunk_t); /* collection of bits in a bitmap */

typedef unsigned char   MNX(u8_t);   /* 8 bit type */
typedef unsigned short MNX(u16_t);   /* 16 bit type */
typedef unsigned long  MNX(u32_t);   /* 32 bit type */

typedef char            MNX(i8_t);      /* 8 bit signed type */
typedef short          MNX(i16_t);      /* 16 bit signed type */
typedef long           MNX(i32_t);      /* 32 bit signed type */

typedef unsigned long long MNX(u64_t);

/* The following types are needed because MINIX uses K&R style function
 * definitions (for maximum portability).  When a short, such as dev_t, is
 * passed to a function with a K&R definition, the compiler automatically
 * promotes it to an int.  The prototype must contain an int as the parameter,
 * not a short, because an int is what an old-style function definition
 * expects.  Thus using dev_t in a prototype would be incorrect.  It would be
 * sufficient to just use int instead of dev_t in the prototypes, but Dev_t
 * is clearer.
 */
typedef int           MNX(Dev_t);
typedef int           _mnx_Gid_t;
typedef int         MNX(Nlink_t);
typedef int           _mnx_Uid_t;
typedef int            MNX(U8_t);
typedef unsigned long MNX(U32_t);
typedef int            MNX(I8_t);
typedef int           MNX(I16_t);
typedef long          MNX(I32_t);

/* ANSI C makes writing down the promotion of unsigned types very messy.  When
 * sizeof(short) == sizeof(int), there is no promotion, so the type stays
 * unsigned.  When the compiler is not ANSI, there is usually no loss of
 * unsignedness, and there are usually no prototypes so the promoted type
 * doesn't matter.  The use of types like Ino_t is an attempt to use ints
 * (which are not promoted) while providing information to the reader.
 */

typedef unsigned long  MNX(Ino_t);

/*typedef int Ino_t; Ino_t is now 32 bits */
typedef int         MNX(Zone1_t);
typedef int      MNX(Bitchunk_t);
typedef int           MNX(U16_t);
typedef int          _mnx_Mode_t;

/* Compatibility with other systems */
typedef unsigned char    MNX(u_char);
typedef unsigned short  MNX(u_short);
typedef unsigned int      MNX(u_int);
typedef unsigned long    MNX(u_long);
typedef char           *MNX(caddr_t);

#endif /* _TYPES_H */
