#include <nucleos/lib.h>
#include <nucleos/unistd.h>

uid_t getnuid(int proc_nr)
{
  message m;
  m.m1_i1 = proc_nr;		/* search uid for this process */
  if (_syscall(PM_PROC_NR, __NR_getuid, &m) < 0) return ( (uid_t) -1);
  return( (uid_t) m.m2_i2);	/* return search result */
}
