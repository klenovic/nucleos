/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains a driver for:
 *     /dev/klog	- system log device
 *
 * Changes:
 *   21 July 2005   - Support for diagnostic messages (Jorrit N. Herder)
 *    7 July 2005   - Created (Ben Gras)
 */

#include "log.h"
#include <nucleos/time.h>
#include <nucleos/select.h>
#include <nucleos/endpoint.h>

#define LOG_DEBUG		0	/* enable/ disable debugging */

#define NR_DEVS            	1	/* number of minor devices */
#define MINOR_KLOG		0	/* /dev/klog */

#define LOGINC(n, i)	do { (n) = (((n) + (i)) % LOG_SIZE); } while(0)

struct logdevice logdevices[NR_DEVS];
static struct device log_geom[NR_DEVS];  	/* base and size of devices */
static int log_device = -1;	 		/* current device */

static char *log_name(void);
static struct device *log_prepare(int device);
static int log_transfer(int proc_nr, int opcode, u64_t position,
			iovec_t *iov, unsigned nr_req);
static int log_do_open(struct driver *dp, kipc_msg_t *m_ptr);
static int log_cancel(struct driver *dp, kipc_msg_t *m_ptr);
static int log_select(struct driver *dp, kipc_msg_t *m_ptr);
static void log_signal(struct driver *dp, sigset_t *set);
static int log_other(struct driver *dp, kipc_msg_t *m_ptr);
static void log_geometry(struct partition *entry);
static int subread(struct logdevice *log, int count, int proc_nr, vir_bytes user_vir, size_t);

/* Entry points to this driver. */
static struct driver log_dtab = {
  log_name,	/* current device's name */
  log_do_open,	/* open or mount */
  do_nop,	/* nothing on a close */
  nop_ioctl,	/* ioctl nop */
  log_prepare,	/* prepare for I/O on a given minor device */
  log_transfer,	/* do the I/O */
  nop_cleanup,	/* no need to clean up */
  log_geometry,	/* geometry */
  log_signal,	/* handle system signal */
  nop_alarm, 	/* no alarm */
  log_cancel,	/* CANCEL request */
  log_select,	/* DEV_SELECT request */
  log_other,	/* Unrecognized messages */
  NULL		/* HW int */
};

extern int device_caller;

/*===========================================================================*
 *				   main 				     *
 *===========================================================================*/
int main(void)
{
  int i;
  for(i = 0; i < NR_DEVS; i++) {
  	log_geom[i].dv_size = cvul64(LOG_SIZE);
 	log_geom[i].dv_base = cvul64((long)logdevices[i].log_buffer);
 	logdevices[i].log_size = logdevices[i].log_read =
	 	logdevices[i].log_write =
	 	logdevices[i].log_select_alerted =
	 	logdevices[i].log_selected =
	 	logdevices[i].log_select_ready_ops = 0;
 	logdevices[i].log_proc_nr = 0;
 	logdevices[i].log_revive_alerted = 0;
  }
  driver_task(&log_dtab, DRIVER_ASYN);
  return(0);
}

/*===========================================================================*
 *				 log_name					     *
 *===========================================================================*/
static char *log_name()
{
/* Return a name for the current device. */
  static char name[] = "log";
  return name;  
}

/*===========================================================================*
 *				log_prepare				     *
 *===========================================================================*/
static struct device *log_prepare(device)
int device;
{
/* Prepare for I/O on a device: check if the minor device number is ok. */

  if (device < 0 || device >= NR_DEVS) return(NIL_DEV);
  log_device = device;

  return(&log_geom[device]);
}

/*===========================================================================*
 *				subwrite				     *
 *===========================================================================*/
static int
subwrite(struct logdevice *log, int count, int proc_nr,
	vir_bytes user_vir, size_t offset)
{
	int d, r;
	char *buf;
	kipc_msg_t m;

	if (log->log_write + count > LOG_SIZE)
		count = LOG_SIZE - log->log_write;
	buf = log->log_buffer + log->log_write;

	if(proc_nr == ENDPT_SELF) {
		memcpy(buf, (char *) user_vir, count);
	}
	else {
		if((r=sys_safecopyfrom(proc_nr, user_vir, offset,
			(vir_bytes)buf, count, D)) != 0)
			return r;
	}

	LOGINC(log->log_write, count);
	log->log_size += count;

        if(log->log_size > LOG_SIZE) {
        	int overflow;
        	overflow = log->log_size - LOG_SIZE;
        	log->log_size -= overflow;
        	LOGINC(log->log_read, overflow);
        }

        if(log->log_size > 0 && log->log_proc_nr && !log->log_revive_alerted) {
        	/* Someone who was suspended on read can now
        	 * be revived.
        	 */
    		log->log_status = subread(log, log->log_iosize,
    			log->log_proc_nr, log->log_user_vir_g,
			log->log_user_vir_offset);

		m.m_type = DEV_REVIVE;
		m.REP_ENDPT = log->log_proc_nr;
		m.REP_STATUS  = log->log_status;
		m.REP_IO_GRANT  = log->log_user_vir_g;
  		r= kipc_send(log->log_source, &m);
		if (r != 0)
		{
			printf("log`subwrite: send to %d failed: %d\n",
				log->log_source, r);
		}
		log->log_proc_nr = 0;
 	} 

	if(log->log_size > 0)
		log->log_select_ready_ops |= SEL_RD;

	if(log->log_size > 0 && log->log_selected &&
	  !(log->log_select_alerted)) {
  		/* Someone(s) who was/were select()ing can now
  		 * be awoken. If there was a blocking read (above),
  		 * this can only happen if the blocking read didn't
  		 * swallow all the data (log_size > 0).
  		 */
  		if(log->log_selected & SEL_RD) {
			d= log-logdevices;
			m.m_type = DEV_SEL_REPL2;
			m.DEV_SEL_OPS = log->log_select_ready_ops;
			m.DEV_MINOR   = d;
#if LOG_DEBUG
			printf("select sending DEV_SEL_REPL2\n");
#endif
  			r= kipc_send(log->log_select_proc, &m);
			if (r != 0)
			{
				printf(	
				"log`subwrite: send to %d failed: %d\n",
					log->log_select_proc, r);
			}
			log->log_selected &= ~log->log_select_ready_ops;
  		}
  	}

        return count;
}

/*===========================================================================*
 *				log_append				*
 *===========================================================================*/
void
log_append(char *buf, int count)
{
	int w = 0, skip = 0;

	if(count < 1) return;
	if(count > LOG_SIZE) skip = count - LOG_SIZE;
	count -= skip;
	buf += skip;
	w = subwrite(&logdevices[0], count, ENDPT_SELF, (vir_bytes) buf,0);

	if(w > 0 && w < count)
		subwrite(&logdevices[0], count-w, ENDPT_SELF, (vir_bytes) buf+w,0);
	return;
}

/*===========================================================================*
 *				subread					     *
 *===========================================================================*/
static int
subread(struct logdevice *log, int count, int proc_nr,
	vir_bytes user_vir, size_t offset)
{
	char *buf;
	int r;
    	if (count > log->log_size)
    		count = log->log_size;
        if (log->log_read + count > LOG_SIZE)
        	count = LOG_SIZE - log->log_read;

    	buf = log->log_buffer + log->log_read;
	if((r=sys_safecopyto(proc_nr, user_vir, offset,
		(vir_bytes)buf, count, D)) != 0)
		return r;

  	LOGINC(log->log_read, count);
        log->log_size -= count;

        return count;
}

/*===========================================================================*
 *				log_transfer				     *
 *===========================================================================*/
static int log_transfer(proc_nr, opcode, position, iov, nr_req)
int proc_nr;			/* process doing the request */
int opcode;			/* DEV_GATHER_S or DEV_SCATTER_S */
u64_t position;			/* offset on device to read or write */
iovec_t *iov;			/* pointer to read or write request vector */
unsigned nr_req;		/* length of request vector */
{
/* Read or write one the driver's minor devices. */
  unsigned count;
  vir_bytes user_vir;
  struct device *dv;
  unsigned long dv_size;
  int accumulated_read = 0;
  struct logdevice *log;
  static int f;
  size_t vir_offset = 0;

  if(log_device < 0 || log_device >= NR_DEVS)
  	return -EIO;

  /* Get minor device number and check for /dev/null. */
  dv = &log_geom[log_device];
  dv_size = cv64ul(dv->dv_size);
  log = &logdevices[log_device];

  while (nr_req > 0) {
	/* How much to transfer and where to / from. */
	count = iov->iov_size;
	user_vir = iov->iov_addr;

	switch (log_device) {

	case MINOR_KLOG:
	    if (opcode == DEV_GATHER_S) {
	    	if (log->log_proc_nr || count < 1) {
	    		/* There's already someone hanging to read, or
	    		 * no real I/O requested.
	    		 */
	    		return 0;
	    	}

	    	if (!log->log_size) {
	    		if(accumulated_read)
	    			return 0;
	    		/* No data available; let caller block. */
	    		log->log_proc_nr = proc_nr;
	    		log->log_iosize = count;
	    		log->log_user_vir_g = user_vir;
	    		log->log_user_vir_offset = 0;
	    		log->log_revive_alerted = 0;

			/* Device_caller is a global in drivers library. */
	    		log->log_source = device_caller;
#if LOG_DEBUG
	    		printf("blocked %d (%d)\n", 
	    			log->log_source, log->log_proc_nr);
#endif
	    		return(-EDONTREPLY);
	    	}
	    	count = subread(log, count, proc_nr, user_vir, vir_offset);
	    	if(count < 0) {
	    		return count;
	    	}
	    	accumulated_read += count;
	    } else {
	    	count = subwrite(log, count, proc_nr, user_vir, vir_offset);
	    	if(count < 0)
	    		return count;
	    }
	    break;
	/* Unknown (illegal) minor device. */
	default:
	    return(-EINVAL);
	}

	/* Book the number of bytes transferred. */
	vir_offset += count;
  	if ((iov->iov_size -= count) == 0) { iov++; nr_req--; vir_offset = 0; }
  }
  return 0;
}

/*============================================================================*
 *				log_do_open				      *
 *============================================================================*/
static int log_do_open(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
  if (log_prepare(m_ptr->DEVICE) == NIL_DEV) return(-ENXIO);
  return 0;
}

/*============================================================================*
 *				log_geometry				      *
 *============================================================================*/
static void log_geometry(entry)
struct partition *entry;
{
  /* take a page from the fake memory device geometry */
  entry->heads = 64;
  entry->sectors = 32;
  entry->cylinders = div64u(log_geom[log_device].dv_size, SECTOR_SIZE) /
  	(entry->heads * entry->sectors);
}

/*============================================================================*
 *				log_cancel				      *
 *============================================================================*/
static int log_cancel(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
  int d;
  d = m_ptr->TTY_LINE;
  if(d < 0 || d >= NR_DEVS)
  	return -EINVAL;
  logdevices[d].log_proc_nr = 0;
  logdevices[d].log_revive_alerted = 0;
  return 0;
}


/*============================================================================*
 *				log_signal				      *
 *============================================================================*/
static void log_signal(dp, set)
struct driver *dp;
sigset_t *set;
{
  if (sigismember(set, SIGKMESS)) {
	do_new_kmess(SYSTEM);
  }	
}

	
/*============================================================================*
 *				log_other				      *
 *============================================================================*/
static int log_other(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
	int r;

	/* This function gets messages that the generic driver doesn't
	 * understand.
	 */
	if (is_notify(m_ptr->m_type)) {
		switch (_ENDPOINT_P(m_ptr->m_source)) {
			case TTY_PROC_NR:
				do_new_kmess(m_ptr->m_source);
				r = -EDONTREPLY;
				break;
			default:
				r = -EINVAL;
				break;
		}
		return r;
	}

	switch(m_ptr->m_type) {
	case DIAGNOSTICS_OLD: {
		r = do_diagnostics(m_ptr, 0);
		break;
	}
	case ASYN_DIAGNOSTICS_OLD:
	case DIAGNOSTICS_S_OLD:
		r = do_diagnostics(m_ptr, 1);
		break;
	case DEV_STATUS: {
		printf("log_other: unexpected DEV_STATUS request\n");
		r = -EDONTREPLY;
		break;
	}
	default:
		r = -EINVAL;
		break;
	}
	return r;
}

/*============================================================================*
 *				log_select				      *
 *============================================================================*/
static int log_select(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
  int d, ready_ops = 0, ops = 0;
  d = m_ptr->TTY_LINE;
  if(d < 0 || d >= NR_DEVS) {
#if LOG_DEBUG
  	printf("line %d? -EINVAL\n", d);
#endif
  	return -EINVAL;
  }

  ops = m_ptr->IO_ENDPT & (SEL_RD|SEL_WR|SEL_ERR);

  	/* Read blocks when there is no log. */
  if((m_ptr->IO_ENDPT & SEL_RD) && logdevices[d].log_size > 0) {
#if LOG_DEBUG
  	printf("log can read; size %d\n", logdevices[d].log_size);
#endif
  	ready_ops |= SEL_RD; /* writes never block */
 }

  	/* Write never blocks. */
  if(m_ptr->IO_ENDPT & SEL_WR) ready_ops |= SEL_WR;

	/* Enable select calback if no operations were
	 * ready to go, but operations were requested,
	 * and notify was enabled.
	 */
  if((m_ptr->IO_ENDPT & SEL_NOTIFY) && ops && !ready_ops) {
  	logdevices[d].log_selected |= ops;
  	logdevices[d].log_select_proc = m_ptr->m_source;
#if LOG_DEBUG
  	printf("log setting selector.\n");
#endif
  }

#if LOG_DEBUG
  printf("log returning ops %d\n", ready_ops);
#endif

  return(ready_ops);
}
