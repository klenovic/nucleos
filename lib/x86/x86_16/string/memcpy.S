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

/* void* memcpy(void* dst, const void* src, uint32_t n) __attribute__((cdecl)) */
BEGIN_FUNC(memcpy,P2ALIGN_16,0x90)
  pushl %esi
  pushl %edi
  movl 12(%esp), %edx // destination
  movl 20(%esp), %ecx // count
  testl %ecx, %ecx
  je 1f
  movl %edx, %edi
  movl 16(%esp), %esi // source
  shrl $2, %ecx  // copy per 4-byte
  cld
  rep movsl
  movl 20(%esp), %ecx // count
  andl $3, %ecx
  rep movsb       // copy per 1-byte
1:
  movl %edx, %eax // return value
  popl %edi
  popl %esi
  retl
END_FUNC(memcpy)
