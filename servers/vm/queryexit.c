#include <stdio.h>
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <servers/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/safecopies.h>
#include <nucleos/bitmap.h>
#include <nucleos/vm.h>
#include <nucleos/errno.h>
#include <nucleos/string.h>
#include <nucleos/sysutil.h>
#include <servers/vm/glo.h>
#include <servers/vm/proto.h>
#include <servers/vm/util.h>

struct query_exit_struct {
	int avail;
	endpoint_t ep;
};
static struct query_exit_struct array[NR_PROCS];

/*===========================================================================*
 *				do_query_exit				     *
 *===========================================================================*/
int do_query_exit(kipc_msg_t *m)
{
	int i, nr;
	endpoint_t ep;

	for (i = 0; i < NR_PROCS; i++) {
		if (!array[i].avail) {
			array[i].avail = 1;
			ep = array[i].ep;
			array[i].ep = 0;

			break;
		}
	}

	nr = 0;
	for (i = 0; i < NR_PROCS; i++) {
		if (!array[i].avail)
			nr++;
	}
	m->VM_QUERY_RET_PT = ep;
	m->VM_QUERY_IS_MORE = (nr > 0);

	return 0;
}

/*===========================================================================*
 *				do_notify_sig				     *
 *===========================================================================*/
int do_notify_sig(kipc_msg_t *m)
{
	int i, avails = 0;
	endpoint_t ep = m->VM_NOTIFY_SIG_ENDPOINT;
	endpoint_t ipc_ep = m->VM_NOTIFY_SIG_IPC;
	int r;

	for (i = 0; i < NR_PROCS; i++) {
		/* its signal is already here */
		if (!array[i].avail && array[i].ep == ep)
			goto out;
		if (array[i].avail)
			avails++;
	}
	if (!avails) {
		/* no slot for signals, impossible */
		printk("VM: no slot for signals!\n");
		return -ENOMEM;
	}

	for (i = 0; i < NR_PROCS; i++) {
		if (array[i].avail) {
			array[i].avail = 0;
			array[i].ep = ep;

			break;
		}
	}

out:
	/* only care when IPC server starts up,
	 * and bypass the process to be signal is IPC itself.
	 */
	if (ipc_ep != 0 && ep != ipc_ep) {
		r = kipc_module_call(KIPC_NOTIFY, 0, ipc_ep, 0);
		if (r != 0)
			printk("VM: notify IPC error!\n");
	}
	return 0;
}

/*===========================================================================*
 *				init_query_exit				     *
 *===========================================================================*/
void init_query_exit(void)
{
	int i;

	for (i = 0; i < NR_PROCS; i++) {
		array[i].avail = 1;
		array[i].ep = 0;
	}
}
