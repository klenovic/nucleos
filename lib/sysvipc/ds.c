#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/config.h>
#include <nucleos/ipc.h>
#include <nucleos/endpoint.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <serves/ds/ds.h>

#include <stdio.h>
#include <stdlib.h>
#include <nucleos/errno.h>
#include <nucleos/string.h>

int mini_ds_retrieve_u32(char *ds_name, u32_t *value)
{
	int r;
	message m;
	size_t len_key;

	len_key = strlen(ds_name)+1;

	m.DS_KEY_GRANT = ds_name;
	m.DS_KEY_LEN = len_key;
	m.DS_FLAGS = DS_TYPE_U32;

	r = _taskcall(DS_PROC_NR, DS_RETRIEVE_LIBC, &m);

	if(r == 0) {
		/* Assign u32 value. */
		*value = m.DS_VAL;
	}

	return r;
}

