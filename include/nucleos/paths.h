/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_PATHS_H
#define __NUCLEOS_PATHS_H

#define _PATH_DHCPCONF		"/etc/dhcp.conf"
#define _PATH_DHCPPID		"/usr/run/dhcpd.pid"
#define _PATH_DHCPCACHE		"/usr/adm/dhcp.cache"
#define _PATH_DHCPPOOL		"/usr/adm/dhcp.pool"

#define _PATH_WTMP		"/usr/adm/wtmp"
#define _PATH_UTMP		"/etc/utmp"
#define _PATH_LASTLOG		"/usr/adm/lastlog"
#define _PATH_MOTD		"/etc/motd"
#define _PATH_HOSTS		"/etc/hosts"

#define _PATH_DEFTAPE		"/dev/sa0"
#define _PATH_RAMDISK		"/dev/ram"
#define _PATH_TMP		"/tmp"

#define _PATH_BSHELL		"/bin/sh"
#define _PATH_SERVICE		"/bin/service"
#define _PATH_DRIVERS_CONF	"/etc/drivers.conf"
#define _PATH_DEV		"/dev/"

#endif /* __NUCLEOS_PATHS_H */
