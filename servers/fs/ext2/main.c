#include "fs.h"
#include <assert.h>
#include <nucleos/unistd.h>
#include <nucleos/signal.h>
#include <nucleos/unistd.h>
#include <nucleos/dmap.h>
#include <nucleos/endpoint.h>
#include <nucleos/vfsif.h>
#include "buf.h"
#include "inode.h"
#include "drivers.h"
#include "optset.h"


/* Declare some local functions. */
static void get_work(kipc_msg_t *m_in);
static void cch_check(void);
static void reply(endpoint_t who, kipc_msg_t *m_out);

int env_argc;
char **env_argv;

static struct optset optset_table[] = {
  { "sb",		OPT_INT,    &opt.block_with_super,	0	},
  { "orlov",		OPT_BOOL,   &opt.use_orlov,		TRUE    },
  { "oldalloc",		OPT_BOOL,   &opt.use_orlov,		FALSE   },
  { "mfsalloc",		OPT_BOOL,   &opt.mfsalloc,		TRUE    },
  { "reserved",		OPT_BOOL,   &opt.use_reserved_blocks,	TRUE    },
  { "prealloc",		OPT_BOOL,   &opt.use_prealloc, 		TRUE	},
  { "noprealloc",	OPT_BOOL,   &opt.use_prealloc, 		FALSE	},
  { NULL								}
};


static int init_server(void)
{
/* Initialize the Nucleos file server. */
  int i;

  /* Defaults */
  opt.use_orlov = TRUE;
  opt.mfsalloc = FALSE;
  opt.use_reserved_blocks = FALSE;
  opt.block_with_super = 0;
  opt.use_prealloc = FALSE;

  /* If we have been given an options string, parse options from there. */
  for (i = 1; i < env_argc - 1; i++)
	if (!strcmp(env_argv[i], "-o"))
		optset_parse(optset_table, env_argv[++i]);

  may_use_vmcache = 1;

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
  buf_pool(NR_BUFS);
  fs_block_size = _MIN_BLOCK_SIZE;

  return(0);
}

/*===========================================================================*
 *				get_work				     *
 *===========================================================================*/
static void get_work(m_in)
kipc_msg_t *m_in;				/* pointer to kipc_msg_t */
{
  int r, srcok = 0;
  endpoint_t src;

  do {
	if ((r = kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, m_in)) != 0)      /* wait for message */
		panic("EXT2","receive failed", r);

	src = m_in->m_source;

	if(src == VFS_PROC_NR) {
		if(unmountdone)
			printk("ext2: unmounted: unexpected kipc_msg_t from FS\n");
		else
			srcok = 1;		/* Normal FS request. */

	} else
		printk("ext2: unexpected source %d\n", src);
  } while(!srcok);

   assert((src == VFS_PROC_NR && !unmountdone));
}


/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(
  endpoint_t who,
  kipc_msg_t *m_out                       	/* report result */
)
{
  if (0 != kipc_module_call(KIPC_SEND,0, who, m_out))    /* send the kipc_msg_t */
	printk("ext2(%d) was unable to send reply\n", SELF_E);
}


/*===========================================================================*
 *				cch_check				     *
 *===========================================================================*/
static void cch_check(void)
{
  int i;

  for (i = 0; i < NR_INODES; ++i) {
	if (inode[i].i_count != cch[i] && req_nr != REQ_GETNODE &&
	    req_nr != REQ_PUTNODE && req_nr != REQ_READSUPER &&
	    req_nr != REQ_MOUNTPOINT && req_nr != REQ_UNMOUNT &&
	    req_nr != REQ_SYNC && req_nr != REQ_LOOKUP) {
		printk("ext2(%d) inode(%ul) cc: %d req_nr: %d\n", SELF_E,
			inode[i].i_num, inode[i].i_count - cch[i], req_nr);
	}
	cch[i] = inode[i].i_count;
  }
}

int main(int argc, char *argv[])
{
/* This is the main routine of this service. The main loop consists of
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  int error, ind;
  unsigned short test_endian = 1;

	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  /* SEF local startup. */
  env_setargs(argc, argv);
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  le_CPU = (*(unsigned char *) &test_endian == 0 ? 0 : 1);
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  /* Server isn't tested on big endian CPU */
  ASSERT(le_CPU == 1);
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
	init_server();
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  fs_m_in.m_type = KCNR_FS_READY;
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  if (kipc_module_call(KIPC_SEND, 0, VFS_PROC_NR, &fs_m_in) != 0) {
        printk("EXT2(%d): Error sending login to VFS\n", SELF_E);
        return(-1);
  }
	printk("%d:Starting ext2(%d)...\n", __LINE__,SELF_E);
  while(!unmountdone || !exitsignaled) {
	endpoint_t src;

	/* Wait for request kipc_msg_t. */
	get_work(&fs_m_in);

	src = fs_m_in.m_source;
	if (src == PM_PROC_NR) {
		exitsignaled = 1;
		fs_sync();
		continue;
	}

	error = 0;
	caller_uid = -1;	/* To trap errors */
	caller_gid = -1;
	req_nr = fs_m_in.m_type;

	if (req_nr < VFS_BASE) {
		fs_m_in.m_type += VFS_BASE;
		req_nr = fs_m_in.m_type;
	}
	ind = req_nr - VFS_BASE;

	if (ind < 0 || ind >= NREQS) {
		printk("mfs: bad request %d\n", req_nr);
		printk("ind = %d\n", ind);
		error = -EINVAL;
	} else {
		error = (*fs_call_vec[ind])();
		/*cch_check();*/
	}

	fs_m_out.m_type = error;
	reply(src, &fs_m_out);

	if (error == 0)
		read_ahead(); /* do block read ahead */
  }

	return 0;
}
