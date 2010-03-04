

/* This file contains the main directory for the server. It waits for a 
 * request and then send a response. */

#include "inc.h"
#include <nucleos/vfsif.h>
#include <nucleos/kipc.h>
#include "buf.h"
#include "const.h"
#include "glo.h"

/* Declare some local functions. */
static void init_server(void);
static void get_work(message *m_in);

message fs_m_in;	/* contains the input message of the request */
message fs_m_out;	/* contains the output message of the request */

uid_t caller_uid;
gid_t caller_gid;

int req_nr;	/* request number to the server */
int SELF_E;	/* process number */
int use_getuptime2; /* Should be removed togetherwith boottime */
int FS_STATE;

struct driver_endpoints driver_endpoints[NR_DEVICES];
struct buf buf[NR_BUFS];

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(void) {
	int who_e, ind, error;
	message m;

	/* Initialize the server, then go to work. */
	init_server();

	fs_m_in.m_type = KCNR_FS_READY;

	if (kipc_send(FS_PROC_NR, &fs_m_in) != 0) {
		printf("ISOFS (%d): Error sending login to VFS\n", SELF_E);
		return -1;
	}

	for (;;) {

		/* Wait for request message. */
		get_work(&fs_m_in);
		error = 0;

		caller_uid = -1;	/* To trap errors */
		caller_gid = -1;

		who_e = fs_m_in.m_source;	/* source of the request */

		if (who_e != FS_PROC_NR) { /* If the message is not for us just 
					    * continue */
			continue;
		}

		req_nr = fs_m_in.m_type;

		if (req_nr < VFS_BASE) {
			fs_m_in.m_type += VFS_BASE;
			req_nr = fs_m_in.m_type;
		}

		ind = req_nr-VFS_BASE;

		if (ind < 0 || ind >= NREQS) {
			error = -EINVAL; 
		} else
			error = (*fs_call_vec[ind])(); /* Process the request calling
						* the appropriate function. */

		fs_m_out.m_type = error; 
		reply(who_e, &fs_m_out); 	/* returns the response to VFS */
	}
}

/*===========================================================================*
 *				init_server                                  *
 *===========================================================================*/
static void init_server(void)
{
   int i;

   /* Init driver mapping */
   for (i = 0; i < NR_DEVICES; ++i) 
       driver_endpoints[i].driver_e = NONE;
   /* SELF_E will contain the id of this process */
   SELF_E = getprocnr();
/*    hash_init(); */			/* Init the table with the ids */
   setenv("TZ","",1);		/* Used to calculate the time */
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
static void get_work(m_in)
message *m_in;				/* pointer to message */
{
  int s;				/* receive status */
  if (0 != (s = kipc_receive(ANY, m_in))) 	/* wait for message */
    panic("ISOFS","kipc_receive failed", s);
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
void reply(who, m_out)
int who;	
message *m_out;                       	/* report result */
{
  if (0 != kipc_send(who, m_out))    /* send the message */
    printf("ISOFS(%d) was unable to send reply\n", SELF_E);
}
