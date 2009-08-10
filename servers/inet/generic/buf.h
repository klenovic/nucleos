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
buf.h

Copyright 1995 Philip Homburg
*/

#ifndef BUF_H
#define BUF_H

/* Note: BUF_S should be defined in const.h */

#define MAX_BUFREQ_PRI	10

#define ARP_PRI_REC		3
#define ARP_PRI_SEND		3

#define ETH_PRI_PORTBUFS	3
#define ETH_PRI_FDBUFS_EXTRA	5
#define ETH_PRI_FDBUFS		6

#define IP_PRI_PORTBUFS		3
#define IP_PRI_ASSBUFS		4
#define IP_PRI_FDBUFS_EXTRA	5
#define IP_PRI_FDBUFS		6

#define ICMP_PRI_QUEUE		1

#define TCP_PRI_FRAG2SEND	4
#define TCP_PRI_CONN_EXTRA	5
#define TCP_PRI_CONNwoUSER	7
#define TCP_PRI_CONN_INUSE	9

#define UDP_PRI_FDBUFS_EXTRA	5
#define UDP_PRI_FDBUFS		6

#define PSIP_PRI_EXP_PROMISC	2

struct acc;
typedef void (*buffree_t)(struct acc *acc);
typedef void (*bf_freereq_t)(int priority);

#ifdef BUF_CONSISTENCY_CHECK
typedef void (*bf_checkreq_t)(void);
#endif

typedef struct buf
{
	int buf_linkC;
	buffree_t buf_free;
	size_t buf_size;
	char *buf_data_p;

#ifdef BUF_TRACK_ALLOC_FREE
	char *buf_alloc_file;
	int buf_alloc_line;
	char *buf_free_file;
	int buf_free_line;
#endif
#ifdef BUF_CONSISTENCY_CHECK
	unsigned buf_generation;
	int buf_check_linkC;
#endif
} buf_t;

typedef struct acc
{
	int acc_linkC;
	int acc_offset, acc_length;
	buf_t *acc_buffer;
	struct acc *acc_next, *acc_ext_link;

#ifdef BUF_TRACK_ALLOC_FREE
	char *acc_alloc_file;
	int acc_alloc_line;
	char *acc_free_file;
	int acc_free_line;
#endif
#ifdef BUF_CONSISTENCY_CHECK
	unsigned acc_generation;
	int acc_check_linkC;
#endif
} acc_t;

extern acc_t *bf_temporary_acc;
extern acc_t *bf_linkcheck_acc;

/* For debugging... */

#ifdef BUF_TRACK_ALLOC_FREE

#ifndef BUF_IMPLEMENTATION

#define bf_memreq(a) _bf_memreq(__FILE__, __LINE__, a)
#define bf_cut(a,b,c) _bf_cut(__FILE__, __LINE__, a, b, c)
#define bf_delhead(a,b) _bf_delhead(__FILE__, __LINE__, a, b)
#define bf_packIffLess(a,b) _bf_packIffLess(__FILE__, __LINE__, \
									a, b)
#define bf_afree(a) _bf_afree(__FILE__, __LINE__, a)
#define bf_pack(a) _bf_pack(__FILE__, __LINE__, a)
#define bf_append(a,b) _bf_append(__FILE__, __LINE__, a, b)
#define bf_dupacc(a) _bf_dupacc(__FILE__, __LINE__, a)
#if 0
#define bf_mark_1acc(a) _bf_mark_1acc(__FILE__, __LINE__, a)
#define bf_mark_acc(a) _bf_mark_acc(__FILE__, __LINE__, a)
#endif
#define bf_align(a,s,al) _bf_align(__FILE__, __LINE__, a, s, al)

#else /* BUF_IMPLEMENTATION */

#define bf_afree(a) _bf_afree(clnt_file, clnt_line, a)
#define bf_pack(a) _bf_pack(clnt_file, clnt_line, a)
#define bf_memreq(a) _bf_memreq(clnt_file, clnt_line, a)
#define bf_dupacc(a) _bf_dupacc(clnt_file, clnt_line, a)
#define bf_cut(a,b,c) _bf_cut(clnt_file, clnt_line, a, b, c)
#define bf_delhead(a,b) _bf_delhead(clnt_file, clnt_line, a, b)
#define bf_align(a,s,al) _bf_align(clnt_file, clnt_line, a, s, al)

#endif /* !BUF_IMPLEMENTATION */

#else

#define bf_mark_1acc(acc)	((void)0)
#define bf_mark_acc(acc)	((void)0)

#endif /* BUF_TRACK_ALLOC_FREE */

/* Prototypes */

void bf_init(void);
#ifndef BUF_CONSISTENCY_CHECK
void bf_logon(bf_freereq_t func);
#else
void bf_logon(bf_freereq_t func, bf_checkreq_t checkfunc);
#endif

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_memreq(unsigned size);
#else
acc_t *_bf_memreq(char *clnt_file, int clnt_line, unsigned size);
#endif
/* the result is an acc with linkC == 1 */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_dupacc(acc_t *acc);
#else
acc_t *_bf_dupacc(char *clnt_file, int clnt_line, acc_t *acc);
#endif
/* the result is an acc with linkC == 1 identical to the given one */

#ifndef BUF_TRACK_ALLOC_FREE
void bf_afree(acc_t *acc);
#else
void _bf_afree(char *clnt_file, int clnt_line, acc_t *acc);
#endif
/* this reduces the linkC off the given acc with one */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_pack(acc_t *pack);
#else
acc_t *_bf_pack(char *clnt_file, int clnt_line, acc_t *pack);
#endif
/* this gives a packed copy of the given acc, the linkC of the given acc is
   reduced by one, the linkC of the result == 1 */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_packIffLess(acc_t *pack, int min_len);
#else
acc_t *_bf_packIffLess(char *clnt_file, int clnt_line, acc_t *pack, int min_len);
#endif
/* this performs a bf_pack iff pack->acc_length<min_len */

size_t bf_bufsize(acc_t *pack);
/* this gives the length of the buffer specified by the given acc. The linkC
   of the given acc remains the same */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_cut(acc_t *data, unsigned offset, unsigned length);
#else
acc_t *_bf_cut(char *clnt_file, int clnt_line, acc_t *data, unsigned offset, unsigned length);
#endif
/* the result is a cut of the buffer from offset with length length.
   The linkC of the result == 1, the linkC of the given acc remains the
   same. */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_delhead(acc_t *data, unsigned offset);
#else
acc_t *_bf_delhead(char *clnt_file, int clnt_line, acc_t *data, unsigned offset);
#endif
/* the result is a cut of the buffer from offset until the end.
   The linkC of the result == 1, the linkC of the given acc is
   decremented. */

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_append(acc_t *data_first, acc_t  *data_second);
#else
acc_t *_bf_append(char *clnt_file, int clnt_line, acc_t *data_first, acc_t  *data_second);
#endif
/* data_second is appended after data_first, a link is returned to the
	result and the linkCs of data_first and data_second are reduced.
	further more, if the contents of the last part of data_first and
	the first part of data_second fit in a buffer, both parts are
	copied into a (possibly fresh) buffer
*/

#ifndef BUF_TRACK_ALLOC_FREE
acc_t *bf_align(acc_t *acc, size_t size, size_t alignment);
#else
acc_t *_bf_align(char *clnt_file, int clnt_line, acc_t *acc, size_t size, size_t alignment);
#endif
/* size bytes of acc (or all bytes of acc if the size buffer is smaller
	than size) are aligned on an address that is multiple of alignment.
	Size must be less than or equal to BUF_S.
*/

int bf_linkcheck(acc_t *acc);
/* check if all link count are positive, and offsets and sizes are within 
 * the underlying buffer.
 */

#define ptr2acc_data(/* acc_t * */ a) (bf_temporary_acc=(a), \
	(&bf_temporary_acc->acc_buffer->buf_data_p[bf_temporary_acc-> \
		acc_offset]))

#define bf_chkbuf(buf) ((buf)? (compare((buf)->acc_linkC,>,0), \
	compare((buf)->acc_buffer, !=, 0), \
	compare((buf)->acc_buffer->buf_linkC,>,0)) : (void)0)

#ifdef BUF_CONSISTENCY_CHECK
int bf_consistency_check(void);
void bf_check_acc(acc_t *acc);
void _bf_mark_1acc(char *clnt_file, int clnt_line, acc_t *acc);
void _bf_mark_acc(char *clnt_file, int clnt_line, acc_t *acc);
#endif

#endif /* BUF_H */
