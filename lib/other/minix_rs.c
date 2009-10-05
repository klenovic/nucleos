#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/ipc.h>
#include <nucleos/endpoint.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <servers/ds/ds.h>
#include <nucleos/errno.h>
#include <nucleos/string.h>

#include <stdio.h>
#include <stdlib.h>

int minix_rs_lookup(const char *name, endpoint_t *value)
{
	int r;
	message m;
	size_t len_key;

	len_key = strlen(name)+1;

	m.RS_NAME = name;
	m.RS_NAME_LEN = len_key;

	r = _taskcall(RS_PROC_NR, RS_LOOKUP, &m);

	if(r == 0) {
		*value = m.RS_ENDPOINT;
	}

	return r;
}
