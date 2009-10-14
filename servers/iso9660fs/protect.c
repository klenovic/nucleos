#include "inc.h"
#include <nucleos/unistd.h>
#include <nucleos/vfsif.h>
#include "buf.h"

/* This calling is used to access a particular file. */
/*===========================================================================*
 *				fs_access				     *
 *===========================================================================*/
int fs_access()
{
  struct dir_record *rip;
  int r = 0;

  /* Temporarily open the file whose access is to be checked. */
  caller_uid = fs_m_in.REQ_UID;
  caller_gid = fs_m_in.REQ_GID;
  
  /* Temporarily open the file. */
  if ( (rip = get_dir_record(fs_m_in.REQ_INODE_NR)) == NULL) {
    printf("I9660FS(%d) get_dir_record by fs_access() failed\n", SELF_E);
    return(-EINVAL);
  }

  /* For now ISO9660 doesn't have permission control (read and execution to
   * everybody by default. So the access is always granted. */
  
  release_dir_record(rip);	/* Release the dir record used */
  return(r);
}
