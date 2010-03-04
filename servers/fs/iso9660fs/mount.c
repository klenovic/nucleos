#include "inc.h"
#include <nucleos/string.h>
#include <nucleos/com.h>
#include <nucleos/vfsif.h>
#include <servers/ds/ds.h>

#include "const.h"
#include "glo.h"

/* Function of mounting and unmounting for ISO9660 */

/* Reads the super inode. This function is requested at the mounting of the
 * filesystem. */
/*===========================================================================*
 *				fs_readsuper				     *
 *===========================================================================*/
int fs_readsuper() {
  cp_grant_id_t label_gid;
  size_t label_len;
  int r = 0;
  unsigned long tasknr;
  endpoint_t driver_e;
  int readonly, isroot;

  fs_dev    = fs_m_in.REQ_DEV;
  label_gid = fs_m_in.REQ_GRANT;
  label_len = fs_m_in.REQ_PATH_LEN;
  readonly  = 1;                       /* Always mount devices read only. */
  isroot    = (fs_m_in.REQ_FLAGS & REQ_ISROOT) ? 1 : 0;

  if (label_len > sizeof(fs_dev_label))
       return(-EINVAL);

  r = sys_safecopyfrom(fs_m_in.m_source, label_gid, 0, (vir_bytes)fs_dev_label,
		       label_len, D);

  if (r != 0) {
	printf("ISOFS %s:%d safecopyfrom failed: %d\n", __FILE__, __LINE__, r);
	return(-EINVAL);
  }

  r = ds_retrieve_u32(fs_dev_label, (u32_t*)&tasknr);
  if (r != 0) {
	printf("ISOFS %s:%d ds_retrieve_u32 failed for '%s': %d\n",
	       __FILE__, __LINE__, fs_dev_label, r);
	return(-EINVAL);
  }

  driver_e = tasknr;

  /* Map the driver endpoint for this major */
  driver_endpoints[(fs_dev >> MAJOR) & BYTE].driver_e =  driver_e;

  /* Open the device the file system lives on */
  if (dev_open(driver_e, fs_dev, driver_e,
	readonly ? R_BIT : (R_BIT|W_BIT)) != 0) {
        return(-EINVAL);
  }

  /* Read the superblock */
  r = read_vds(&v_pri, fs_dev);

  if (r != 0)
	return(r);

  /* Return some root inode properties */
  fs_m_out.RES_INODE_NR = ID_DIR_RECORD(v_pri.dir_rec_root);
  fs_m_out.RES_MODE = v_pri.dir_rec_root->d_mode;
  fs_m_out.RES_FILE_SIZE_LO = v_pri.dir_rec_root->d_file_size;
  fs_m_out.RES_UID = SYS_UID; /* Always root */
  fs_m_out.RES_GID = SYS_GID; /* operator */

  return(r);
}

/*===========================================================================*
 *				fs_mountpoint				     *
 *===========================================================================*/
int fs_mountpoint() {
/* This function looks up the mount point, it checks the condition whether
 * the partition can be mounted on the inode or not. 
 */

  register struct dir_record *rip;
  int r = 0;
  mode_t bits;
  
  /* Temporarily open the file. */
  if ((rip = get_dir_record(fs_m_in.REQ_INODE_NR)) == NULL)
	return(-EINVAL);

  if (rip->d_mountpoint)
	r = -EBUSY;

  /* If the inode is not a dir returns error */
  if ((rip->d_mode & I_TYPE) != I_DIRECTORY)
	r = -ENOTDIR;
	
  release_dir_record(rip);

  if (r == 0)
	rip->d_mountpoint = TRUE;

  return(r);
}


/*===========================================================================*
 *				fs_unmount				     *
 *===========================================================================*/
int fs_unmount(void) {
  release_v_pri(&v_pri);	/* Release the super block */
  dev_close(driver_endpoints[(fs_dev >> MAJOR) & BYTE].driver_e, fs_dev);
  return(0);
}
