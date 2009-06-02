/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>

/* uint32_t strlen(const char* s) __attribute__((cdecl)) */
BEGIN_FUNC(strlen,P2ALIGN_16,0x90)
  pushl %edi
  xorl %eax, %eax    // '\0' we are looking for
  movl 8(%esp), %edi // destination
  orl $-1, %ecx      // set max. value (for repne
                     //  termination condition)
  cld
  repne scasb
  notl %ecx          // get the value
  decl %ecx
  movl %ecx, %eax
  popl %edi
  retl
END_FUNC(strlen)
