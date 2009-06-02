/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
server/ip/gen/psip_hdr.h
*/

#ifndef __SERVER__IP__GEN__PSIP_HDR_H__
#define __SERVER__IP__GEN__PSIP_HDR_H__

typedef struct psip_io_hdr
{
	u8_t pih_flags;
	u8_t pih_dummy[3];
	u32_t pih_nexthop;
} psip_io_hdr_t;

#define PF_LOC_REM_MASK	1
#define PF_LOC2REM		0
#define PF_REM2LOC		1

#endif /* __SERVER__IP__GEN__PSIP_HDR_H__ */

/*
 * $PchId: psip_hdr.h,v 1.3 2001/02/19 07:35:38 philip Exp $
 */
