/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the procedures that look up path names in the directory
 * system and determine the inode number that goes with a given path name.
 *
 *  The entry points into this file are
 *   eat_path:   the 'main' routine of the path-to-inode conversion mechanism
 *   last_dir:   find the final directory on a given path
 *   advance:    parse one component of a path name
 *   search_dir: search a directory for a string and return its inode number
 *
 */
#include "fs.h"
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>
#include <nucleos/stat.h>
#include <servers/mfs/buf.h>
#include <servers/mfs/inode.h>
#include <servers/mfs/super.h>
#include <nucleos/vfsif.h>

char dot1[2] = ".";	/* used for search_dir to bypass the access */
char dot2[3] = "..";	/* permissions for . and ..		    */

static char *get_name(char *name, char string[NAME_MAX+1]);
static int ltraverse(struct inode *rip, char *suffix);
static int parse_path(ino_t dir_ino, ino_t root_ino,
					int flags, struct inode **res_inop,
					size_t *offsetp, int *symlinkp);


/*===========================================================================*
 *                             fs_lookup				     *
 *===========================================================================*/
int fs_lookup()
{
  cp_grant_id_t grant, grant2;
  int r, r1, len, flags, symlinks;
  size_t offset = 0, path_size, cred_size;
  ino_t dir_ino, root_ino;
  struct inode *rip;

  grant		= fs_m_in.REQ_GRANT;
  path_size	= fs_m_in.REQ_PATH_SIZE;	/* Size of the buffer */
  len		= fs_m_in.REQ_PATH_LEN;	/* including terminating nul */
  dir_ino	= fs_m_in.REQ_DIR_INO;
  root_ino	= fs_m_in.REQ_ROOT_INO;
  flags		= fs_m_in.REQ_FLAGS;

  /* Check length. */
  if(len > sizeof(user_path)) return(-E2BIG);	/* too big for buffer */
  if(len < 1) return(-EINVAL);			/* too small */

  /* Copy the pathname and set up caller's user and group id */
  r = sys_safecopyfrom(VFS_PROC_NR, grant, /*offset*/ 0, 
            (vir_bytes) user_path, (phys_bytes) len, D);
  if(r != 0) {
	printf("MFS %s:%d sys_safecopyfrom failed: %d\n", 
	       __FILE__, __LINE__, r);
	return(r);
  }

  /* Verify this is a null-terminated path. */
  if(user_path[len - 1] != '\0') return(-EINVAL);

  if(flags & PATH_GET_UCRED) { /* Do we have to copy uid/gid credentials? */
  	int i;
  	grant2	= fs_m_in.REQ_GRANT2;
  	cred_size = fs_m_in.REQ_UCRED_SIZE;

  	if (cred_size > sizeof(credentials)) return(-EINVAL); /* Too big. */
  	r = sys_safecopyfrom(VFS_PROC_NR, grant2, 0, (vir_bytes) &credentials,
  			     (phys_bytes) cred_size, D);
  	if (r != 0) {
  		printf("MFS %s:%d sys_safecopyfrom failed: %d\n",
  		       __FILE__, __LINE__, r);
  		return(r);
  	}
  	caller_uid = credentials.vu_uid;
  	caller_gid = credentials.vu_gid;
  } else {
  	memset(&credentials, 0, sizeof(credentials));
	caller_uid	= fs_m_in.REQ_UID;
	caller_gid	= fs_m_in.REQ_GID;
  }

  /* Lookup inode */
  rip = NULL;
  r = parse_path(dir_ino, root_ino, flags, &rip, &offset, &symlinks);

  if(symlinks != 0 && (r == -ELEAVEMOUNT || r == -EENTERMOUNT || r == -ESYMLINK)){
	len = strlen(user_path)+1;
	if(len > path_size) return(-ENAMETOOLONG);

	r1 = sys_safecopyto(VFS_PROC_NR, grant, 0, (vir_bytes) user_path,
			    (phys_bytes) len, D);
	if(r1 != 0) {
		printf("%s:%d fs_lookup: sys_safecopyto failed: %d\n",
		       __FILE__, __LINE__, r1);
		return(r1);
	}
  }

  if(r == -ELEAVEMOUNT || r == -ESYMLINK) {
	  /* Report offset and the error */
	  fs_m_out.RES_OFFSET = offset;
	  fs_m_out.RES_SYMLOOP = symlinks;

	  return(r);
  }

  if (r != 0 && r != -EENTERMOUNT) return(r);

  fs_m_out.RES_INODE_NR	= rip->i_num;
  fs_m_out.RES_MODE		= rip->i_mode;
  fs_m_out.RES_FILE_SIZE_LO	= rip->i_size;
  fs_m_out.RES_SYMLOOP		= symlinks;
  fs_m_out.RES_UID		= rip->i_uid;
  fs_m_out.RES_GID		= rip->i_gid;
  
  /* This is only valid for block and character specials. But it doesn't
   * cause any harm to set RES_DEV always. */
  fs_m_out.RES_DEV		= (dev_t) rip->i_zone[0];

  if(r == -EENTERMOUNT) {
	  fs_m_out.RES_OFFSET	= offset;
	  put_inode(rip); /* Only return a reference to the final object */
  }

  return(r);
}


/*===========================================================================*
 *                             parse_path				     *
 *===========================================================================*/
static int parse_path(dir_ino, root_ino, flags, res_inop, offsetp, symlinkp)
ino_t dir_ino;
ino_t root_ino;
int flags;
struct inode **res_inop;
size_t *offsetp;
int *symlinkp;
{
  /* Parse the path in user_path, starting at dir_ino. If the path is the empty
   * string, just return dir_ino. It is upto the caller to treat an empty
   * path in a special way. Otherwise, if the path consists of just one or
   * more slash ('/') characters, the path is replaced with ".". Otherwise,
   * just look up the first (or only) component in path after skipping any
   * leading slashes. 
   */
  int r, leaving_mount;
  struct inode *rip, *dir_ip;
  char *cp, *next_cp; /* component and next component */
  char component[NAME_MAX+1];

  /* Start parsing path at the first component in user_path */
  cp = user_path;  

  /* No symlinks encountered yet */
  *symlinkp = 0;

  /* Find starting inode inode according to the request message */
  if((rip = find_inode(fs_dev, dir_ino)) == NIL_INODE) 
	return(-ENOENT);

  /* If dir has been removed return -ENOENT. */
  if (rip->i_nlinks == 0) 
	return(-ENOENT);
 
  dup_inode(rip);

  /* If the given start inode is a mountpoint, we must be here because the file
   * system mounted on top returned an -ELEAVEMOUNT error. In this case, we must
   * only accept ".." as the first path component.
   */
  leaving_mount = rip->i_mountpoint; /* True iff rip is a mountpoint */

  /* Scan the path component by component. */
  while (TRUE) {
	if(cp[0] == '\0') {
		/* We're done; either the path was empty or we've parsed all 
		   components of the path */
		
		*res_inop = rip;
		*offsetp += cp - user_path;

		/* Return -EENTERMOUNT if we are at a mount point */
		if (rip->i_mountpoint) return(-EENTERMOUNT);
		
		return(0);
	}

	while(cp[0] == '/') cp++;
	next_cp = get_name(cp, component);

	/* Special code for '..'. A process is not allowed to leave a chrooted
	 * environment. A lookup of '..' at the root of a mounted filesystem
	 * has to return -ELEAVEMOUNT. In both cases, the caller needs search
	 * permission for the current inode, as it is used as directory.
	 */
	if(strcmp(component, "..") == 0) {
		/* 'rip' is now accessed as directory */
		if ((r = forbidden(rip, X_BIT)) != 0) {
			put_inode(rip);
			return(r);
		}

		if (rip->i_num == root_ino) {
			cp = next_cp;
			continue;	/* Ignore the '..' at a process' root 
					   and move on to the next component */
		}
		
		if (rip->i_num == ROOT_INODE && !rip->i_sp->s_is_root) {
			/* Climbing up to parent FS */

			put_inode(rip);
			*offsetp += cp - user_path; 
			return(-ELEAVEMOUNT);
		}
	}

	/* Only check for a mount point if we are not coming from one. */
	if (!leaving_mount && rip->i_mountpoint) {
		/* Going to enter a child FS */

		*res_inop = rip;
		*offsetp += cp - user_path;
		return(-EENTERMOUNT);
	}

	/* There is more path.  Keep parsing.
	 * If we're leaving a mountpoint, skip directory permission checks.
	 */
	dir_ip = rip;
	rip = advance(dir_ip, leaving_mount ? dot2 : component, CHK_PERM);
	if(err_code == -ELEAVEMOUNT || err_code == -EENTERMOUNT)
		err_code = 0;

	if (err_code != 0) {
		put_inode(dir_ip);
		return(err_code);
	}
	
	leaving_mount = 0;

	/* The call to advance() succeeded.  Fetch next component. */
	if (S_ISLNK(rip->i_mode)) {

		if (next_cp[0] == '\0' && (flags & PATH_RET_SYMLINK)) {
			put_inode(dir_ip);
			*res_inop = rip;
			*offsetp += next_cp - user_path;
			return(0);
		}

		/* Extract path name from the symlink file */
		r = ltraverse(rip, next_cp);
		next_cp = user_path;
		*offsetp = 0;

		/* Symloop limit reached? */
		if (++(*symlinkp) > SYMLOOP_MAX)
			r = -ELOOP;

		if (r != 0) {
			put_inode(dir_ip);
			put_inode(rip);
			return(r);
		}

		if (next_cp[0] == '/') {
                        put_inode(dir_ip);
                        put_inode(rip);
                        return(-ESYMLINK);
		}
	
		put_inode(rip);
		dup_inode(dir_ip);
		rip = dir_ip;
	} 

	put_inode(dir_ip);
	cp = next_cp; /* Process subsequent component in next round */
  }
}


/*===========================================================================*
 *                             ltraverse				     *
 *===========================================================================*/
static int ltraverse(rip, suffix)
register struct inode *rip;	/* symbolic link */
char *suffix;			/* current remaining path. Has to point in the
				 * user_path buffer
				 */
{
/* Traverse a symbolic link. Copy the link text from the inode and insert
 * the text into the path. Return error code or report success. Base 
 * directory has to be determined according to the first character of the
 * new pathname.
 */
  
  block_t blink;	/* block containing link text */
  size_t llen;		/* length of link */
  size_t slen;		/* length of suffix */
  struct buf *bp;	/* buffer containing link text */
  char *sp;		/* start of link text */

  bp  = NIL_BUF;

  if ((blink = read_map(rip, (off_t) 0)) == NO_BLOCK)
	return(-EIO);

  bp = get_block(rip->i_dev, blink, NORMAL);
  llen = rip->i_size;
  sp = bp->b_data;
  slen = strlen(suffix);

  /* The path we're parsing looks like this:
   * /already/processed/path/<link> or
   * /already/processed/path/<link>/not/yet/processed/path
   * After expanding the <link>, the path will look like
   * <expandedlink> or
   * <expandedlink>/not/yet/processed
   * In both cases user_path must have enough room to hold <expandedlink>.
   * However, in the latter case we have to move /not/yet/processed to the
   * right place first, before we expand <link>. When strlen(<expandedlink>) is
   * smaller than strlen(/already/processes/path), we move the suffix to the
   * left. Is strlen(<expandedlink>) greater then we move it to the right. Else
   * we do nothing. */ 

  if (slen > 0) { /* Do we have path after the link? */
	/* For simplicity we require that suffix starts with a slash */
	if (suffix[0] != '/') {
		panic(__FILE__,
			"ltraverse: suffix does not start with a slash",
			NO_NUM);
	}

	/* To be able to expand the <link>, we have to move the 'suffix'
	 * to the right place. */
	if (slen + llen + 1 > sizeof(user_path))
		return(-ENAMETOOLONG);/* <expandedlink>+suffix+\0 does not fit*/
	if (suffix-user_path != llen) /* Move suffix left or right if needed */
		memmove(&user_path[llen], suffix, slen+1);
  } else {
  	if (llen + 1 > sizeof(user_path))
  		return(-ENAMETOOLONG); /* <expandedlink> + \0 does not fix */
  		
	/* Set terminating nul */
	user_path[llen]= '\0';
  }

  /* Everything is set, now copy the expanded link to user_path */
  memmove(user_path, sp, llen);

  put_block(bp, DIRECTORY_BLOCK);
  return(0);
}


/*===========================================================================*
 *				advance					     *
 *===========================================================================*/
struct inode *advance(dirp, string, chk_perm)
struct inode *dirp;		/* inode for directory to be searched */
char string[NAME_MAX];		/* component name to look for */
int chk_perm;			/* check permissions when string is looked up*/
{
/* Given a directory and a component of a path, look up the component in
 * the directory, find the inode, open it, and return a pointer to its inode
 * slot.
 */
  ino_t numb;
  struct inode *rip;

  /* If 'string' is empty, return an error. */
  if (string[0] == '\0') {
  	err_code = -ENOENT;
	return(NIL_INODE);
  }

  /* Check for NIL_INODE. */
  if (dirp == NIL_INODE) return(NIL_INODE);

  /* If 'string' is not present in the directory, signal error. */
  if ( (err_code = search_dir(dirp, string, &numb, LOOK_UP, chk_perm)) != 0) {
	return(NIL_INODE);
  }

  /* The component has been found in the directory.  Get inode. */
  if ( (rip = get_inode(dirp->i_dev, (int) numb)) == NIL_INODE)  {
	return(NIL_INODE);
  }

  /* The following test is for "mountpoint/.." where mountpoint is a
   * mountpoint. ".." will refer to the root of the mounted filesystem,
   * but has to become a reference to the parent of the 'mountpoint'
   * directory.
   *
   * This case is recognized by the looked up name pointing to a
   * root inode, and the directory in which it is held being a
   * root inode, _and_ the name[1] being '.'. (This is a test for '..'
   * and excludes '.'.)
   */
  if (rip->i_num == ROOT_INODE) {
	  if (dirp->i_num == ROOT_INODE) {
		  if (string[1] == '.') {
			  if (!rip->i_sp->s_is_root) {
				  /* Climbing up mountpoint */
				  err_code = -ELEAVEMOUNT;
			  }
		  }
	  }
  }

  /* See if the inode is mounted on.  If so, switch to root directory of the
   * mounted file system.  The minix3_super_block provides the linkage between the
   * inode mounted on and the root directory of the mounted file system.
   */
  if (rip != NIL_INODE && rip->i_mountpoint) {
	  /* Mountpoint encountered, report it */
	  err_code = -EENTERMOUNT;
  }

  return(rip);
}


/*===========================================================================*
 *				get_name				     *
 *===========================================================================*/
static char *get_name(path_name, string)
char *path_name;		/* path name to parse */
char string[NAME_MAX+1];	/* component extracted from 'old_name' */
{
/* Given a pointer to a path name in fs space, 'path_name', copy the first
 * component to 'string' (truncated if necessary, always nul terminated).
 * A pointer to the string after the first component of the name as yet
 * unparsed is returned.  Roughly speaking,
 * 'get_name' = 'path_name' - 'string'.
 *
 * This routine follows the standard convention that /usr/ast, /usr//ast,
 * //usr///ast and /usr/ast/ are all equivalent.
 */
  size_t len;
  char *cp, *ep;

  cp = path_name;

  /* Skip leading slashes */
  while (cp[0] == '/') cp++;

  /* Find the end of the first component */
  ep = cp;
  while(ep[0] != '\0' && ep[0] != '/')
	ep++;

  len = ep - cp;

  /* Truncate the amount to be copied if it exceeds NAME_MAX */
  if (len > NAME_MAX)
	len = NAME_MAX;

  /* Special case of the string at cp is empty */
  if (len == 0) 
	strcpy(string, ".");  /* Return "." */
  else {
	memcpy(string, cp, len);
	string[len]= '\0';
  }

  return(ep);
}


/*===========================================================================*
 *				search_dir				     *
 *===========================================================================*/
int search_dir(ldir_ptr, string, numb, flag, check_permissions)
register struct inode *ldir_ptr; /* ptr to inode for dir to search */
char string[NAME_MAX];		 /* component to search for */
ino_t *numb;			 /* pointer to inode number */
int flag;			 /* LOOK_UP, ENTER, DELETE or IS_EMPTY */
int check_permissions;		 /* check permissions when flag is !IS_EMPTY */
{
/* This function searches the directory whose inode is pointed to by 'ldip':
 * if (flag == ENTER)  enter 'string' in the directory with inode # '*numb';
 * if (flag == DELETE) delete 'string' from the directory;
 * if (flag == LOOK_UP) search for 'string' and return inode # in 'numb';
 * if (flag == IS_EMPTY) return 0 if only . and .. in dir else -ENOTEMPTY;
 *
 *    if 'string' is dot1 or dot2, no access permissions are checked.
 */

  register struct direct *dp = NULL;
  register struct buf *bp = NULL;
  int i, r, e_hit, t, match;
  mode_t bits;
  off_t pos;
  unsigned new_slots, old_slots;
  block_t b;
  struct minix3_super_block *sp;
  int extended = 0;

  /* If 'ldir_ptr' is not a pointer to a dir inode, error. */
  if ( (ldir_ptr->i_mode & I_TYPE) != I_DIRECTORY)  {
	return(-ENOTDIR);
   }
  
  r = 0;

  if (flag != IS_EMPTY) {
	bits = (flag == LOOK_UP ? X_BIT : W_BIT | X_BIT);

	if (string == dot1 || string == dot2) {
		if (flag != LOOK_UP) r = read_only(ldir_ptr);
				     /* only a writable device is required. */
        } else if(check_permissions) {
		r = forbidden(ldir_ptr, bits); /* check access permissions */
	}
  }
  if (r != 0) return(r);
  
  /* Step through the directory one block at a time. */
  old_slots = (unsigned) (ldir_ptr->i_size/DIR_ENTRY_SIZE);
  new_slots = 0;
  e_hit = FALSE;
  match = 0;			/* set when a string match occurs */

  for (pos = 0; pos < ldir_ptr->i_size; pos += ldir_ptr->i_sp->s_block_size) {
	b = read_map(ldir_ptr, pos);	/* get block number */

	/* Since directories don't have holes, 'b' cannot be NO_BLOCK. */
	bp = get_block(ldir_ptr->i_dev, b, NORMAL);	/* get a dir block */

	if (bp == NO_BLOCK)
		panic(__FILE__,"get_block returned NO_BLOCK", NO_NUM);

	/* Search a directory block. */
	for (dp = &bp->b_dir[0];
		dp < &bp->b_dir[NR_DIR_ENTRIES(ldir_ptr->i_sp->s_block_size)];
		dp++) {
		if (++new_slots > old_slots) { /* not found, but room left */
			if (flag == ENTER) e_hit = TRUE;
			break;
		}

		/* Match occurs if string found. */
		if (flag != ENTER && dp->d_ino != 0) {
			if (flag == IS_EMPTY) {
				/* If this test succeeds, dir is not empty. */
				if (strcmp(dp->d_name, "." ) != 0 &&
				    strcmp(dp->d_name, "..") != 0) match = 1;
			} else {
				if (strncmp(dp->d_name, string, NAME_MAX) == 0){
					match = 1;
				}
			}
		}

		if (match) {
			/* LOOK_UP or DELETE found what it wanted. */
			r = 0;
			if (flag == IS_EMPTY) r = -ENOTEMPTY;
			else if (flag == DELETE) {
				/* Save d_ino for recovery. */
				t = NAME_MAX - sizeof(ino_t);
				*((ino_t *) &dp->d_name[t]) = dp->d_ino;
				dp->d_ino = 0;	/* erase entry */
				bp->b_dirt = DIRTY;
				ldir_ptr->i_update |= CTIME | MTIME;
				ldir_ptr->i_dirt = DIRTY;
			} else {
				sp = ldir_ptr->i_sp;	/* 'flag' is LOOK_UP */
				*numb = conv4(sp->s_native, (int) dp->d_ino);
			}
			put_block(bp, DIRECTORY_BLOCK);
			return(r);
		}

		/* Check for free slot for the benefit of ENTER. */
		if (flag == ENTER && dp->d_ino == 0) {
			e_hit = TRUE;	/* we found a free slot */
			break;
		}
	}

	/* The whole block has been searched or ENTER has a free slot. */
	if (e_hit) break;	/* e_hit set if ENTER can be performed now */
	put_block(bp, DIRECTORY_BLOCK);	/* otherwise, continue searching dir */
  }

  /* The whole directory has now been searched. */
  if (flag != ENTER) {
  	return(flag == IS_EMPTY ? 0 : -ENOENT);
  }

  /* This call is for ENTER.  If no free slot has been found so far, try to
   * extend directory.
   */
  if (e_hit == FALSE) { /* directory is full and no room left in last block */
	new_slots++;		/* increase directory size by 1 entry */
	if (new_slots == 0) return(-EFBIG); /* dir size limited by slot count */
	if ( (bp = new_block(ldir_ptr, ldir_ptr->i_size)) == NIL_BUF)
		return(err_code);
	dp = &bp->b_dir[0];
	extended = 1;
  }

  /* 'bp' now points to a directory block with space. 'dp' points to slot. */
  (void) memset(dp->d_name, 0, (size_t) NAME_MAX); /* clear entry */
  for (i = 0; i < NAME_MAX && string[i]; i++) dp->d_name[i] = string[i];
  sp = ldir_ptr->i_sp; 
  dp->d_ino = conv4(sp->s_native, (int) *numb);
  bp->b_dirt = DIRTY;
  put_block(bp, DIRECTORY_BLOCK);
  ldir_ptr->i_update |= CTIME | MTIME;	/* mark mtime for update later */
  ldir_ptr->i_dirt = DIRTY;
  if (new_slots > old_slots) {
	ldir_ptr->i_size = (off_t) new_slots * DIR_ENTRY_SIZE;
	/* Send the change to disk if the directory is extended. */
	if (extended) rw_inode(ldir_ptr, WRITING);
  }
  return(0);
}

