#include "fs.h"
#include <nucleos/com.h>
#include <nucleos/endpoint.h>
#include <nucleos/safecopies.h>
#include <nucleos/u64.h>
#include <nucleos/string.h>
#include <servers/ext2/inode.h>
#include <servers/ext2/super.h>
#include <servers/ext2/const.h>
#include "drivers.h"

#include <nucleos/vfsif.h>

static int safe_io_conversion(endpoint_t driver,cp_grant_id_t *gid, int *op,
		       cp_grant_id_t *gids, endpoint_t *io_ept, void **buffer,
		       int *vec_grants, size_t bytes);
static void safe_io_cleanup(cp_grant_id_t, cp_grant_id_t *, int);
static int gen_opcl(endpoint_t driver_e, int op, dev_t dev, endpoint_t proc_e, int flags);
static int gen_io(endpoint_t task_nr, kipc_msg_t *mess_ptr);


/*===========================================================================*
 *				fs_new_driver   			     *
 *===========================================================================*/
int fs_new_driver(void)
{
 /* New driver endpoint for this device */
  dev_t dev;
  dev = (dev_t) fs_m_in.REQ_DEV;
  driver_endpoints[major(dev)].driver_e = (endpoint_t) fs_m_in.REQ_DRIVER_E;
  return(0);
}


/*===========================================================================*
 *				safe_io_conversion			     *
 *===========================================================================*/
static int safe_io_conversion(driver, gid, op, gids, io_ept, buffer,
			       vec_grants, bytes)
endpoint_t driver;
cp_grant_id_t *gid;
int *op;
cp_grant_id_t *gids;
endpoint_t *io_ept;
void **buffer;
int *vec_grants;
size_t bytes;
{
  unsigned int j;
  int access;
  iovec_t *v;
  static iovec_t *new_iovec;

  STATICINIT(new_iovec, NR_IOREQS);

  /* Number of grants allocated in vector I/O. */
  *vec_grants = 0;

  /* Driver can handle it - change request to a safe one. */

  *gid = GRANT_INVALID;

  switch(*op) {
	case MFS_DEV_READ:
	case MFS_DEV_WRITE:
	  /* Change to safe op. */
	  *op = *op == MFS_DEV_READ ? DEV_READ_S : DEV_WRITE_S;
	  *gid = cpf_grant_direct(driver, (vir_bytes) *buffer, bytes,
				  *op == DEV_READ_S ? CPF_WRITE : CPF_READ);
	  if(*gid == GRANT_INVALID) {
		panic("EXT2","cpf_grant_magic of buffer failed",NO_NUM);
	  }

	  break;
	case MFS_DEV_GATHER:
	case MFS_DEV_SCATTER:
	  /* Change to safe op. */
	  *op = *op == MFS_DEV_GATHER ? DEV_GATHER_S : DEV_SCATTER_S;

	  /* Grant access to my new i/o vector. */
	  *gid = cpf_grant_direct(driver, (vir_bytes) new_iovec,
				  bytes * sizeof(iovec_t), CPF_READ|CPF_WRITE);
	  if(*gid == GRANT_INVALID) {
		panic("EXT2","cpf_grant_direct of vector failed",NO_NUM);
	  }

	  v = (iovec_t *) *buffer;

	  /* Grant access to i/o buffers. */
	  for(j = 0; j < bytes; j++) {
		if(j >= NR_IOREQS)
			panic("EXT2","vec too big: %u", bytes);
		access = (*op == DEV_GATHER_S) ? CPF_WRITE : CPF_READ;
		new_iovec[j].iov_addr = gids[j] =
			cpf_grant_direct(driver, (vir_bytes) v[j].iov_addr,
					 (size_t) v[j].iov_size, access);

		if(!GRANT_VALID(gids[j])) {
			panic("EXT2","ext2: grant to iovec buf failed",NO_NUM);
		}
		new_iovec[j].iov_size = v[j].iov_size;
		(*vec_grants)++;
	  }

	  /* Set user's vector to the new one. */
	  *buffer = new_iovec;
	  break;
	default:
	  panic("EXT2","Illegal operation %d\n", *op);
	  break;
  }

  /* If we have converted to a safe operation, I/O
   * endpoint becomes FS if it wasn't already.
   */
  if(GRANT_VALID(*gid)) {
	*io_ept = SELF_E;
	return 1;
  }

  /* Not converted to a safe operation (because there is no
   * copying involved in this operation).
   */
  return 0;
}

/*===========================================================================*
 *			safe_io_cleanup					     *
 *===========================================================================*/
static void safe_io_cleanup(gid, gids, gids_size)
cp_grant_id_t gid;
cp_grant_id_t *gids;
int gids_size;
{
/* Free resources (specifically, grants) allocated by safe_io_conversion(). */
  int j;

  (void) cpf_revoke(gid);

  for(j = 0; j < gids_size; j++)
	(void) cpf_revoke(gids[j]);

  return;
}

/*===========================================================================*
 *			block_dev_io					     *
 *===========================================================================*/
int block_dev_io(
  int op,			/* MFS_DEV_READ, MFS_DEV_WRITE, etc. */
  dev_t dev,			/* major-minor device number */
  endpoint_t proc_e,		/* in whose address space is buf? */
  void *buffer,			/* virtual address of the buffer */
  u64_t pos,			/* byte position */
  size_t bytes			/* how many bytes to transfer */
)
{
/* Read or write from a device.  The parameter 'dev' tells which one. */
  int r, safe;
  kipc_msg_t m;
  cp_grant_id_t gid = GRANT_INVALID;
  int vec_grants;
  int op_used;
  void *buf_used;
  static cp_grant_id_t *gids;
  endpoint_t driver_e;

  STATICINIT(gids, NR_IOREQS);

  /* Determine driver endpoint for this device */
  driver_e = driver_endpoints[major(dev)].driver_e;

  /* See if driver is roughly valid. */
  if (driver_e == ENDPT_NONE) {
	printk("ext2(%d) block_dev_io: no driver for dev %x\n", SELF_E, dev);
	return(-EDSTDIED);
  }

  /* The io vector copying relies on this I/O being for FS itself. */
  if(proc_e != SELF_E) {
	printk("ext2(%d) doing block_dev_io for non-self %d\n", SELF_E, proc_e);
	panic("EXT2","doing block_dev_io for non-self: %d", proc_e);
  }

  /* By default, these are right. */
  m.IO_ENDPT = proc_e;
  m.ADDRESS  = buffer;
  buf_used = buffer;

  /* Convert parameters to 'safe mode'. */
  op_used = op;
  safe = safe_io_conversion(driver_e, &gid, &op_used, gids, &m.IO_ENDPT,
			    &buf_used, &vec_grants, bytes);

  /* Set up rest of the message. */
  if (safe) m.IO_GRANT = (char *) gid;

  m.m_type   = op_used;
  m.DEVICE   = minor(dev);
  m.POSITION = ex64lo(pos);
  m.COUNT    = bytes;
  m.HIGHPOS  = ex64hi(pos);

  /* Call the task. */
  r = kipc_module_call(KIPC_SENDREC,0, driver_e, &m);
  if(r == 0 && m.REP_STATUS == -ERESTART) r = -EDSTDIED;

  /* As block I/O never SUSPENDs, safe cleanup must be done whether
   * the I/O succeeded or not. */
  if (safe) safe_io_cleanup(gid, gids, vec_grants);

  /* RECOVERY:
   * - send back dead driver number
   * - VFS unmaps it, waits for new driver
   * - VFS sends the new driver endp for the FS proc and the request again
   */
  if (r != 0) {
	if (r == -EDEADSRCDST || r == -EDSTDIED) {
		printk("ext2(%d) dead driver %d\n", SELF_E, driver_e);
		driver_endpoints[major(dev)].driver_e = ENDPT_NONE;
		return(r);
	} else if (r == -ELOCKED) {
		printk("ext2(%d) -ELOCKED talking to %d\n", SELF_E, driver_e);
		return(r);
	} else
		panic("EXT2", "call_task: can't send/receive: %d", r);
  } else {
	/* Did the process we did the sendrec() for get a result? */
	if (m.REP_ENDPT != proc_e) {
		printk("ext2(%d) strange device reply from %d, type = %d, proc "
		       "= %d (not %d) (2) ignored\n", SELF_E, m.m_source,
		       m.m_type, proc_e, m.REP_ENDPT);
		r = -EIO;
	}
  }

  /* Task has completed.  See if call completed. */
  if (m.REP_STATUS == SUSPEND) {
	panic("EXT2","ext2 block_dev_io: driver returned SUSPEND",NO_NUM);
  }

  if(buffer != buf_used && r == 0) {
	memcpy(buffer, buf_used, bytes * sizeof(iovec_t));
  }

  return(m.REP_STATUS);
}

/*===========================================================================*
 *				dev_open				     *
 *===========================================================================*/
int dev_open(
  endpoint_t driver_e,
  dev_t dev,			/* device to open */
  endpoint_t proc_e,		/* process to open for */
  int flags			/* mode bits and flags */
)
{
  int major, r;

  /* Determine the major device number call the device class specific
   * open/close routine.  (This is the only routine that must check the
   * device number for being in range.  All others can trust this check.)
   */
  major = major(dev);
  if (major >= NR_DEVICES) {
	printk("Major device number %d not in range\n", major(dev));
	return(-EIO);
  }
  r = gen_opcl(driver_e, DEV_OPEN, dev, proc_e, flags);
  if (r == SUSPEND) panic("EXT2","suspend on open from",NO_NUM);
  return(r);
}


/*===========================================================================*
 *				dev_close				     *
 *===========================================================================*/
void dev_close(
  endpoint_t driver_e,
  dev_t dev			/* device to close */
)
{
  (void) gen_opcl(driver_e, DEV_CLOSE, dev, 0, 0);
}


/*===========================================================================*
 *				gen_opcl				     *
 *===========================================================================*/
static int gen_opcl(
  endpoint_t driver_e,
  int op,			/* operation, DEV_OPEN or DEV_CLOSE */
  dev_t dev,			/* device to open or close */
  endpoint_t proc_e,		/* process to open/close for */
  int flags			/* mode bits and flags */
)
{
/* Called from the dmap struct in table.c on opens & closes of special files.*/
  kipc_msg_t dev_mess;

  dev_mess.m_type   = op;
  dev_mess.DEVICE   = minor(dev);
  dev_mess.IO_ENDPT = proc_e;
  dev_mess.COUNT    = flags;

  /* Call the task. */
  (void) gen_io(driver_e, &dev_mess);

  return(dev_mess.REP_STATUS);
}


/*===========================================================================*
 *				gen_io					     *
 *===========================================================================*/
static int gen_io(
  endpoint_t task_nr,		/* which task to call */
  kipc_msg_t *mess_ptr		/* pointer to message for task */
)
{
/* All file system I/O ultimately comes down to I/O on major/minor device
 * pairs.  These lead to calls on the following routines via the dmap table.
 */

  int r, proc_e;

  proc_e = mess_ptr->IO_ENDPT;

  r = kipc_module_call(KIPC_SENDREC, 0, task_nr, mess_ptr);
  if(r == 0 && mess_ptr->REP_STATUS == -ERESTART)
	r = -EDSTDIED;

  if (r != 0) {
	if (r == -EDEADSRCDST || r == -EDSTDIED) {
		printk("fs: dead driver %d\n", task_nr);
		panic("EXT2", "should handle crashed drivers", NO_NUM);
		return(r);
	}
	if (r == -ELOCKED) {
		printk("fs: -ELOCKED talking to %d\n", task_nr);
		return(r);
	}
	panic("EXT2","call_task: can't send/receive: %d", r);
  }

   /* Did the process we did the sendrec() for get a result? */
  if (mess_ptr->REP_ENDPT != proc_e) {
	printk("fs: strange device reply from %d, type = %d, proc = %d (not "
	       "%d) (2) ignored\n", mess_ptr->m_source, mess_ptr->m_type,
		proc_e,
		mess_ptr->REP_ENDPT);
	return(-EIO);
  }

  return(0);
}
