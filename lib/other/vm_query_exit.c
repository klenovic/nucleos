#include <nucleos/lib.h>
#include <nucleos/unistd.h>

/* return -1, when the query itself or the processing of query has errors.
 * return 1, when there are more processes waiting to be queried.
 * return 0, when there are no more processes.
 * note that for the return value of 0 and 1, the 'endpt' is set accordingly.
 */
int vm_query_exit(int *endpt)
{
	message m;
	int r;

	r = ktaskcall(VM_PROC_NR, VM_QUERY_EXIT, &m);
	if (r != 0)
		return -1;
	if (endpt == NULL)
		return -1;

	*endpt = m.VM_QUERY_RET_PT;
	return (m.VM_QUERY_IS_MORE ? 1 : 0);
}
