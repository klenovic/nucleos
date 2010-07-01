/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * Copyright (c) 1985, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * Send query to name server and wait for reply.
 */
#include <nucleos/types.h>
#include <nucleos/ioctl.h>
#include <nucleos/select.h>
#include <nucleos/stat.h>
#include <assert.h>
#include <nucleos/errno.h>
#include <nucleos/fcntl.h>
#include <nucleos/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>

#include <net/hton.h>

#include <net/netlib.h>
#include <net/in.h>
#include <net/inet.h>
#include <net/netdb.h>
#include <net/nameser.h>
#include <net/resolv.h>
#include <net/tcp.h>
#include <net/tcp_io.h>
#include <net/udp.h>
#include <net/udp_hdr.h>
#include <net/udp_io.h>

#include <asm/ioctls.h>

static int tcp_connect(ipaddr_t host, tcpport_t port, int *terrno);
static int tcpip_writeall(int fd, const char *buf, size_t siz);
static int udp_connect(void);
static int udp_sendto(int fd, const char *buf, unsigned buflen, ipaddr_t addr, udpport_t port);
static int udp_receive(int fd, char *buf, unsigned buflen, time_t timeout);

static int s = -1;	/* socket used for communications */

int res_send(buf, buflen, answer, anslen)
	const char *buf;
	int buflen;
	char *answer;
	int anslen;
{
	register int n;
	int try, v_circuit, resplen, ns;
	int gotsomewhere = 0, connected = 0;
	int connreset = 0;
	u16_t id, len;
	char *cp;
	time_t timeout;
	dns_hdr_t *hp = (dns_hdr_t *) buf;
	dns_hdr_t *anhp = (dns_hdr_t *) answer;
	int terrno = ETIMEDOUT;
	char junk[512];

#ifdef DEBUG
	if (_res.options & RES_DEBUG) {
		printf("res_send()\n");
		__p_query(buf);
	}
#endif /* DEBUG */
	if (!(_res.options & RES_INIT))
		if (res_init() == -1) {
			return(-1);
		}

	v_circuit = (_res.options & RES_USEVC) || buflen > PACKETSZ;
	id = hp->dh_id;
	/*
	 * Send request, RETRY times, or until successful
	 */
	for (try = 0; try < _res.retry; try++) {
	   for (ns = 0; ns < _res.nscount; ns++) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf("Querying server (# %d) address = %s\n", ns+1,
			      inet_ntoa(_res.nsaddr_list[ns]));
#endif /* DEBUG */
	usevc:
		if (v_circuit) {
			int truncated = 0;
			int nbytes;

			/*
			 * Use virtual circuit;
			 * at most one attempt per server.
			 */
			try = _res.retry;
			if (s < 0) 
			{
				s= tcp_connect(_res.nsaddr_list[ns],
					_res.nsport_list[ns], &terrno);
				if (s == -1)
					continue;
			}
			/*
			 * Send length & message
			 */
			len = htons((u_short)buflen);
			nbytes= tcpip_writeall(s, (char *)&len, 
				sizeof(len));
			if (nbytes != sizeof(len))
			{
				terrno= errno;
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					fprintf(stderr, "write failed: %s\n",
					strerror(terrno));
#endif /* DEBUG */
				close(s);
				s= -1;
				continue;
			}
			nbytes= tcpip_writeall(s, buf, buflen);
			if (nbytes != buflen)
			{
				terrno= errno;
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					fprintf(stderr, "write failed: %s\n",
					strerror(terrno));
#endif /* DEBUG */
				close(s);
				s= -1;
				continue;
			}
			/*
			 * Receive length & response
			 */
			cp = answer;
			len = sizeof(short);
			while (len != 0)
			{
				n = read(s, (char *)cp, (int)len);
				if (n <= 0)
					break;
				cp += n;
				assert(len >= n);
				len -= n;
			}
			if (len) {
				terrno = errno;
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					fprintf(stderr, "read failed: %s\n",
						strerror(terrno));
#endif /* DEBUG */
				close(s);
				s= -1;
				/*
				 * A long running process might get its TCP
				 * connection reset if the remote server was
				 * restarted.  Requery the server instead of
				 * trying a new one.  When there is only one
				 * server, this means that a query might work
				 * instead of failing.  We only allow one reset
				 * per query to prevent looping.
				 */
				if (terrno == ECONNRESET && !connreset) {
					connreset = 1;
					ns--;
				}
				continue;
			}
			cp = answer;
			if ((resplen = ntohs(*(u_short *)cp)) > anslen) {
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					fprintf(stderr, "response truncated\n");
#endif /* DEBUG */
				len = anslen;
				truncated = 1;
			} else
				len = resplen;
			while (len != 0)
			{
				n= read(s, (char *)cp, (int)len);
				if (n <= 0)
					break;
				cp += n;
				assert(len >= n);
				len -= n;
			}
			if (len) {
				terrno = errno;
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					fprintf(stderr, "read failed: %s\n",
						strerror(terrno));
#endif /* DEBUG */
				close(s);
				s= -1;
				continue;
			}
			if (truncated) {
				/*
				 * Flush rest of answer
				 * so connection stays in synch.
				 */
				anhp->dh_flag1 |= DHF_TC;
				len = resplen - anslen;
				while (len != 0) {
					n = (len > sizeof(junk) ?
					    sizeof(junk) : len);
					n = read(s, junk, n);
					if (n <= 0)
					{
						assert(len >= n);
						len -= n;
					}
					else
						break;
				}
			}
		} else {
			/*
			 * Use datagrams.
			 */
			if (s < 0) {
				s = udp_connect();
				if (s < 0) {
					terrno = errno;
#ifdef DEBUG
					if (_res.options & RES_DEBUG)
					    perror("udp_connect failed");
#endif /* DEBUG */
					continue;
				}
			}
			if (udp_sendto(s, buf, buflen, _res.nsaddr_list[ns],
				_res.nsport_list[ns]) != buflen) {
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					perror("sendto");
#endif /* DEBUG */
				continue;
			}

			/*
			 * Wait for reply
			 */
			timeout= (_res.retrans << try);
			if (try > 0)
				timeout /= _res.nscount;
			if (timeout <= 0)
				timeout= 1;
wait:
			if ((resplen= udp_receive(s, answer, anslen, timeout))
				== -1)
			{
				if (errno == EINTR)
				{
				/*
				 * timeout
				 */
#ifdef DEBUG
					if (_res.options & RES_DEBUG)
						printf("timeout\n");
#endif /* DEBUG */
					gotsomewhere = 1;
				}
				else
				{
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					perror("udp_receive");
#endif /* DEBUG */
				}
				continue;
			}
			gotsomewhere = 1;
			if (id != anhp->dh_id) {
				/*
				 * response from old query, ignore it
				 */
#ifdef DEBUG
				if (_res.options & RES_DEBUG) {
					printf("old answer:\n");
					__p_query(answer);
				}
#endif /* DEBUG */
				goto wait;
			}
			if (!(_res.options & RES_IGNTC) &&
				(anhp->dh_flag1 & DHF_TC)) {
				/*
				 * get rest of answer;
				 * use TCP with same server.
				 */
#ifdef DEBUG
				if (_res.options & RES_DEBUG)
					printf("truncated answer\n");
#endif /* DEBUG */
				(void) close(s);
				s = -1;
				v_circuit = 1;
				goto usevc;
			}
		}
#ifdef DEBUG
		if (_res.options & RES_DEBUG) {
			printf("got answer:\n");
			__p_query(answer);
		}
#endif /* DEBUG */
		/*
		 * If using virtual circuits, we assume that the first server
		 * is preferred * over the rest (i.e. it is on the local
		 * machine) and only keep that one open.
		 * If we have temporarily opened a virtual circuit,
		 * or if we haven't been asked to keep a socket open,
		 * close the socket.
		 */
		if ((v_circuit &&
		    ((_res.options & RES_USEVC) == 0 || ns != 0)) ||
		    (_res.options & RES_STAYOPEN) == 0) {
			(void) close(s);
			s = -1;
		}
		return (resplen);
	   }
	}
	if (s >= 0) {
		(void) close(s);
		s = -1;
	}
	if (v_circuit == 0)
		if (gotsomewhere == 0)
			errno = ECONNREFUSED;	/* no nameservers found */
		else
			errno = ETIMEDOUT;	/* no answer obtained */
	else
		errno = terrno;
	return (-1);
}

/*
 * This routine is for closing the socket if a virtual circuit is used and
 * the program wants to close it.  This provides support for endhostent()
 * which expects to close the socket.
 *
 * This routine is not expected to be user visible.
 */
void
_res_close()
{
	if (s != -1) {
		(void) close(s);
		s = -1;
	}
}

static int tcp_connect(host, port, terrno)
ipaddr_t host;
tcpport_t port;
int *terrno;
{
	char *dev_name;
	int fd;
	int error;
	nwio_tcpconf_t tcpconf;
	nwio_tcpcl_t clopt;

	dev_name= getenv("TCP_DEVICE");
	if (!dev_name)
		dev_name= TCP_DEVICE;
	fd= open(dev_name, O_RDWR);
	if (fd == -1)
	{
		*terrno= errno;
		return -1;
	}
	tcpconf.nwtc_flags= NWTC_EXCL | NWTC_LP_SEL | NWTC_SET_RA | NWTC_SET_RP;
	tcpconf.nwtc_remaddr= host;
	tcpconf.nwtc_remport= port;
	error= ioctl(fd, NWIOSTCPCONF, &tcpconf);
	if (error == -1)
	{
		*terrno= errno;
		close(fd);
		return -1;
	}
	clopt.nwtcl_flags= 0;
	error= ioctl(fd, NWIOTCPCONN, &clopt);
	if (error == -1)
	{
		*terrno= errno;
		close(fd);
		return -1;
	}
	*terrno= 0;
	return fd;
}

static int tcpip_writeall(fd, buf, siz)
int fd;
const char *buf;
size_t siz;
{
	size_t siz_org;
	int nbytes;

	siz_org= siz;

	while (siz)
	{
		nbytes= write(fd, buf, siz);
		if (nbytes <= 0)
			return siz_org-siz;
		assert(siz >= nbytes);
		buf += nbytes;
		siz -= nbytes;
	}
	return siz_org;
}


static int udp_connect()
{
	nwio_udpopt_t udpopt;
	char *dev_name;
	int fd, r, terrno;

	dev_name= getenv("UDP_DEVICE");
	if (!dev_name)
		dev_name= UDP_DEVICE;
	fd= open(dev_name, O_RDWR);
	if (fd == -1)
		return -1;

	udpopt.nwuo_flags= NWUO_COPY | NWUO_LP_SEL | NWUO_EN_LOC |
		NWUO_EN_BROAD | NWUO_RP_ANY | NWUO_RA_ANY | NWUO_RWDATALL |
		NWUO_DI_IPOPT;
	r= ioctl(fd, NWIOSUDPOPT, &udpopt);
	if (r == -1)
	{
		terrno= errno;
		close(fd);
		errno= terrno;
		return -1;
	}
	return fd;
}

static int udp_sendto(fd, buf, buflen, addr, port)
int fd;
const char *buf;
unsigned buflen;
ipaddr_t addr;
udpport_t port;
{
	char *newbuf;
	udp_io_hdr_t *udp_io_hdr;
	int r, terrno;

	newbuf= malloc(sizeof(*udp_io_hdr) + buflen);
	if (newbuf == NULL)
	{
		errno= ENOMEM;
		return -1;
	}
	udp_io_hdr= (udp_io_hdr_t *)newbuf;
	udp_io_hdr->uih_dst_addr= addr;
	udp_io_hdr->uih_dst_port= port;
	udp_io_hdr->uih_ip_opt_len= 0;
	udp_io_hdr->uih_data_len= buflen;

	memcpy(newbuf + sizeof(*udp_io_hdr), buf, buflen);
	r= write(fd, newbuf, sizeof(*udp_io_hdr) + buflen);
	terrno= errno;
	free(newbuf);
	if (r >= sizeof(*udp_io_hdr))
		r -= sizeof(*udp_io_hdr);
	errno= terrno;
	return r;
}

static int udp_receive(fd, buf, buflen, timeout)
int fd;
char *buf;
unsigned buflen;
time_t timeout;
{
	char *newbuf;
	udp_io_hdr_t *udp_io_hdr;
	int r, terrno;
	fd_set readfds;
	struct timeval timeval;

	newbuf = malloc(sizeof(*udp_io_hdr) + buflen);
	if (newbuf == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	/* only read if there is something to be read within timeout seconds */
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	timeval.tv_sec = timeout;
	timeval.tv_usec = 0;
	r = select(fd + 1, &readfds, NULL, NULL, &timeval);

	if (r >= 0 && !FD_ISSET(fd, &readfds)) {
		errno = EINTR;
		r = -1;
	}

	if (r >= 0)
		r= read(fd, newbuf, sizeof(*udp_io_hdr) + buflen);

	terrno = errno;

	if (r < 0 || r <= sizeof(*udp_io_hdr))
	{
		if (r > 0)
			r= 0;

		free(newbuf);
		errno= terrno;

		return r;
	}

	/* copy packet body to caller-provided buffer */
	memcpy(buf, newbuf + sizeof(*udp_io_hdr), r - sizeof(*udp_io_hdr));
	free(newbuf);

	return r - sizeof(*udp_io_hdr);
}
