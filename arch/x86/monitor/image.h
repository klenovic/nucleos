/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*  image.h - Info between installboot and boot. Author: Kees J. Bot
 */
#define IM_NAME_MAX 63

struct image_header {
  char   name[IM_NAME_MAX + 1]; /* Null terminated. */
  struct exec process;
};
