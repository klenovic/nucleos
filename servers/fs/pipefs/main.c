#include "fs.h"
#include <assert.h>
#include <nucleos/dmap.h>
#include <nucleos/endpoint.h>
#include <nucleos/vfsif.h>
#include "buf.h"
#include "inode.h"
#include "drivers.h"

static void init_server(void);
static void get_work(kipc_msg_t *m_in);

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[])
{
/* This is the main routine of this service. The main loop consists of 
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  int error, ind;
  kipc_msg_t m;

  /* Initialize the server, then go to work. */
  init_server();	

  while(!exitsignaled || busy) {
	endpoint_t src;

	/* Wait for request message. */
	get_work(&fs_m_in);
	
	src = fs_m_in.m_source;
	error = 0;
	caller_uid = -1;	/* To trap errors */
	caller_gid = -1;

	if (src == PM_PROC_NR) continue; /* Exit signal */
	assert(src == VFS_PROC_NR); /* Otherwise this must be VFS talking */
	req_nr = fs_m_in.m_type;
	if (req_nr < VFS_BASE) {
		fs_m_in.m_type += VFS_BASE;
		req_nr = fs_m_in.m_type;
	}
	ind = req_nr - VFS_BASE;

	if (ind < 0 || ind >= NREQS) {
		printk("pipefs: bad request %d\n", req_nr); 
		printk("ind = %d\n", ind);
		error = -EINVAL; 
	} else {
		error = (*fs_call_vec[ind])();
	}

	fs_m_out.m_type = error; 
	reply(src, &fs_m_out);
      
  }
  return(0);
}


/*===========================================================================*
 *				init_server                                  *
 *===========================================================================*/
static void init_server(void)
{
  int i;

  /* Initialize main loop parameters. */
  exitsignaled = 0;	/* No exit request seen yet. */
  busy = 0;		/* Server is not 'busy' (i.e., inodes in use). */

  /* Init inode table */
  for (i = 0; i < NR_INODES; ++i) {
	inode[i].i_count = 0;
	cch[i] = 0;
  }
	
  init_inode_cache();

  /* Init driver mapping */
  for (i = 0; i < NR_DEVICES; ++i) 
	driver_endpoints[i].driver_e = ENDPT_NONE;
	
  SELF_E = getprocnr();
  buf_pool();
}


/*===========================================================================*
 *				get_work				     *
 *===========================================================================*/
static void get_work(m_in)
kipc_msg_t *m_in;				/* pointer to message */
{
  int r, srcok = 0;
  endpoint_t src;

  do {
	if ((r = kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, m_in)) != 0) 	/* wait for message */
		panic("PFS","receive failed", r);
	src = fs_m_in.m_source;

	if (src != VFS_PROC_NR) {
		if(src == PM_PROC_NR) {
			if(is_notify(fs_m_in.m_type)) {
				exitsignaled = 1; /* Normal exit request. */
				srcok = 1;	
			} else
				printk("PFS: unexpected message from PM\n");
		} else
			printk("PFS: unexpected source %d\n", src);
	} else if(src == VFS_PROC_NR) {
		srcok = 1;		/* Normal FS request. */
	} else
		printk("PFS: unexpected source %d\n", src);
  } while(!srcok);

   assert( src == VFS_PROC_NR || 
	  (src == PM_PROC_NR && is_notify(fs_m_in.m_type))
	 );
}


/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
void reply(who, m_out)
int who;	
kipc_msg_t *m_out;                       	/* report result */
{
  if (kipc_module_call(KIPC_SEND, 0, who, m_out) != 0)    /* send the message */
	printk("PFS(%d) was unable to send reply\n", SELF_E);
}

