
#include <nucleos/syslib.h>

#include <nucleos/vm.h>

/*===========================================================================*
 *                                vm_umap				     *
 *===========================================================================*/
int vm_ctl(int what, int param)
{
    kipc_msg_t m;
    int result;

    m.VCTL_WHAT = what;
    m.VCTL_PARAM = param;
    return ktaskcall(VM_PROC_NR, VM_CTL, &m);
}

