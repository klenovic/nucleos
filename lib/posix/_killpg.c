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
#include <nucleos/signal.h>

/**
 * @brief killpg
 * @param pgrp  which process group is to be sent the signal
 * @param sig  signal number
 */
int killpg(int pgrp, int sig)
{
  return kill(-pgrp, sig);
}
