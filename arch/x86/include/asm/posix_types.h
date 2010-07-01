/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_POSIX_TYPES_H
#define __ASM_X86_POSIX_TYPES_H

#if defined(__KERNEL__) || defined(__UKERNEL__)

# ifdef CONFIG_X86_32
#  include "posix_types_32.h"
# else
#  include "posix_types_64.h"
# endif

#else

/* Note that __i386__ is defined by gcc */
# ifdef __i386__
#  include "posix_types_32.h"
# else
#  include "posix_types_64.h"
# endif

#endif

#endif /* __ASM_X86_POSIX_TYPES_H */
