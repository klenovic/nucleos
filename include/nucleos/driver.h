/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Types and constants shared between the generic and device dependent
 * device driver code.
 */
/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/com.h>
#include <nucleos/unistd.h>
#include <nucleos/types.h>
#include <nucleos/const.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

#include <nucleos/string.h>
#include <nucleos/limits.h>
#include <nucleos/stddef.h>
#include <nucleos/errno.h>

#include <nucleos/partition.h>
#include <nucleos/u64.h>

/* Info about and entry points into the device dependent code. */
struct driver {
  char *(*dr_name)(void);
  int (*dr_open)(struct driver *dp, message *m_ptr);
  int (*dr_close)(struct driver *dp, message *m_ptr);
  int (*dr_ioctl)(struct driver *dp, message *m_ptr);
  struct device *(*dr_prepare)(int device);
  int (*dr_transfer)(int proc_nr, int opcode, u64_t position,
					iovec_t *iov, unsigned nr_req);
  void (*dr_cleanup)(void);
  void (*dr_geometry)(struct partition *entry);
  void (*dr_signal)(struct driver *dp, sigset_t *set);
  void (*dr_alarm)(struct driver *dp, message *m_ptr);
  int (*dr_cancel)(struct driver *dp, message *m_ptr);
  int (*dr_select)(struct driver *dp, message *m_ptr);
  int (*dr_other)(struct driver *dp, message *m_ptr);
  int (*dr_hw_int)(struct driver *dp, message *m_ptr);
};

/* Base and size of a partition in bytes. */
struct device {
  u64_t dv_base;
  u64_t dv_size;
};

#define NIL_DEV		((struct device *) 0)

#define DRIVER_STD	0	/* Use the standard reply protocol */
#define DRIVER_ASYN	1	/* Use the new asynchronous protocol */

/* Functions defined by driver.c: */
void driver_task(struct driver *dr, int type);
char *no_name(void);
int do_nop(struct driver *dp, message *m_ptr);
struct device *nop_prepare(int device);
void nop_cleanup(void);
void nop_task(void);
void nop_signal(struct driver *dp, sigset_t *set);
void nop_alarm(struct driver *dp, message *m_ptr);
int nop_cancel(struct driver *dp, message *m_ptr);
int nop_select(struct driver *dp, message *m_ptr);
int do_diocntl(struct driver *dp, message *m_ptr);
int nop_ioctl(struct driver *dp, message *m_ptr);
int mq_queue(message *m_ptr);
void init_buffer(void);

/* Parameters for the disk drive. */
#define SECTOR_SIZE      512	/* physical sector size in bytes */
#define SECTOR_SHIFT       9	/* for division */
#define SECTOR_MASK      511	/* and remainder */

/* DMA_SECTORS may be increased to speed up DMA based drivers. */
#define DMA_SECTORS	1	/* DMA buffer size (must be >= 1) */

/* Size of the DMA buffer buffer in bytes. */
#define USE_EXTRA_DMA_BUF  0	/* usually not needed */
#define DMA_BUF_SIZE	(DMA_SECTORS * SECTOR_SIZE)

#ifdef CONFIG_X86_32
extern u8_t *tmp_buf;			/* the DMA buffer */
#else
extern u8_t tmp_buf[];			/* the DMA buffer */
#endif
extern phys_bytes tmp_phys;		/* phys address of DMA buffer */
