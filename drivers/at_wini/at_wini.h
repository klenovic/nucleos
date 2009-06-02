/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/drivers.h>
#include <nucleos/driver.h>
#include <nucleos/drvlib.h>

_PROTOTYPE(int main, (int argc, char *argv[]));

#define VERBOSE		   0	/* display identify messages during boot */
#define ENABLE_ATAPI	   1	/* add ATAPI cd-rom support to driver */
