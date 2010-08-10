#include "inc.h"
#include <nucleos/stat.h>
#include <nucleos/statfs.h>
#include <nucleos/com.h>
#include <nucleos/vfsif.h>
#include <nucleos/string.h>
#include <nucleos/time.h>

static int stat_dir_record(struct dir_record *dir, int pipe_pos,
					 int who_e, cp_grant_id_t gid);

/**
 * Common code for stat and fstat system calls.
 * @param dir  pointer to dir record to stat
 * @param pipe_pos  position in a pipe, supplied by fstat()
 * @param who_e  Caller endpoint
 * @param gid  grant for the stat buf
 * @return 0 on success
 */
static int stat_dir_record(struct dir_record *dir, int pipe_pos, int who_e, cp_grant_id_t gid)
{
	/* This function returns all the info about a particular inode. It's missing
	 * the recording date because of a bug in the standard functions stdtime.
	 * Once the bug is fixed the function can be called inside this function to
	 * return the date. */


	struct kstat ksb;
	mode_t mo;
	int r, s;
	struct tm ltime;
	time_t time1;

	ksb.dev = fs_dev;	/* the device of the file */
	ksb.ino = ID_DIR_RECORD(dir); /* the id of the dir record */
	ksb.mode = dir->d_mode; /* flags of the file */
	ksb.nlink = dir->d_count; /* times this file is used */
	ksb.uid = 0;		/* user root */
	ksb.gid = 0;		/* group operator */
	ksb.rdev = NO_DEV;
	ksb.size = dir->d_file_size;	/* size of the file */

	/* @nucleos: FIX ME! */
	ksb.blksize = 0;
	ksb.blocks = ((ksb.size + 511) >> 9);

	ltime.tm_year = dir->rec_date[0];
	ltime.tm_mon = dir->rec_date[1] - 1;
	ltime.tm_mday = dir->rec_date[2];
	ltime.tm_hour = dir->rec_date[3];
	ltime.tm_min = dir->rec_date[4];
	ltime.tm_sec = dir->rec_date[5];
	ltime.tm_isdst = 0;

	if (dir->rec_date[6] != 0)
		ltime.tm_hour += dir->rec_date[6] / 4;

	time1 = mktime(ltime.tm_year, ltime.tm_mon,
		       ltime.tm_mday, ltime.tm_hour,
		       ltime.tm_min, ltime.tm_sec);

	ksb.atime.tv_sec = time1;
	ksb.mtime.tv_sec = time1;
	ksb.ctime.tv_sec = time1;

	ksb.atime.tv_nsec = 0;
	ksb.mtime.tv_nsec = 0;
	ksb.ctime.tv_nsec = 0;

	/* Copy the struct to caller space. */
	r = sys_safecopyto(who_e, gid, 0, (vir_bytes)&ksb, (phys_bytes)sizeof(ksb), D);

	return(r);
}

int fs_stat(void)
{
	register int r;
	struct dir_record *dir;
	r = -EINVAL;

	if ((dir = get_dir_record(fs_m_in.REQ_INODE_NR)) != NULL) {
		r = stat_dir_record(dir, 0, fs_m_in.m_source, fs_m_in.REQ_GRANT);
		release_dir_record(dir);
	}

	return(r);
}

int fs_fstatfs(void)
{
	struct statfs st;
	int r;

	st.f_bsize = v_pri.logical_block_size_l;

	/* Copy the struct to user space. */
	r = sys_safecopyto(fs_m_in.m_source, fs_m_in.REQ_GRANT, 0,
		 	   (vir_bytes) &st, (phys_bytes) sizeof(st), D);

	return(r);
}
