/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Includes. */
#include <nucleos/drivers.h>
#include <nucleos/driver.h>
#include <nucleos/type.h>
#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/types.h>
#include <nucleos/kipc.h>

/* Constants and types. */

#define LOG_SIZE	(50*1024) 
#define SUSPENDABLE 	      1

struct logdevice {
	char log_buffer[LOG_SIZE];
	int	log_size,	/* no. of bytes in log buffer */
		log_read,	/* read mark */
		log_write;	/* write mark */
#if SUSPENDABLE
	int log_proc_nr,
		log_source,
		log_iosize,
		log_revive_alerted,
		log_status;	/* proc that is blocking on read */
	vir_bytes log_user_vir_g, log_user_vir_offset;
#endif
	int	log_selected, log_select_proc,
		log_select_alerted, log_select_ready_ops;
};

/* Function prototypes. */
int do_new_kmess(endpoint_t from);
int do_diagnostics(message *m, int safe);
void log_append(char *buf, int len);

