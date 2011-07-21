#ifndef _NUCLEOS_SHM_H
#define _NUCLEOS_SHM_H

#include <nucleos/types.h>
#include <nucleos/ipc.h>

#if !defined(__KERNEL__) && !defined(__UKERNEL__)
#include <unistd.h>
#endif

typedef unsigned long int shmatt_t;

#define SHMLBA	getpagesize()
#define SHMMNI	4096
#define SHMSEG	32		/* max shared segs per process */

struct shmid_ds
{
	struct ipc_perm shm_perm;	/* Ownership and permissions */
	size_t	shm_segsz;			/* Size of segment (bytes) */
	time_t	shm_atime;			/* Last attach time */
	time_t	shm_dtime;			/* Last detach time */
	time_t	shm_ctime;			/* Last change time */
	pid_t	shm_cpid;			/* PID of creator */
	pid_t	shm_lpid;			/* PID of last shmat()/shmdt() */
	shmatt_t	shm_nattch;		/* No. of current attaches */
};

/* Permission flag for shmget. */
#define SHM_R	0400
#define SHM_W	0200

#define SHM_RDONLY 010000		/* attach read-only else read-write */
#define SHM_RND    020000		/* round attach address to SHMLBA */

/* shm_mode upper byte flags */
#define SHM_DEST 01000			/* segment will be destroyed on last detach */
#define SHM_LOCKED 02000		/* segment will not be swapped */

/* ipcs ctl commands */
#define SHM_STAT	13
#define SHM_INFO	14

struct  shminfo
{
	unsigned long int shmmax;
	unsigned long int shmmin;
	unsigned long int shmmni;
	unsigned long int shmseg;
	unsigned long int shmall;
};

struct shm_info
{
	int used_ids;
	unsigned long int shm_tot;  /* total allocated shm */
	unsigned long int shm_rss;  /* total resident shm */
	unsigned long int shm_swp;  /* total swapped shm */
	unsigned long int swap_attempts;
	unsigned long int swap_successes;
};

#if defined(__KERNEL__) || defined(__UKERNEL__)

/* The following System V style IPC functions implement a shared memory
 * facility.  The definition is found in XPG4.2.
 */

/* Shared memory control operation. */
int shmctl(int __shmid, int __cmd, struct shmid_ds *__buf);

/* Get shared memory segment. */
int shmget(key_t __key, size_t __size, int __shmflg);

/* Attach shared memory segment. */
void *shmat(int __shmid, const void *__shmaddr, int __shmflg);

/* Deattach shared memory segment. */
int shmdt(const void *__shmaddr);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* _NUCLEOS_SHM_H */
