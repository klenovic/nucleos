#define VERBOSE 0

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

#include <nucleos/errno.h>
#include <nucleos/string.h>
#include <env.h>
#include <stdio.h>

#include <servers/vm/glo.h>
#include <servers/vm/proto.h>
#include <servers/vm/util.h>

/*===========================================================================*
 *				do_rs_set_priv				     *
 *===========================================================================*/
int do_rs_set_priv(message *m)
{
	int r, n, nr;
	struct vmproc *vmp;

	nr = m->VM_RS_NR;

	if ((r = vm_isokendpt(nr, &n)) != 0) {
		printf("do_rs_set_priv: message from strange source %d\n", nr);
		return -EINVAL;
	}

	vmp = &vmproc[n];

	if (m->VM_RS_BUF) {
		r = sys_datacopy(m->m_source, (vir_bytes) m->VM_RS_BUF,
				 SELF, (vir_bytes) vmp->vm_call_priv_mask,
				 sizeof(vmp->vm_call_priv_mask));
		if (r != 0)
			return r;
	}

	return 0;
}

