/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the device dependent part of the drivers for the
 * following special files:
 *     /dev/ram		- RAM disk 
 *     /dev/mem		- absolute memory
 *     /dev/kmem	- kernel virtual memory
 *     /dev/null	- null device (data sink)
 *     /dev/boot	- boot device loaded from boot image 
 *     /dev/zero	- null byte stream generator
 *
 *  Changes:
 *	Apr 29, 2005	added null byte generator  (Jorrit N. Herder)
 *	Apr 09, 2005	added support for boot device  (Jorrit N. Herder)
 *	Jul 26, 2004	moved RAM driver to user-space  (Jorrit N. Herder)
 *	Apr 20, 1992	device dependent/independent split  (Kees J. Bot)
 */
#include <nucleos/kernel.h>
#include <asm/bootparam.h>
#include <nucleos/drivers.h>
#include <nucleos/driver.h>
#include <asm/ioctls.h>
#include <nucleos/sysutil.h>
#include <servers/ds/ds.h>
#include <nucleos/vm.h>
#include <nucleos/mman.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <servers/vm/vm.h>
#include <asm/servers/vm/vm.h>
#include <assert.h>

#ifndef CONFIG_BUILTIN_INITRD
unsigned long initrd_base;
size_t initrd_size;
#else /* CONFIG_BUILTIN_INITRD */
extern unsigned char initrd[];
extern size_t initrd_size;
#endif

/* ramdisks (/dev/ram*) */
#define RAMDISKS     6

#define RAM_DEV_LAST (RAM_DEV_FIRST+RAMDISKS-1)

#define NR_DEVS            (7+RAMDISKS)	/* number of minor devices */

static struct device m_geom[NR_DEVS];  /* base and size of each device */
static vir_bytes m_vaddrs[NR_DEVS];
static int m_device;			/* current device */
static struct kinfo kinfo;		/* kernel information */ 
static struct boot_params boot_params;	/* boot parameters */

extern int errno;			/* error number for PM calls */

static int openct[NR_DEVS];

static char *m_name(void);
static struct device *m_prepare(int device);
static int m_transfer(int proc_nr, int opcode,
			u64_t position, iovec_t *iov, unsigned nr_req);
static int m_do_open(struct driver *dp, kipc_msg_t *m_ptr);
static int m_do_close(struct driver *dp, kipc_msg_t *m_ptr);
static void m_init(void);
static int m_ioctl(struct driver *dp, kipc_msg_t *m_ptr);
static void m_geometry(struct partition *entry);

/* Entry points to this driver. */
static struct driver m_dtab = {
  m_name,	/* current device's name */
  m_do_open,	/* open or mount */
  m_do_close,	/* nothing on a close */
  m_ioctl,	/* specify ram disk geometry */
  m_prepare,	/* prepare for I/O on a given minor device */
  m_transfer,	/* do the I/O */
  nop_cleanup,	/* no need to clean up */
  m_geometry,	/* memory device "geometry" */
  nop_signal,	/* system signals */
  nop_alarm,
  nop_cancel,
  nop_select,
  NULL,
  NULL
};

/* Buffer for the /dev/zero null byte feed. */
#define ZERO_BUF_SIZE 			1024
static char dev_zero[ZERO_BUF_SIZE];

#define click_to_round_k(n) \
	((unsigned) ((((unsigned long) (n) << CLICK_SHIFT) + 512) / 1024))

/*===========================================================================*
 *				   main 				     *
 *===========================================================================*/
int main(void)
{
/* Main program. Initialize the memory driver and start the main loop. */
  struct sigaction sa;

  sa.sa_handler = SIG_MESS;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGTERM,&sa,NULL)<0) panic("MEM","sigaction failed", errno);

  m_init();
  driver_task(&m_dtab, DRIVER_STD);
  return(0);
}

/*===========================================================================*
 *				 m_name					     *
 *===========================================================================*/
static char *m_name()
{
/* Return a name for the current device. */
  static char name[] = "memory";
  return name;  
}

/*===========================================================================*
 *				m_prepare				     *
 *===========================================================================*/
static struct device *m_prepare(device)
int device;
{
/* Prepare for I/O on a device: check if the minor device number is ok. */
  if (device < 0 || device >= NR_DEVS) return(NIL_DEV);
  m_device = device;

  return(&m_geom[device]);
}

/*===========================================================================*
 *				m_transfer				     *
 *===========================================================================*/
static int m_transfer(proc_nr, opcode, pos64, iov, nr_req)
int proc_nr;			/* process doing the request */
int opcode;			/* DEV_GATHER_S or DEV_SCATTER_S */
u64_t pos64;			/* offset on device to read or write */
iovec_t *iov;			/* pointer to read or write request vector */
unsigned nr_req;		/* length of request vector */
{
/* Read or write one the driver's minor devices. */
  unsigned count, left, chunk;
  vir_bytes user_vir, vir_offset = 0;
  struct device *dv;
  unsigned long dv_size;
  int s, r;
  off_t position;
  vir_bytes dev_vaddr;

  /* ZERO_DEV and NULL_DEV are infinite in size. */
  if (m_device != ZERO_DEV && m_device != NULL_DEV && ex64hi(pos64) != 0)
	return 0;	/* Beyond EOF */
  position= cv64ul(pos64);

  /* Get minor device number and check for /dev/null. */
  dv = &m_geom[m_device];
  dv_size = cv64ul(dv->dv_size);
  dev_vaddr = m_vaddrs[m_device];

  while (nr_req > 0) {

	/* How much to transfer and where to / from. */
	count = iov->iov_size;
	user_vir = iov->iov_addr;

	switch (m_device) {

	/* No copying; ignore request. */
	case NULL_DEV:
	    if (opcode == DEV_GATHER_S) return 0;	/* always at EOF */
	    break;

	/* Virtual copying. For RAM disks, kernel memory and internal VFS_PROC_NR. */
	default:
	case KMEM_DEV:
	case RAM_DEV_OLD:
	case IMGRD_DEV:
	    /* Bogus number. */
	    if(m_device < 0 || m_device >= NR_DEVS) {
		    return(-EINVAL);
	    }
  	    if(!dev_vaddr || dev_vaddr == (vir_bytes) MAP_FAILED) {
		printk("MEM: dev %d not initialized\n", m_device);
		return -EIO;
	    }
	    if (position >= dv_size) return 0; 	/* check for EOF */
	    if (position + count > dv_size) count = dv_size - position;
	    if (opcode == DEV_GATHER_S) {	/* copy actual data */
	        r=sys_safecopyto(proc_nr, user_vir, vir_offset,
	  	  dev_vaddr + position, count, D);
	    } else {
	        r=sys_safecopyfrom(proc_nr, user_vir, vir_offset,
	  	  dev_vaddr + position, count, D);
	    }
	    if(r != 0) {
              panic("MEM","I/O copy failed",r);
	    }
	    break;

	/* Physical copying. Only used to access entire memory.
	 * Transfer one 'page window' at a time.
	 */
	case MEM_DEV:
	{
	    u32_t pagestart, page_off;
	    static u32_t pagestart_mapped;
	    static int any_mapped = 0;
	    static char *vaddr;
	    int r;
	    u32_t subcount;
  	    phys_bytes mem_phys;

	    if (position >= dv_size)
		return 0; 	/* check for EOF */
	    if (position + count > dv_size)
		count = dv_size - position;
	    mem_phys = position;

	    page_off = mem_phys % I386_PAGE_SIZE;
	    pagestart = mem_phys - page_off; 

	    /* All memory to the map call has to be page-aligned.
	     * Don't have to map same page over and over.
	     */
	    if(!any_mapped || pagestart_mapped != pagestart) {
	     if(any_mapped) {
		if(vm_unmap_phys(ENDPT_SELF, vaddr, I386_PAGE_SIZE) != 0)
      			panic("MEM","vm_unmap_phys failed",NO_NUM);
		any_mapped = 0;
	     }
	     vaddr = vm_map_phys(ENDPT_SELF, (void *) pagestart, I386_PAGE_SIZE);
	     if(vaddr == MAP_FAILED) 
		r = -ENOMEM;
	     else
		r = 0;
	     if(r != 0) {
		printk("memory: vm_map_phys failed\n");
		return r;
	     }
	     any_mapped = 1;
	     pagestart_mapped = pagestart;
	   }

	    /* how much to be done within this page. */
	    subcount = I386_PAGE_SIZE-page_off;
	    if(subcount > count)
		subcount = count;

	    if (opcode == DEV_GATHER_S) {			/* copy data */
	           s=sys_safecopyto(proc_nr, user_vir,
		       vir_offset, (vir_bytes) vaddr+page_off, subcount, D);
	    } else {
	           s=sys_safecopyfrom(proc_nr, user_vir,
		       vir_offset, (vir_bytes) vaddr+page_off, subcount, D);
	    }
	    if(s != 0)
		return s;
	    count = subcount;
	    break;
	}

	/* Null byte stream generator. */
	case ZERO_DEV:
	    if (opcode == DEV_GATHER_S) {
		size_t suboffset = 0;
	        left = count;
	    	while (left > 0) {
	    	    chunk = (left > ZERO_BUF_SIZE) ? ZERO_BUF_SIZE : left;
	             s=sys_safecopyto(proc_nr, user_vir,
		       vir_offset+suboffset, (vir_bytes) dev_zero, chunk, D);
		    if(s != 0)
	    	        report("MEM","sys_safecopyto failed", s);
	    	    left -= chunk;
 	            suboffset += chunk;
	    }
	    }
	    break;

	}

	/* Book the number of bytes transferred. */
	position += count;
	vir_offset += count;
  	if ((iov->iov_size -= count) == 0) { iov++; nr_req--; vir_offset = 0; }

  }
  return 0;
}

/*===========================================================================*
 *				m_do_open				     *
 *===========================================================================*/
static int m_do_open(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
  int r;

/* Check device number on open. */
  if (m_prepare(m_ptr->DEVICE) == NIL_DEV) return(-ENXIO);
  if (m_device == MEM_DEV)
  {
	r = sys_enable_iop(m_ptr->IO_ENDPT);
	if (r != 0)
	{
		printk("m_do_open: sys_enable_iop failed for %d: %d\n",
			m_ptr->IO_ENDPT, r);
		return r;
	}
  }

  if(m_device < 0 || m_device >= NR_DEVS) {
      panic("MEM","wrong m_device",m_device);
  }

  openct[m_device]++;

  return(0);
}

/*===========================================================================*
 *				m_do_close				     *
 *===========================================================================*/
static int m_do_close(dp, m_ptr)
struct driver *dp;
kipc_msg_t *m_ptr;
{
  int r;

  if (m_prepare(m_ptr->DEVICE) == NIL_DEV) return(-ENXIO);

  if(m_device < 0 || m_device >= NR_DEVS) {
      panic("MEM","wrong m_device",m_device);
  }

  if(openct[m_device] < 1) {
      panic("MEM","closed too often",NO_NUM);
  }
  openct[m_device]--;

  return(0);
}


/*===========================================================================*
 *				m_init					     *
 *===========================================================================*/
static void m_init()
{
  /* Initialize this task. All minor devices are initialized one by one. */
  int i, s;

  if ((s=sys_getbootparam(&boot_params)) != 0) {
      panic("MEM","Couldn't get kernel information.",s);
  }

#if 0
  /* Map in kernel memory for /dev/kmem. */
  m_geom[KMEM_DEV].dv_base = cvul64(kinfo.kmem_base);
  m_geom[KMEM_DEV].dv_size = cvul64(kinfo.kmem_size);
  if((m_vaddrs[KMEM_DEV] = vm_map_phys(ENDPT_SELF, (void *) kinfo.kmem_base,
	kinfo.kmem_size)) == MAP_FAILED) {
	printk("MEM: Couldn't map in /dev/kmem.");
  }
#endif

#ifndef CONFIG_BUILTIN_INITRD
  initrd_base = boot_params.hdr.ramdisk_image;
  initrd_size = boot_params.hdr.ramdisk_size;

  /* Map in kernel memory for /dev/imgrd. */
  m_geom[IMGRD_DEV].dv_base = cvul64(0);
  m_geom[IMGRD_DEV].dv_size = cvul64(initrd_size);
  m_vaddrs[IMGRD_DEV] = (vir_bytes) vm_map_phys(ENDPT_SELF, (void *) initrd_base, initrd_size);

  if((void*)m_vaddrs[IMGRD_DEV] == MAP_FAILED) {
    printk("MEM: Couldn't map initial ramdisk.");
  } else {
    printk("memory: initrd (physical: 0x%x) mapped at 0x%x\n", initrd_base, m_vaddrs[IMGRD_DEV]);
  }
#else /* CONFIG_BUILTIN_INITRD */
  /* Ramdisk image built into the memory driver */
  m_geom[IMGRD_DEV].dv_base= cvul64(0);
  m_geom[IMGRD_DEV].dv_size= cvul64(initrd_size);
  m_vaddrs[IMGRD_DEV] = (vir_bytes) initrd;
#endif

  /* Initialize /dev/zero. Simply write zeros into the buffer. */
  for (i=0; i<ZERO_BUF_SIZE; i++) {
       dev_zero[i] = '\0';
  }

  for(i = 0; i < NR_DEVS; i++)
	openct[i] = 0;

  /* Set up memory range for /dev/mem. */
  m_geom[MEM_DEV].dv_base = cvul64(0);
  m_geom[MEM_DEV].dv_size = cvul64(0xffffffff);

  m_vaddrs[MEM_DEV] = (vir_bytes) MAP_FAILED; /* we are not mapping this in. */
}

/*===========================================================================*
 *				m_ioctl					     *
 *===========================================================================*/
static int m_ioctl(dp, m_ptr)
struct driver *dp;			/* pointer to driver structure */
kipc_msg_t *m_ptr;				/* pointer to control message */
{
/* I/O controls for the memory driver. Currently there is one I/O control:
 * - MIOCRAMSIZE: to set the size of the RAM disk.
 */
  struct device *dv;

  switch (m_ptr->REQUEST) {
    case MIOCRAMSIZE: {
	/* Someone wants to create a new RAM disk with the given size. */
	u32_t ramdev_size;
	int s, dev;
	void *mem;

	/* A ramdisk can be created only once, and only on RAM disk device. */
	dev = m_ptr->DEVICE;
	if(dev < 0 || dev >= NR_DEVS) {
		printk("MEM: MIOCRAMSIZE: %d not a valid device\n", dev);
	}
	if((dev < RAM_DEV_FIRST || dev > RAM_DEV_LAST) && dev != RAM_DEV_OLD) {
		printk("MEM: MIOCRAMSIZE: %d not a ramdisk\n", dev);
	}
        if ((dv = m_prepare(dev)) == NIL_DEV) return(-ENXIO);

	/* Get request structure */
	   s= sys_safecopyfrom(m_ptr->IO_ENDPT, (vir_bytes)m_ptr->IO_GRANT,
		0, (vir_bytes)&ramdev_size, sizeof(ramdev_size), D);
	if (s != 0)
		return s;
	if(m_vaddrs[dev] && !cmp64(dv->dv_size, cvul64(ramdev_size))) {
		return 0;
	}
	/* openct is 1 for the ioctl(). */
	if(openct[dev] != 1) {
		printk("MEM: MIOCRAMSIZE: %d in use (count %d)\n",
			dev, openct[dev]);
		return(-EBUSY);
	}
	if(m_vaddrs[dev]) {
		u32_t size;
		if(ex64hi(dv->dv_size)) {
			panic("MEM","huge old ramdisk", NO_NUM);
		}
		size = ex64lo(dv->dv_size);
		munmap((void *) m_vaddrs[dev], size);
		m_vaddrs[dev] = (vir_bytes) NULL;
	}

#if DEBUG
	printk("MEM:%d: allocating ramdisk of size 0x%x\n", dev, ramdev_size);
#endif

	/* Try to allocate a piece of memory for the RAM disk. */
	if((mem = mmap(0, ramdev_size, PROT_READ|PROT_WRITE,
		MAP_PREALLOC|MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
	    printk("MEM: failed to get memory for ramdisk\n");
            return(-ENOMEM);
        }

	m_vaddrs[dev] = (vir_bytes) mem;

	dv->dv_size = cvul64(ramdev_size);

	break;
    }

    default:
  	return(do_diocntl(&m_dtab, m_ptr));
  }
  return 0;
}

/*===========================================================================*
 *				m_geometry				     *
 *===========================================================================*/
static void m_geometry(entry)
struct partition *entry;
{
	/* Memory devices don't have a geometry, but the outside world insists. */
	entry->cylinders = div64u(m_geom[m_device].dv_size, SECTOR_SIZE) / (64 * 32);
	entry->heads = 64;
	entry->sectors = 32;
}

