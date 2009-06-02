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
sys/vm.h
*/

/* MIOCMAP */
struct mapreq
{
	void *base;
	size_t size;
	off_t offset;
	int readonly;
};

/* used in ioctl to tty for mapvm map and unmap request. */
struct mapreqvm
{
	int	flags;		/* reserved, must be 0 */
	off_t	phys_offset;	
	size_t	size;
	int	readonly;
	char	reserved[36];	/* reserved, must be 0 */
	void	*vaddr;		
	void	*vaddr_ret;	
};

