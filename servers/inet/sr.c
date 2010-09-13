/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	this file contains the interface of the network software with the file
 *	system.
 *
 * Copyright 1995 Philip Homburg
 *
 * The valid messages and their parameters are:
 * 
 * Requests:
 *
 *    m_type      NDEV_MINOR   NDEV_PROC    NDEV_REF   NDEV_MODE
 * -------------------------------------------------------------
 * | DEV_OPEN    |minor dev  | proc nr   |  fd       |   mode   |
 * |-------------+-----------+-----------+-----------+----------+
 * | DEV_CLOSE   |minor dev  | proc nr   |  fd       |          |
 * |-------------+-----------+-----------+-----------+----------+
 *
 *    m_type      NDEV_MINOR   NDEV_PROC    NDEV_REF   NDEV_COUNT NDEV_BUFFER
 * ---------------------------------------------------------------------------
 * | DEV_READ    |minor dev  | proc nr   |  fd       |  count    | buf ptr   |
 * |-------------+-----------+-----------+-----------+-----------+-----------|
 * | DEV_WRITE   |minor dev  | proc nr   |  fd       |  count    | buf ptr   |
 * |-------------+-----------+-----------+-----------+-----------+-----------|
 *
 *    m_type      NDEV_MINOR   NDEV_PROC    NDEV_REF   NDEV_IOCTL NDEV_BUFFER
 * ---------------------------------------------------------------------------
 * | DEV_IOCTL3  |minor dev  | proc nr   |  fd       |  command  | buf ptr   |
 * |-------------+-----------+-----------+-----------+-----------+-----------|
 *
 *    m_type      NDEV_MINOR   NDEV_PROC    NDEV_REF   NDEV_OPERATION
 * -------------------------------------------------------------------|
 * | DEV_CANCEL  |minor dev  | proc nr   |  fd       | which operation|
 * |-------------+-----------+-----------+-----------+----------------|
 *
 * Replies:
 *
 *    m_type        REP_PROC_NR   REP_STATUS   REP_REF    REP_OPERATION
 * ----------------------------------------------------------------------|
 * | DEVICE_REPLY |   proc nr   |  status    |  fd     | which operation |
 * |--------------+-------------+------------+---------+-----------------|
 */

#include "inet.h"

#include <nucleos/svrctl.h>
#include <nucleos/unistd.h>

#include "mq.h"
#include "qp.h"
#include "proto.h"
#include "generic/type.h"

#include "generic/assert.h"
#include "generic/buf.h"
#include "generic/event.h"
#include "generic/sr.h"
#include "sr_int.h"

#define DEV_CANCEL NW_CANCEL
#define DEVICE_REPLY KCNR_TASK_REPLY
#define DEV_IOCTL3 DEV_IOCTL
#define NDEV_BUFFER ADDRESS
#define NDEV_COUNT COUNT
#define NDEV_IOCTL REQUEST
#define NDEV_MINOR DEVICE
#define NDEV_PROC IO_ENDPT

sr_fd_t sr_fd_table[FD_NR];

static mq_t *repl_queue, *repl_queue_tail;
static struct vir_cp_req vir_cp_req[CPVEC_NR];
static struct vscp_vec s_cp_req[CPVEC_NR];

static int sr_open(kipc_msg_t *m);
static void sr_close(kipc_msg_t *m);
static int sr_rwio(mq_t *m);
static int sr_rwio_s(mq_t *m);
static int sr_restart_read(sr_fd_t *fdp);
static int sr_restart_write(sr_fd_t *fdp);
static int sr_restart_ioctl(sr_fd_t *fdp);
static int sr_cancel(kipc_msg_t *m);
static int sr_select(kipc_msg_t *m);
static void sr_status(kipc_msg_t *m);
static void sr_reply_(mq_t *m, int reply, int is_revive);
static sr_fd_t *sr_getchannel(int minor);
static acc_t *sr_get_userdata(int fd, size_t offset, size_t count, int for_ioctl);
static int sr_put_userdata(int fd, size_t offset, acc_t *data, int for_ioctl);
static void sr_select_res(int fd, unsigned ops);
static int sr_repl_queue(int proc, int ref, int operation);
static int walk_queue(sr_fd_t *sr_fd, mq_t **q_head_ptr, mq_t **q_tail_ptr, int type,
		      int proc_nr, int ref, int first_flag);
static void process_req_q(mq_t *mq, mq_t *tail, mq_t **tail_ptr);
static void sr_event(event_t *evp, ev_arg_t arg);
static int cp_u2b(int proc, char *src, acc_t **var_acc_ptr, int size);
static int cp_b2u(acc_t *acc_ptr, int proc, char *dest);
static int cp_u2b_s(int proc, int gid, vir_bytes offset, acc_t **var_acc_ptr, int size);
static int cp_b2u_s(acc_t *acc_ptr, int proc, int gid, vir_bytes offset);

void sr_init()
{
	int i;

	for (i=0; i<FD_NR; i++)
	{
		sr_fd_table[i].srf_flags= SFF_FREE;
		ev_init(&sr_fd_table[i].srf_ioctl_ev);
		ev_init(&sr_fd_table[i].srf_read_ev);
		ev_init(&sr_fd_table[i].srf_write_ev);
	}
	repl_queue= NULL;
}

void sr_rec(m)
mq_t *m;
{
	int result;
	int send_reply, free_mess;

	if (repl_queue)
	{
		if (m->mq_mess.m_type == DEV_CANCEL)
		{
			result= sr_repl_queue(m->mq_mess.IO_ENDPT, 
				(int)m->mq_mess.IO_GRANT, 0);
			if (result)
			{
				mq_free(m);
				return;	/* canceled request in queue */
			}
		}
#if 0
		else
			sr_repl_queue(ENDPT_ANY, 0, 0);
#endif
	}

	switch (m->mq_mess.m_type)
	{
	case DEV_OPEN:
		result= sr_open(&m->mq_mess);
		send_reply= 1;
		free_mess= 1;
		break;
	case DEV_CLOSE:
		sr_close(&m->mq_mess);
		result= 0;
		send_reply= 1;
		free_mess= 1;
		break;
#ifdef DEV_READ
	case DEV_READ:
	case DEV_WRITE:
	case DEV_IOCTL3:
		result= sr_rwio(m);
		assert(result == 0 || result == SUSPEND);
		send_reply= (result == SUSPEND);
		free_mess= 0;
		break;
#endif
	case DEV_READ_S:
	case DEV_WRITE_S:
	case DEV_IOCTL_S:
		result= sr_rwio_s(m);
		assert(result == 0 || result == SUSPEND);
		send_reply= (result == SUSPEND);
		free_mess= 0;
		break;
	case DEV_CANCEL:
		result= sr_cancel(&m->mq_mess);
		assert(result == 0 || result == -EINTR);
		send_reply= (result == -EINTR);
		free_mess= 1;
		m->mq_mess.m_type= 0;
		break;
	case DEV_SELECT:
		result= sr_select(&m->mq_mess);
		send_reply= 1;
		free_mess= 1;
		break;
	case DEV_STATUS:
		sr_status(&m->mq_mess);
		send_reply= 0;
		free_mess= 1;
		break;
	default:
		ip_panic(("unknown message, from %d, type %d",
				m->mq_mess.m_source, m->mq_mess.m_type));
	}
	if (send_reply)
	{
		sr_reply_(m, result, FALSE /* !is_revive */);
	}
	if (free_mess)
		mq_free(m);
}

void sr_add_minor(minor, port, openf, closef, readf, writef,
	ioctlf, cancelf, selectf)
int minor;
int port;
sr_open_t openf;
sr_close_t closef;
sr_read_t readf;
sr_write_t writef;
sr_ioctl_t ioctlf;
sr_cancel_t cancelf;
sr_select_t selectf;
{
	sr_fd_t *sr_fd;

	assert (minor>=0 && minor<FD_NR);

	sr_fd= &sr_fd_table[minor];

	assert(!(sr_fd->srf_flags & SFF_INUSE));

	sr_fd->srf_flags= SFF_INUSE | SFF_MINOR;
	sr_fd->srf_port= port;
	sr_fd->srf_open= openf;
	sr_fd->srf_close= closef;
	sr_fd->srf_write= writef;
	sr_fd->srf_read= readf;
	sr_fd->srf_ioctl= ioctlf;
	sr_fd->srf_cancel= cancelf;
	sr_fd->srf_select= selectf;
}

static int sr_open(m)
kipc_msg_t *m;
{
	sr_fd_t *sr_fd;

	int minor= m->NDEV_MINOR;
	int i, fd;

	if (minor<0 || minor>FD_NR)
	{
		DBLOCK(1, printf("replying -EINVAL\n"));
		return -EINVAL;
	}
	if (!(sr_fd_table[minor].srf_flags & SFF_MINOR))
	{
		DBLOCK(1, printf("replying -ENXIO\n"));
		return -ENXIO;
	}
	for (i=0; i<FD_NR && (sr_fd_table[i].srf_flags & SFF_INUSE); i++);

	if (i>=FD_NR)
	{
		DBLOCK(1, printf("replying -ENFILE\n"));
		return -ENFILE;
	}

	sr_fd= &sr_fd_table[i];
	*sr_fd= sr_fd_table[minor];
	sr_fd->srf_flags= SFF_INUSE;
	fd= (*sr_fd->srf_open)(sr_fd->srf_port, i, sr_get_userdata,
		sr_put_userdata, 0 /* no put_pkt */, sr_select_res);
	if (fd<0)
	{
		sr_fd->srf_flags= SFF_FREE;
		DBLOCK(1, printf("replying %d\n", fd));
		return fd;
	}
	sr_fd->srf_fd= fd;
	return i;
}

static void sr_close(m)
kipc_msg_t *m;
{
	sr_fd_t *sr_fd;

	sr_fd= sr_getchannel(m->NDEV_MINOR);
	assert (sr_fd);

	if (sr_fd->srf_flags & SFF_BUSY)
		ip_panic(("close on busy channel"));

	assert (!(sr_fd->srf_flags & SFF_MINOR));
	(*sr_fd->srf_close)(sr_fd->srf_fd);
	sr_fd->srf_flags= SFF_FREE;
}

static int sr_rwio(m)
mq_t *m;
{
	sr_fd_t *sr_fd;
	mq_t **q_head_ptr, **q_tail_ptr;
	int ip_flag, susp_flag, first_flag;
	int r;
	ioreq_t request;
	size_t size;

	sr_fd= sr_getchannel(m->mq_mess.NDEV_MINOR);
	assert (sr_fd);

	switch(m->mq_mess.m_type)
	{
#ifdef DEV_READ
	case DEV_READ:
		q_head_ptr= &sr_fd->srf_read_q;
		q_tail_ptr= &sr_fd->srf_read_q_tail;
		ip_flag= SFF_READ_IP;
		susp_flag= SFF_READ_SUSP;
		first_flag= SFF_READ_FIRST;
		break;
	case DEV_WRITE:
		q_head_ptr= &sr_fd->srf_write_q;
		q_tail_ptr= &sr_fd->srf_write_q_tail;
		ip_flag= SFF_WRITE_IP;
		susp_flag= SFF_WRITE_SUSP;
		first_flag= SFF_WRITE_FIRST;
		break;
	case DEV_IOCTL3:
		q_head_ptr= &sr_fd->srf_ioctl_q;
		q_tail_ptr= &sr_fd->srf_ioctl_q_tail;
		ip_flag= SFF_IOCTL_IP;
		susp_flag= SFF_IOCTL_SUSP;
		first_flag= SFF_IOCTL_FIRST;
		break;
#endif
	default:
		ip_panic(("illegal case entry"));
	}

	if (sr_fd->srf_flags & ip_flag)
	{
		assert(sr_fd->srf_flags & susp_flag);
		assert(*q_head_ptr);

		(*q_tail_ptr)->mq_next= m;
		*q_tail_ptr= m;
		return SUSPEND;
	}
	assert(!*q_head_ptr);

	*q_tail_ptr= *q_head_ptr= m;
	sr_fd->srf_flags |= ip_flag;
	assert(!(sr_fd->srf_flags & first_flag));
	sr_fd->srf_flags |= first_flag;

	switch(m->mq_mess.m_type)
	{
#ifdef DEV_READ
	case DEV_READ:
		r= (*sr_fd->srf_read)(sr_fd->srf_fd, 
			m->mq_mess.NDEV_COUNT);
		break;
	case DEV_WRITE:
		r= (*sr_fd->srf_write)(sr_fd->srf_fd, 
			m->mq_mess.NDEV_COUNT);
		break;
	case DEV_IOCTL3:
		request= m->mq_mess.NDEV_IOCTL;
		size= (request >> 16) & _IOCPARM_MASK;
		if (size>MAX_IOCTL_S)
		{
			DBLOCK(1, printf("replying -EINVAL\n"));
			r= sr_put_userdata(sr_fd-sr_fd_table, -EINVAL, 
				NULL, 1);
			assert(r == 0);
			assert(sr_fd->srf_flags & first_flag);
			sr_fd->srf_flags &= ~first_flag;
			return 0;
		}
		r= (*sr_fd->srf_ioctl)(sr_fd->srf_fd, request);
			break;
#endif
	default:
		ip_panic(("illegal case entry"));
	}

	assert(sr_fd->srf_flags & first_flag);
	sr_fd->srf_flags &= ~first_flag;

	assert(r == 0 || r == SUSPEND || 
		(printf("r= %d\n", r), 0));
	if (r == SUSPEND)
		sr_fd->srf_flags |= susp_flag;
	else
		mq_free(m);
	return r;
		}

static int sr_rwio_s(m)
mq_t *m;
{
	sr_fd_t *sr_fd;
	mq_t **q_head_ptr, **q_tail_ptr;
	int ip_flag, susp_flag, first_flag;
	int r;
	ioreq_t request;
	size_t size;

	sr_fd= sr_getchannel(m->mq_mess.NDEV_MINOR);
	assert (sr_fd);

	switch(m->mq_mess.m_type)
	{
	case DEV_READ_S:
		q_head_ptr= &sr_fd->srf_read_q;
		q_tail_ptr= &sr_fd->srf_read_q_tail;
		ip_flag= SFF_READ_IP;
		susp_flag= SFF_READ_SUSP;
		first_flag= SFF_READ_FIRST;
		break;
	case DEV_WRITE_S:
		q_head_ptr= &sr_fd->srf_write_q;
		q_tail_ptr= &sr_fd->srf_write_q_tail;
		ip_flag= SFF_WRITE_IP;
		susp_flag= SFF_WRITE_SUSP;
		first_flag= SFF_WRITE_FIRST;
		break;
	case DEV_IOCTL_S:
		q_head_ptr= &sr_fd->srf_ioctl_q;
		q_tail_ptr= &sr_fd->srf_ioctl_q_tail;
		ip_flag= SFF_IOCTL_IP;
		susp_flag= SFF_IOCTL_SUSP;
		first_flag= SFF_IOCTL_FIRST;
		break;
	default:
		ip_panic(("illegal case entry"));
	}

	if (sr_fd->srf_flags & ip_flag)
	{
		assert(sr_fd->srf_flags & susp_flag);
		assert(*q_head_ptr);

		(*q_tail_ptr)->mq_next= m;
		*q_tail_ptr= m;
		return SUSPEND;
	}
	assert(!*q_head_ptr);

	*q_tail_ptr= *q_head_ptr= m;
	sr_fd->srf_flags |= ip_flag;
	assert(!(sr_fd->srf_flags & first_flag));
	sr_fd->srf_flags |= first_flag;

	switch(m->mq_mess.m_type)
	{
	case DEV_READ_S:
		r= (*sr_fd->srf_read)(sr_fd->srf_fd, 
			m->mq_mess.NDEV_COUNT);
		break;
	case DEV_WRITE_S:
		r= (*sr_fd->srf_write)(sr_fd->srf_fd, 
			m->mq_mess.NDEV_COUNT);
		break;
	case DEV_IOCTL_S:
		request= m->mq_mess.NDEV_IOCTL;
		size= (request >> 16) & _IOCPARM_MASK;
		if (size>MAX_IOCTL_S)
		{
			DBLOCK(1, printf("replying -EINVAL\n"));
			r= sr_put_userdata(sr_fd-sr_fd_table, -EINVAL, 
				NULL, 1);
			assert(r == 0);
			assert(sr_fd->srf_flags & first_flag);
			sr_fd->srf_flags &= ~first_flag;
			return 0;
		}
		r= (*sr_fd->srf_ioctl)(sr_fd->srf_fd, request);
		break;
	default:
		ip_panic(("illegal case entry"));
	}

	assert(sr_fd->srf_flags & first_flag);
	sr_fd->srf_flags &= ~first_flag;

	assert(r == 0 || r == SUSPEND || 
		(printf("r= %d\n", r), 0));
	if (r == SUSPEND)
		sr_fd->srf_flags |= susp_flag;
	else
		mq_free(m);
	return r;
}

static int sr_restart_read(sr_fd)
sr_fd_t *sr_fd;
{
	mq_t *mp;
	int r;

	mp= sr_fd->srf_read_q;
	assert(mp);

	if (sr_fd->srf_flags & SFF_READ_IP)
	{
		assert(sr_fd->srf_flags & SFF_READ_SUSP);
		return SUSPEND;
	}
	sr_fd->srf_flags |= SFF_READ_IP;

	r= (*sr_fd->srf_read)(sr_fd->srf_fd, 
		mp->mq_mess.NDEV_COUNT);

	assert(r == 0 || r == SUSPEND || 
		(printf("r= %d\n", r), 0));
	if (r == SUSPEND)
		sr_fd->srf_flags |= SFF_READ_SUSP;
	return r;
}

static int sr_restart_write(sr_fd)
sr_fd_t *sr_fd;
{
	mq_t *mp;
	int r;

	mp= sr_fd->srf_write_q;
	assert(mp);

	if (sr_fd->srf_flags & SFF_WRITE_IP)
	{
		assert(sr_fd->srf_flags & SFF_WRITE_SUSP);
		return SUSPEND;
	}
	sr_fd->srf_flags |= SFF_WRITE_IP;

	r= (*sr_fd->srf_write)(sr_fd->srf_fd, 
		mp->mq_mess.NDEV_COUNT);

	assert(r == 0 || r == SUSPEND || 
		(printf("r= %d\n", r), 0));
	if (r == SUSPEND)
		sr_fd->srf_flags |= SFF_WRITE_SUSP;
	return r;
}

static int sr_restart_ioctl(sr_fd)
sr_fd_t *sr_fd;
{
	mq_t *mp;
	int r;

	mp= sr_fd->srf_ioctl_q;
	assert(mp);

	if (sr_fd->srf_flags & SFF_IOCTL_IP)
	{
		assert(sr_fd->srf_flags & SFF_IOCTL_SUSP);
		return SUSPEND;
	}
	sr_fd->srf_flags |= SFF_IOCTL_IP;

	r= (*sr_fd->srf_ioctl)(sr_fd->srf_fd, 
		mp->mq_mess.NDEV_COUNT);

	assert(r == 0 || r == SUSPEND || 
		(printf("r= %d\n", r), 0));
	if (r == SUSPEND)
		sr_fd->srf_flags |= SFF_IOCTL_SUSP;
	return r;
}

static int sr_cancel(m)
kipc_msg_t *m;
{
	sr_fd_t *sr_fd;
	int result;
	int proc_nr, ref, operation;

        result=-EINTR;
	proc_nr=  m->NDEV_PROC;
	ref=  (int)m->IO_GRANT;
	operation= 0;
	sr_fd= sr_getchannel(m->NDEV_MINOR);
	assert (sr_fd);

	{
		result= walk_queue(sr_fd, &sr_fd->srf_ioctl_q, 
			&sr_fd->srf_ioctl_q_tail, SR_CANCEL_IOCTL,
			proc_nr, ref, SFF_IOCTL_FIRST);
		if (result != -EAGAIN)
			return result;
	}
	{
		result= walk_queue(sr_fd, &sr_fd->srf_read_q, 
			&sr_fd->srf_read_q_tail, SR_CANCEL_READ,
			proc_nr, ref, SFF_READ_FIRST);
		if (result != -EAGAIN)
			return result;
	}
	{
		result= walk_queue(sr_fd, &sr_fd->srf_write_q, 
			&sr_fd->srf_write_q_tail, SR_CANCEL_WRITE,
			proc_nr, ref, SFF_WRITE_FIRST);
		if (result != -EAGAIN)
			return result;
	}
	ip_panic((
"request not found: from %d, type %d, MINOR= %d, PROC= %d, REF= %d",
		m->m_source, m->m_type, m->NDEV_MINOR,
		m->NDEV_PROC, m->IO_GRANT));
}

static int sr_select(m)
kipc_msg_t *m;
{
	sr_fd_t *sr_fd;
	mq_t **q_head_ptr, **q_tail_ptr;
	int ip_flag, susp_flag;
	int r, ops;
	unsigned m_ops, i_ops;
	ioreq_t request;
	size_t size;

	sr_fd= sr_getchannel(m->NDEV_MINOR);
	assert (sr_fd);

	sr_fd->srf_select_proc= m->m_source;

	m_ops= m->IO_ENDPT;
	i_ops= 0;
	if (m_ops & SEL_RD) i_ops |= SR_SELECT_READ;
	if (m_ops & SEL_WR) i_ops |= SR_SELECT_WRITE;
	if (m_ops & SEL_ERR) i_ops |= SR_SELECT_EXCEPTION;
	if (!(m_ops & SEL_NOTIFY)) i_ops |= SR_SELECT_POLL;

	r= (*sr_fd->srf_select)(sr_fd->srf_fd,  i_ops);
	if (r < 0)
		return r;
	m_ops= 0;
	if (r & SR_SELECT_READ) m_ops |= SEL_RD;
	if (r & SR_SELECT_WRITE) m_ops |= SEL_WR;
	if (r & SR_SELECT_EXCEPTION) m_ops |= SEL_ERR;

	return m_ops;
}

static void sr_status(m)
kipc_msg_t *m;
{
	int fd, result;
	unsigned m_ops;
	sr_fd_t *sr_fd;
	mq_t *mq;

	mq= repl_queue;
	if (mq != NULL)
	{
		repl_queue= mq->mq_next;

		mq->mq_mess.m_type= DEV_REVIVE;
		result= kipc_send(mq->mq_mess.m_source, &mq->mq_mess, 0);
		if (result != 0)
			ip_panic(("unable to send"));
		mq_free(mq);

		return;
	}

	for (fd=0, sr_fd= sr_fd_table; fd<FD_NR; fd++, sr_fd++)
	{
		if ((sr_fd->srf_flags &
			(SFF_SELECT_R|SFF_SELECT_W|SFF_SELECT_X)) == 0)
		{
			/* Nothing to report */
			continue;
		}
		if (sr_fd->srf_select_proc != m->m_source)
		{
			/* Wrong process */
			continue;
		}

		m_ops= 0;
		if (sr_fd->srf_flags & SFF_SELECT_R) m_ops |= SEL_RD;
		if (sr_fd->srf_flags & SFF_SELECT_W) m_ops |= SEL_WR;
		if (sr_fd->srf_flags & SFF_SELECT_X) m_ops |= SEL_ERR;

		sr_fd->srf_flags &= ~(SFF_SELECT_R|SFF_SELECT_W|SFF_SELECT_X);

		m->m_type= DEV_IO_READY;
		m->DEV_MINOR= fd;
		m->DEV_SEL_OPS= m_ops;

		result= kipc_send(m->m_source, m, 0);
		if (result != 0)
			ip_panic(("unable to send"));
		return;
	}

	m->m_type= DEV_NO_STATUS;
	result= kipc_send(m->m_source, m, 0);
	if (result != 0)
		ip_panic(("unable to send"));
}

static int walk_queue(sr_fd, q_head_ptr, q_tail_ptr, type, proc_nr, ref,
	first_flag)
sr_fd_t *sr_fd;
mq_t **q_head_ptr;
mq_t **q_tail_ptr;
int type;
int proc_nr;
int ref;
int first_flag;
{
	mq_t *q_ptr_prv, *q_ptr;
	int result;

	for(q_ptr_prv= NULL, q_ptr= *q_head_ptr; q_ptr; 
		q_ptr_prv= q_ptr, q_ptr= q_ptr->mq_next)
	{
		if (q_ptr->mq_mess.NDEV_PROC != proc_nr)
			continue;
		if ((int)q_ptr->mq_mess.IO_GRANT != ref)
			continue;
		if (!q_ptr_prv)
		{
			assert(!(sr_fd->srf_flags & first_flag));
			sr_fd->srf_flags |= first_flag;

			result= (*sr_fd->srf_cancel)(sr_fd->srf_fd, type);
			assert(result == 0);

			*q_head_ptr= q_ptr->mq_next;
			mq_free(q_ptr);

			assert(sr_fd->srf_flags & first_flag);
			sr_fd->srf_flags &= ~first_flag;

			return 0;
		}
		q_ptr_prv->mq_next= q_ptr->mq_next;
		mq_free(q_ptr);
		if (!q_ptr_prv->mq_next)
			*q_tail_ptr= q_ptr_prv;
		return -EINTR;
	}
	return -EAGAIN;
}

static sr_fd_t *sr_getchannel(minor)
int minor;
{
	sr_fd_t *loc_fd;

	compare(minor, >=, 0);
	compare(minor, <, FD_NR);

	loc_fd= &sr_fd_table[minor];

	assert (!(loc_fd->srf_flags & SFF_MINOR) &&
		(loc_fd->srf_flags & SFF_INUSE));

	return loc_fd;
}

static void sr_reply_(mq, status, is_revive)
mq_t *mq;
int status;
int is_revive;
{
	int result, proc, ref,operation;
	kipc_msg_t reply, *mp;

	proc= mq->mq_mess.NDEV_PROC;
	ref= (int)mq->mq_mess.IO_GRANT;
	operation= mq->mq_mess.m_type;

	if (is_revive)
		mp= &mq->mq_mess;
	else
		mp= &reply;

	mp->m_type= DEVICE_REPLY;
	mp->REP_ENDPT= proc;
	mp->REP_STATUS= status;
	mp->REP_IO_GRANT= ref;
	if (is_revive)
	{
		kipc_notify(mq->mq_mess.m_source);
		result= -ELOCKED;
	}
	else
	{
		result= kipc_send(mq->mq_mess.m_source, mp, 0);
	}

	if (result == -ELOCKED && is_revive)
	{
		mq->mq_next= NULL;
		if (repl_queue)
			repl_queue_tail->mq_next= mq;
		else
			repl_queue= mq;
		repl_queue_tail= mq;
		return;
	}
	if (result != 0)
		ip_panic(("unable to send"));
	if (is_revive)
		mq_free(mq);
}

static acc_t *sr_get_userdata (fd, offset, count, for_ioctl)
int fd;
size_t offset;
size_t count;
int for_ioctl;
{
	sr_fd_t *loc_fd;
	mq_t **head_ptr, *m, *mq;
	int ip_flag, susp_flag, first_flag, m_type, safe_copy;
	int result, suspended, is_revive;
	char *src;
	acc_t *acc;
	event_t *evp;
	ev_arg_t arg;

	loc_fd= &sr_fd_table[fd];

	if (for_ioctl)
	{
		head_ptr= &loc_fd->srf_ioctl_q;
		evp= &loc_fd->srf_ioctl_ev;
		ip_flag= SFF_IOCTL_IP;
		susp_flag= SFF_IOCTL_SUSP;
		first_flag= SFF_IOCTL_FIRST;
	}
	else
	{
		head_ptr= &loc_fd->srf_write_q;
		evp= &loc_fd->srf_write_ev;
		ip_flag= SFF_WRITE_IP;
		susp_flag= SFF_WRITE_SUSP;
		first_flag= SFF_WRITE_FIRST;
	}
		
assert (loc_fd->srf_flags & ip_flag);

	if (!count)
	{
		m= *head_ptr;
		mq= m->mq_next;
		*head_ptr= mq;
		result= (int)offset;
		is_revive= !(loc_fd->srf_flags & first_flag);
		sr_reply_(m, result, is_revive);
		suspended= (loc_fd->srf_flags & susp_flag);
		loc_fd->srf_flags &= ~(ip_flag|susp_flag);
		if (suspended)
		{
			if (mq)
			{
				arg.ev_ptr= loc_fd;
				ev_enqueue(evp, sr_event, arg);
			}
		}
		return NULL;
	}

	m_type= (*head_ptr)->mq_mess.m_type;
	if (m_type == DEV_READ_S || m_type == DEV_WRITE_S ||
		m_type == DEV_IOCTL_S)
	{
		safe_copy= 1;
	}
	else
	{
#ifdef DEV_READ
		assert(m_type == DEV_READ || m_type == DEV_WRITE ||
			m_type == DEV_IOCTL);
#else
		ip_panic(("sr_get_userdata: m_type not *_S\n"));
#endif
		safe_copy= 0;
	}

	if (safe_copy)
	{
		result= cp_u2b_s ((*head_ptr)->mq_mess.NDEV_PROC,
			(int)(*head_ptr)->mq_mess.NDEV_BUFFER, offset, &acc,
			count);
	}
	else
	{
	src= (*head_ptr)->mq_mess.NDEV_BUFFER + offset;
		result= cp_u2b ((*head_ptr)->mq_mess.NDEV_PROC, src, &acc,
			count);
	}

	return result<0 ? NULL : acc;
}

static int sr_put_userdata (fd, offset, data, for_ioctl)
int fd;
size_t offset;
acc_t *data;
int for_ioctl;
{
	sr_fd_t *loc_fd;
	mq_t **head_ptr, *m, *mq;
	int ip_flag, susp_flag, first_flag, m_type, safe_copy;
	int result, suspended, is_revive;
	char *dst;
	event_t *evp;
	ev_arg_t arg;

	loc_fd= &sr_fd_table[fd];

	if (for_ioctl)
	{
		head_ptr= &loc_fd->srf_ioctl_q;
		evp= &loc_fd->srf_ioctl_ev;
		ip_flag= SFF_IOCTL_IP;
		susp_flag= SFF_IOCTL_SUSP;
		first_flag= SFF_IOCTL_FIRST;
	}
	else
	{
		head_ptr= &loc_fd->srf_read_q;
		evp= &loc_fd->srf_read_ev;
		ip_flag= SFF_READ_IP;
		susp_flag= SFF_READ_SUSP;
		first_flag= SFF_READ_FIRST;
	}
		
	assert (loc_fd->srf_flags & ip_flag);

	if (!data)
	{
		m= *head_ptr;
		mq= m->mq_next;
		*head_ptr= mq;
		result= (int)offset;
		is_revive= !(loc_fd->srf_flags & first_flag);
		sr_reply_(m, result, is_revive);
		suspended= (loc_fd->srf_flags & susp_flag);
		loc_fd->srf_flags &= ~(ip_flag|susp_flag);
		if (suspended)
		{
			if (mq)
			{
				arg.ev_ptr= loc_fd;
				ev_enqueue(evp, sr_event, arg);
			}
		}
		return 0;
	}

	m_type= (*head_ptr)->mq_mess.m_type;
	if (m_type == DEV_READ_S || m_type == DEV_WRITE_S ||
		m_type == DEV_IOCTL_S)
	{
		safe_copy= 1;
	}
	else
	{
#ifdef DEV_READ
		assert(m_type == DEV_READ || m_type == DEV_WRITE ||
			m_type == DEV_IOCTL);
#else
		ip_panic(("sr_put_userdata: m_type not *_S\n"));
#endif
		safe_copy= 0;
	}

	if (safe_copy)
	{
		return cp_b2u_s (data, (*head_ptr)->mq_mess.NDEV_PROC, 
			(int)(*head_ptr)->mq_mess.NDEV_BUFFER, offset);
	}
	else
	{
	dst= (*head_ptr)->mq_mess.NDEV_BUFFER + offset;
	return cp_b2u (data, (*head_ptr)->mq_mess.NDEV_PROC, dst);
}
}

static void sr_select_res(fd, ops)
int fd;
unsigned ops;
{
	sr_fd_t *sr_fd;

	sr_fd= &sr_fd_table[fd];
	
	if (ops & SR_SELECT_READ) sr_fd->srf_flags |= SFF_SELECT_R;
	if (ops & SR_SELECT_WRITE) sr_fd->srf_flags |= SFF_SELECT_W;
	if (ops & SR_SELECT_EXCEPTION) sr_fd->srf_flags |= SFF_SELECT_X;

	kipc_notify(sr_fd->srf_select_proc);
}

static void process_req_q(mq, tail, tail_ptr)
mq_t *mq, *tail, **tail_ptr;
{
	mq_t *m;
	int result;

	for(;mq;)
	{
		m= mq;
		mq= mq->mq_next;

		result= sr_rwio(m);
		if (result == SUSPEND)
		{
			if (mq)
			{
				(*tail_ptr)->mq_next= mq;
				*tail_ptr= tail;
			}
			return;
		}
	}
	return;
}

static void sr_event(evp, arg)
event_t *evp;
ev_arg_t arg;
{
	sr_fd_t *sr_fd;
	int r;

	sr_fd= arg.ev_ptr;
	if (evp == &sr_fd->srf_write_ev)
	{
		while(sr_fd->srf_write_q)
		{
			r= sr_restart_write(sr_fd);
			if (r == SUSPEND)
				return;
		}
		return;
	}
	if (evp == &sr_fd->srf_read_ev)
	{
		while(sr_fd->srf_read_q)
		{
			r= sr_restart_read(sr_fd);
			if (r == SUSPEND)
				return;
		}
		return;
	}
	if (evp == &sr_fd->srf_ioctl_ev)
	{
		while(sr_fd->srf_ioctl_q)
		{
			r= sr_restart_ioctl(sr_fd);
			if (r == SUSPEND)
				return;
		}
		return;
	}
	ip_panic(("sr_event: unkown event\n"));
}

static int cp_u2b (proc, src, var_acc_ptr, size)
int proc;
char *src;
acc_t **var_acc_ptr;
int size;
{
	static kipc_msg_t mess;
	acc_t *acc;
	int i;

	acc= bf_memreq(size);

	*var_acc_ptr= acc;
	i=0;

	while (acc)
	{
		size= (vir_bytes)acc->acc_length;

		vir_cp_req[i].count= size;
		vir_cp_req[i].src.proc_nr_e = proc;
		vir_cp_req[i].src.segment = D;
		vir_cp_req[i].src.offset = (vir_bytes) src;
		vir_cp_req[i].dst.proc_nr_e = this_proc;
		vir_cp_req[i].dst.segment = D;
		vir_cp_req[i].dst.offset = (vir_bytes) ptr2acc_data(acc);

		src += size;
		acc= acc->acc_next;
		i++;

		if (i == CPVEC_NR || acc == NULL)
		{
			mess.m_type= SYS_VIRVCOPY;
			mess.VCP_VEC_SIZE= i;
			mess.VCP_VEC_ADDR= (char *)vir_cp_req;

			if (kipc_module_call(KIPC_SENDREC, 0, SYSTASK, &mess) <0)
				ip_panic(("unable to sendrec"));
			if (mess.m_type <0)
			{
				bf_afree(*var_acc_ptr);
				*var_acc_ptr= 0;
				return mess.m_type;
			}
			i= 0;
		}
	}
	return 0;
}

static int cp_b2u (acc_ptr, proc, dest)
acc_t *acc_ptr;
int proc;
char *dest;
{
	static kipc_msg_t mess;
	acc_t *acc;
	int i, size;

	acc= acc_ptr;
	i=0;

	while (acc)
	{
		size= (vir_bytes)acc->acc_length;

		if (size)
		{
			vir_cp_req[i].src.proc_nr_e = this_proc;
			vir_cp_req[i].src.segment = D;
			vir_cp_req[i].src.offset= (vir_bytes)ptr2acc_data(acc);
			vir_cp_req[i].dst.proc_nr_e = proc;
			vir_cp_req[i].dst.segment = D;
			vir_cp_req[i].dst.offset= (vir_bytes)dest;
			vir_cp_req[i].count= size;
			i++;
		}

		dest += size;
		acc= acc->acc_next;

		if (i == CPVEC_NR || acc == NULL)
		{
			mess.m_type= SYS_VIRVCOPY;
			mess.VCP_VEC_SIZE= i;
			mess.VCP_VEC_ADDR= (char *) vir_cp_req;

			if (kipc_module_call(KIPC_SENDREC, 0, SYSTASK, &mess) <0)
				ip_panic(("unable to sendrec"));
			if (mess.m_type <0)
			{
				bf_afree(acc_ptr);
				return mess.m_type;
			}
			i= 0;
		}
	}
	bf_afree(acc_ptr);
	return 0;
}

static int cp_u2b_s(proc, gid, offset, var_acc_ptr, size)
int proc;
int gid;
vir_bytes offset;
acc_t **var_acc_ptr;
int size;
{
	acc_t *acc;
	int i, r;

	acc= bf_memreq(size);

	*var_acc_ptr= acc;
	i=0;

	while (acc)
	{
		size= (vir_bytes)acc->acc_length;

		s_cp_req[i].v_from= proc;
		s_cp_req[i].v_to= ENDPT_SELF;
		s_cp_req[i].v_gid= gid;
		s_cp_req[i].v_offset= offset;
		s_cp_req[i].v_addr= (vir_bytes) ptr2acc_data(acc);
		s_cp_req[i].v_bytes= size;

		offset += size;
		acc= acc->acc_next;
		i++;

		if (acc == NULL && i == 1)
		{
			r= sys_safecopyfrom(s_cp_req[0].v_from,
				s_cp_req[0].v_gid, s_cp_req[0].v_offset,
				s_cp_req[0].v_addr, s_cp_req[0].v_bytes, D);
			if (r <0)
			{
				printf("sys_safecopyfrom failed: %d\n", r);
				bf_afree(*var_acc_ptr);
				*var_acc_ptr= 0;
				return r;
			}
			i= 0;
			continue;
		}
		if (i == CPVEC_NR || acc == NULL)
		{
			r= sys_vsafecopy(s_cp_req, i);

			if (r <0)
			{
				printf("cp_u2b_s: sys_vsafecopy failed: %d\n",
					r);
				bf_afree(*var_acc_ptr);
				*var_acc_ptr= 0;
				return r;
			}
			i= 0;
		}
	}
	return 0;
}

static int cp_b2u_s(acc_ptr, proc, gid, offset)
acc_t *acc_ptr;
int proc;
int gid;
vir_bytes offset;
{
	acc_t *acc;
	int i, r, size;

	acc= acc_ptr;
	i=0;

	while (acc)
	{
		size= (vir_bytes)acc->acc_length;

		if (size)
		{
			s_cp_req[i].v_from= ENDPT_SELF;
			s_cp_req[i].v_to= proc;
			s_cp_req[i].v_gid= gid;
			s_cp_req[i].v_offset= offset;
			s_cp_req[i].v_addr= (vir_bytes) ptr2acc_data(acc);
			s_cp_req[i].v_bytes= size;

			i++;
		}

		offset += size;
		acc= acc->acc_next;

		if (acc == NULL && i == 1)
		{
			r= sys_safecopyto(s_cp_req[0].v_to,
				s_cp_req[0].v_gid, s_cp_req[0].v_offset,
				s_cp_req[0].v_addr, s_cp_req[0].v_bytes, D);
			if (r <0)
			{
				printf("sys_safecopyto failed: %d\n", r);
				bf_afree(acc_ptr);
				return r;
			}
			i= 0;
			continue;
		}
		if (i == CPVEC_NR || acc == NULL)
		{
			r= sys_vsafecopy(s_cp_req, i);

			if (r <0)
			{
				printf("cp_b2u_s: sys_vsafecopy failed: %d\n",
					r);
				bf_afree(acc_ptr);
				return r;
			}
			i= 0;
		}
	}
	bf_afree(acc_ptr);
	return 0;
}

static int sr_repl_queue(proc, ref, operation)
int proc;
int ref;
int operation;
{
	mq_t *m, *m_cancel, *m_tmp;
	mq_t *new_queue;
	int result;

	m_cancel= NULL;
	new_queue= NULL;

	for (m= repl_queue; m;)
	{
		if (m->mq_mess.REP_ENDPT == proc &&
			m->mq_mess.REP_IO_GRANT == ref)

		{
			assert(!m_cancel);
			m_cancel= m;
			m= m->mq_next;
			continue;
		}
		m_tmp= m;
		m= m->mq_next;
		m_tmp->mq_next= new_queue;
		new_queue= m_tmp;
	}
	repl_queue= NULL;
	repl_queue= new_queue;

	if (m_cancel)
	{
		result= kipc_send(m_cancel->mq_mess.m_source, &m_cancel->mq_mess, 0);
		if (result != 0)
			ip_panic(("unable to send: %d", result));
		mq_free(m_cancel);
		return 1;
	}
	return 0;
}
