/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* PC (and AT) BIOS structure to hold disk parameters.  Under Minix, it is
 * used mainly for formatting.
 */

#ifndef _DISKPARM_H
#define _DISKPARM_H
struct disk_parameter_s {
  char spec1;
  char spec2;
  char motor_turnoff_sec;
  char sector_size_code;
  char sectors_per_cylinder;
  char gap_length;
  char dtl;
  char gap_length_for_format;
  char fill_byte_for_format;
  char head_settle_msec;
  char motor_start_eigth_sec;
};
#endif /* _DISKPARM_H */
