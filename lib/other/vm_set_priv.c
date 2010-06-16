#include <nucleos/lib.h>
#include <nucleos/unistd.h>

int vm_set_priv(int nr, void *buf)
{
	kipc_msg_t m;
	m.VM_RS_NR = nr;
	m.VM_RS_BUF = (long) buf;
	return ktaskcall(VM_PROC_NR, VM_RS_SET_PRIV, &m);
}
