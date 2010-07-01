/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_SELECT_H
#define __SERVERS_VFS_SELECT_H

/* return codes for select_request_* and select_cancel_* */
#define SEL_OK		0	/* ready */
#define SEL_ERROR	1	/* failed */
#define SEL_DEFERRED	2	/* request is sent to driver */

#endif /* __SERVERS_VFS_SELECT_H */
