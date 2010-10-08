#include "inc.h"

int identifier = 0x1234;
endpoint_t who_e;
int call_type;
endpoint_t SELF_E;

struct {
	int type;
	int (*func)(kipc_msg_t *);
	int reply;	/* whether the reply action is passed through */
} ipc_calls[] = {
	{ IPC_SHMGET,	do_shmget,	0 },
	{ IPC_SHMAT,	do_shmat,	0 },
	{ IPC_SHMDT,	do_shmdt,	0 },
	{ IPC_SHMCTL,	do_shmctl,	0 },
	{ IPC_SEMGET,	do_semget,	0 },
	{ IPC_SEMCTL,	do_semctl,	0 },
	{ IPC_SEMOP,	do_semop,	1 },
};

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

int verbose = 0;

int main(int argc, char *argv[])
{
	kipc_msg_t m;

	SELF_E = getprocnr();

	if(verbose)
		printk("IPC: self: %d\n", SELF_E);

	while (TRUE) {
		int r;
		int i;

		if ((r = kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, &m)) != 0)
			printk("IPC receive error %d.\n", r);
		who_e = m.m_source;
		call_type = m.m_type;

		if(verbose)
			printk("IPC: get %d from %d\n", call_type, who_e);

		if (call_type & NOTIFY_MESSAGE) {
			switch (who_e) {
			case PM_PROC_NR:
				/* PM sends a notify() on shutdown,
				 * checkout if there are still IPC keys,
				 * give warning messages.
				 */
				if (!is_sem_nil() || !is_shm_nil())
					printk("IPC: exit with un-clean states.\n");
				break;
			case VM_PROC_NR:
				/* currently, only semaphore needs such information. */
				sem_process_vm_notify();
				break;
			default:
				printk("IPC: ignoring notify() from %d\n",
					who_e);
				break;
			}
			continue;
		}

		/* dispatch messages */
		for (i = 0; i < SIZE(ipc_calls); i++) {
			if (ipc_calls[i].type == call_type) {
				int result;

				result = ipc_calls[i].func(&m);

				if (ipc_calls[i].reply)
					break;

				m.m_type = result;

				if(verbose && result != 0)
					printk("IPC: error for %d: %d\n",
						call_type, result);

				if ((r = kipc_module_call(KIPC_SEND, KIPC_FLG_NONBLOCK, who_e, &m)) != 0)
					printk("IPC send error %d.\n", r);
				break;
			}
		}

		if (i == SIZE(ipc_calls)) {
			/* warn and then ignore */
			printk("IPC unknown call type: %d from %d.\n",
				call_type, who_e);
		}
		update_refcount_and_destroy();
	}

	/* no way to get here */
	return -1;
}

