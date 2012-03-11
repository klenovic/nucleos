/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
type.h

Copyright 1995 Philip Homburg
*/

#ifndef INET_TYPE_H
#define INET_TYPE_H

typedef struct acc *(*get_userdata_t)(int fd, size_t offset, size_t count, int for_ioctl);
typedef int (*put_userdata_t)(int fd, size_t offset, struct acc *data, int for_ioctl);
typedef void (*put_pkt_t)(int fd, struct acc *data, size_t datalen);
typedef void (*select_res_t)(int fd, unsigned ops);

#endif /* INET_TYPE_H */
