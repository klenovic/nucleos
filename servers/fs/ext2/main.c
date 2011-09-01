#include "fs.h"
#include <assert.h>
#include <nucleos/unistd.h>
#include <nucleos/signal.h>
#include <nucleos/unistd.h>
#include <nucleos/dmap.h>
#include <nucleos/endpoint.h>
#include <nucleos/vfsif.h>
#include <servers/fs/ext2/buf.h>
#include <servers/fs/ext2/inode.h>
#include "drivers.h"
#include "optset.h"

int env_argc;
char **env_argv;

static struct optset optset_table[] = {
	{ "sb",         OPT_INT,  &opt.block_with_super,    0     },
	{ "orlov",      OPT_BOOL, &opt.use_orlov,           TRUE  },
	{ "oldalloc",   OPT_BOOL, &opt.use_orlov,           FALSE },
	{ "mfsalloc",   OPT_BOOL, &opt.mfsalloc,            TRUE  },
	{ "reserved",   OPT_BOOL, &opt.use_reserved_blocks, TRUE  },
	{ "prealloc",   OPT_BOOL, &opt.use_prealloc,        TRUE  },
	{ "noprealloc", OPT_BOOL, &opt.use_prealloc,        FALSE },
	{ NULL}
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
static void get_work(kipc_msg_t *m_in)
{
	int r, srcok = 0;
	endpoint_t src;

	do {
		/* wait for message */
		if ((r = kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, m_in)) != 0)
			panic("ext2","receive failed", r);

		src = m_in->m_source;

		if (src != VFS_PROC_NR) {
			if(src == PM_PROC_NR) {
				if(is_notify(fs_m_in.m_type))
					srcok = 1;	/* Normal exit request. */
				else
					printk("ext2: unexpected message from PM\n");
			} else
				printk("ext2: unexpected source %d\n", src);
		} else if(src == VFS_PROC_NR) {
			if(unmountdone) 
				printk("ext2: unmounted: unexpected message from FS\n");
			else
				srcok = 1;	/* Normal FS request. */
		} else
			printk("ext2: unexpected source %d\n", src);

	} while(!srcok);

	assert((src == VFS_PROC_NR && !unmountdone) || (src == PM_PROC_NR &&
	       is_notify(fs_m_in.m_type)));
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(endpoint_t who, kipc_msg_t *m_out)
{
	if (kipc_module_call(KIPC_SEND,0, who, m_out) != 0)	/* send the kipc_msg_t */
		printk("ext2(%d) was unable to send reply\n", SELF_E);
}

int main(int argc, char *argv[])
{
/* This is the main routine of this service. The main loop consists of
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
	int error = 0;
	int ind;
	unsigned short test_endian = 1;

	/* Local startup. */
	env_setargs(argc, argv);

	le_CPU = (*(unsigned char *) &test_endian == 0 ? 0 : 1);

	/* Server isn't tested on big endian CPU */
	ASSERT(le_CPU == 1);

	init_server();

	fs_m_in.m_type = KCNR_FS_READY;
	if (kipc_module_call(KIPC_SEND, 0, VFS_PROC_NR, &fs_m_in) != 0) {
		printk("ext2(%d): Error sending login to VFS\n", SELF_E);
		return(-1);
	}

	while(!unmountdone || !exitsignaled) {
		endpoint_t src;

		/* Wait for request message */
		get_work(&fs_m_in);

		src = fs_m_in.m_source;

		/* Exit request? */
		if (src == PM_PROC_NR) {
			exitsignaled = 1;
			fs_sync();
			continue;
		}

		/* Must be a regular VFS request */
		assert(src == VFS_PROC_NR && !unmountdone);

		caller_uid = -1;	/* To trap errors */
		caller_gid = -1;
		req_nr = fs_m_in.m_type;

		if (req_nr < VFS_BASE) {
			fs_m_in.m_type += VFS_BASE;
			req_nr = fs_m_in.m_type;
		}

		ind = req_nr - VFS_BASE;
		if (ind < 0 || ind >= NREQS) {
			printk("ext2: bad request %d (table index %d)\n", req_nr, ind);
			error = -EINVAL;
		} else {
			error = (*fs_call_vec[ind])();
		}

		fs_m_out.m_type = error;
		reply(src, &fs_m_out);
		if (error == 0)
			read_ahead();	/* do block read ahead */
	}

	return 0;
}
