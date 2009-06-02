/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __INSTALLBOOT_IMAGE_H
#define __INSTALLBOOT_IMAGE_H

#define IM_NAME_MAX 63

struct MNX(image_header_aout) {
  char name[IM_NAME_MAX + 1]; /* Null terminated. */
  struct MNX(exec) process;
};

struct MNX(image_header_elf32) {
  char name[IM_NAME_MAX + 1]; /* Null terminated. */
  struct MNX(exec_elf32) process;
};

union MNX(image_header) {
  struct MNX(exec) aout;
  struct MNX(exec_elf32) elf;
};

#endif /* __INSTALLBOOT_IMAGE_H */
