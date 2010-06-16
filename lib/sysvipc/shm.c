#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/ipc.h>
#include <nucleos/endpoint.h>
#include <nucleos/sysutil.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <servers/rs/rs.h>

#include <nucleos/lib.h>
#include <nucleos/types.h>
#include <nucleos/ipc.h>
#include <nucleos/shm.h>
#include <nucleos/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <nucleos/errno.h>

static int get_ipc_endpt(endpoint_t *pt)
{
	return minix_rs_lookup("ipc", pt);
}

/* Shared memory control operation. */
int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	kipc_msg_t m;
	endpoint_t ipc_pt;
	int r;

	if (get_ipc_endpt(&ipc_pt) != 0) {
		errno = -ENOSYS;
		return -1;
	}

	m.SHMCTL_ID = shmid;
	m.SHMCTL_CMD = cmd;
	m.SHMCTL_BUF = (long) buf;

	r = ktaskcall(ipc_pt, IPC_SHMCTL, &m);
	if ((cmd == IPC_INFO || cmd == SHM_INFO || cmd == SHM_STAT)
		&& (r == 0))
		return m.SHMCTL_RET;
	return r;
}

/* Get shared memory segment. */
int shmget(key_t key, size_t size, int shmflg)
{
	kipc_msg_t m;
	endpoint_t ipc_pt;
	int r;

	if (get_ipc_endpt(&ipc_pt) != 0) {
		errno = -ENOSYS;
		return -1;
	}

	m.SHMGET_KEY = key;
	m.SHMGET_SIZE = size;
	m.SHMGET_FLAG = shmflg;

	r = ktaskcall(ipc_pt, IPC_SHMGET, &m);
	if (r != 0)
		return r;
	return m.SHMGET_RETID;
}

/* Attach shared memory segment. */
void *shmat(int shmid, const void *shmaddr, int shmflg)
{
	kipc_msg_t m;
	endpoint_t ipc_pt;
	int r;

	if (get_ipc_endpt(&ipc_pt) != 0) {
		errno = -ENOSYS;
		return NULL;
	}

	m.SHMAT_ID = shmid;
	m.SHMAT_ADDR = (long) shmaddr;
	m.SHMAT_FLAG = shmflg;

	r = ktaskcall(ipc_pt, IPC_SHMAT, &m);
	if (r != 0)
		return (void *) -1;
	return (void *) m.SHMAT_RETADDR;
}

/* Deattach shared memory segment. */
int shmdt(const void *shmaddr)
{
	kipc_msg_t m;
	endpoint_t ipc_pt;

	if (get_ipc_endpt(&ipc_pt) != 0) {
		errno = -ENOSYS;
		return -1;
	}

	m.SHMDT_ADDR = (long) shmaddr;

	return ktaskcall(ipc_pt, IPC_SHMDT, &m);
}
