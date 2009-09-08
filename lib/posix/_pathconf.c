/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* POSIX pathconf (Sec. 5.7.1) 		Author: Andy Tanenbaum */

#include <nucleos/lib.h>
#include <nucleos/fcntl.h>
#include <nucleos/errno.h>
#include <nucleos/unistd.h>

/**
 * @brief pathconf
 * @param path  name of file being interrogated
 * @param name  property being inspected
 */
long pathconf(const char *path, int name)
{
/* POSIX allows some of the values in <nucleos/limits.h> to be increased at
 * run time.  The pathconf and fpathconf functions allow these values
 * to be checked at run time.  MINIX does not use this facility.
 * The run-time limits are those given in <nucleos/limits.h>.
 */

  int fd;
  long val;

  if ( (fd = open(path, O_RDONLY)) < 0) return(-1L);
  val = fpathconf(fd, name);
  close(fd);
  return(val);
}
