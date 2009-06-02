/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <a.out> header file describes the format of executable files. */
#ifndef __NUCS_EXEC_H
#define __NUCS_EXEC_H

#include "elf.h"

#define NEXEC 0x0002

/**
 * A SimpleExec describes an executable binary decoded and relocated
 * into memory by the bootloader
 */
typedef struct MNX(exec_elf32) {
  uint32_t  type;
  uint32_t  version;
  uint32_t  offset_next;
  uint32_t  text_pstart;
  uint32_t  text_vstart;
  uint32_t  text_size;
  uint32_t  data_pstart;
  uint32_t  data_vstart;
  uint32_t  data_size;
  uint32_t  bss_pstart;
  uint32_t  bss_vstart;
  uint32_t  bss_size;
  uint32_t  initial_ip;
  uint32_t  symtab_size;
  uint32_t  strtab_size;
  uint32_t  flags;
  uint32_t  label;
  uint32_t  cmdline_offset;
} MNX(exec_elf32_t);
#endif /* __NUCS_EXEC_H */
