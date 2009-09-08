/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>

int _syscall(who, syscallnr, msgptr)
int who;
int syscallnr;
register message *msgptr;
{
  int status;

  msgptr->m_type = syscallnr;
  status = kipc_sendrec(who, msgptr);

  if (status != 0) {
	/* 'sendrec' itself failed. */
	/* XXX - strerror doesn't know all the codes */
	msgptr->m_type = status;
  }

  if (msgptr->m_type < 0) {
	errno = -msgptr->m_type;
	return(-1);
  }

  return(msgptr->m_type);
}
