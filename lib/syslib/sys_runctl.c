#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_runctl			     	     *
 *===========================================================================*/
int sys_runctl(endpoint_t proc_ep, int action, int flags)
{
  kipc_msg_t m;

  m.RC_ENDPT = proc_ep;
  m.RC_ACTION = action;
  m.RC_FLAGS = flags;

  return(ktaskcall(SYSTASK, SYS_RUNCTL, &m));
}
