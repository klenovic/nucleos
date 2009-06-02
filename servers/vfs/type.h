/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#if 1
extern struct dmap {
  int _PROTOTYPE ((*dmap_opcl), (int, Dev_t, int, int) );
  int _PROTOTYPE ((*dmap_io), (int, message *) );
  int dmap_driver;
  int dmap_flags;
} dmap[];
#endif

