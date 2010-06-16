/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the wrapper functions for issueing a request
 * and receiving response from FS processes.
 * Each function builds a request message according to the request
 * parameter, calls the most low-level fs_sendrec and copies
 * back the response.
 * The low-level fs_sendrec handles the recovery mechanism from
 * a dead driver and reissues the request.
 */
#include "fs.h"
#include <nucleos/string.h>
#include <nucleos/stat.h>
#include <nucleos/statfs.h>
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/keymap.h>
#include <nucleos/const.h>
#include <nucleos/endpoint.h>
#include <nucleos/u64.h>
#include <nucleos/vfsif.h>
#include <servers/vfs/fproc.h>
#include "vmnt.h"
#include "vnode.h"
#include "param.h"

static int fs_sendrec_f(char *file, int line, endpoint_t fs_e,
				      kipc_msg_t *reqm);

#define fs_sendrec(e, m) fs_sendrec_f(__FILE__, __LINE__, (e), (m))


/*===========================================================================*
 *			req_breadwrite					     *
 *===========================================================================*/
int req_breadwrite(fs_e, user_e, dev, pos, num_of_bytes, user_addr,
	rw_flag, new_posp, cum_iop)
endpoint_t fs_e;
endpoint_t user_e;
dev_t dev;
u64_t pos;
unsigned int num_of_bytes;
char *user_addr;
int rw_flag;
u64_t *new_posp;
unsigned int *cum_iop;
{
    int r;
  cp_grant_id_t grant_id;
    kipc_msg_t m;

  grant_id = cpf_grant_magic(fs_e, user_e, (vir_bytes) user_addr, num_of_bytes,
			(rw_flag == READING ? CPF_WRITE : CPF_READ));
  if(grant_id == -1)
	panic(__FILE__, "req_breadwrite: cpf_grant_magic failed", NO_NUM);

    /* Fill in request message */
  m.m_type = rw_flag == READING ? REQ_BREAD : REQ_BWRITE;
  m.REQ_DEV2 = dev;
  m.REQ_GRANT = grant_id;
  m.REQ_SEEK_POS_LO = ex64lo(pos);
  m.REQ_SEEK_POS_HI = ex64hi(pos);
  m.REQ_NBYTES = num_of_bytes;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);
  if (r != 0) return(r);

    /* Fill in response structure */
  *new_posp = make64(m.RES_SEEK_POS_LO, m.RES_SEEK_POS_HI);
  *cum_iop = m.RES_NBYTES;

  return(0);
}


/*===========================================================================*
 *				req_chmod	      			     *
 *===========================================================================*/
int req_chmod(fs_e, inode_nr, rmode, new_modep)
int fs_e;
ino_t inode_nr;
mode_t rmode;
mode_t *new_modep;
{
    kipc_msg_t m;
    int r;

    /* Fill in request message */
    m.m_type = REQ_CHMOD;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_MODE = rmode;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);

    /* Copy back actual mode. */
    *new_modep = m.RES_MODE;

  return(r);
}


/*===========================================================================*
 *				req_chown          			     *
 *===========================================================================*/
int req_chown(fs_e, inode_nr, newuid, newgid, new_modep)
endpoint_t fs_e;
ino_t inode_nr;
uid_t newuid;
gid_t newgid;
mode_t *new_modep;
{
    kipc_msg_t m;
    int r;

    /* Fill in request message */
    m.m_type = REQ_CHOWN;
    m.REQ_INODE_NR = inode_nr;
  m.REQ_UID = newuid;
  m.REQ_GID = newgid;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);

    /* Return new mode to caller. */
    *new_modep = m.RES_MODE;

  return(r);
}


/*===========================================================================*
 *				req_create				     *
 *===========================================================================*/
int req_create(fs_e, inode_nr, omode, uid, gid, path, res)
int fs_e;
ino_t inode_nr;
int omode;
uid_t uid;
gid_t gid;
char *path;
node_details_t *res; 
{
    int r;
    cp_grant_id_t grant_id;
    size_t len;
    kipc_msg_t m;

    len= strlen(path) + 1;
    grant_id= cpf_grant_direct(fs_e, (vir_bytes)path, len, CPF_READ);
    if (grant_id == -1)
	panic(__FILE__, "req_create: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type	= REQ_CREATE;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_MODE = omode;
    m.REQ_UID = uid;
    m.REQ_GID = gid;
    m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
    cpf_revoke(grant_id);
  if (r != 0) return(r);

    /* Fill in response structure */
    res->fs_e = m.m_source;
    res->inode_nr = m.RES_INODE_NR;
    res->fmode = m.RES_MODE;
  res->fsize	= m.RES_FILE_SIZE_LO;
    res->uid = m.RES_UID;
    res->gid = m.RES_GID;
    res->dev = m.RES_DEV;

  return(0);
}


/*===========================================================================*
 *				req_flush	      			     *
 *===========================================================================*/
int req_flush(fs_e, dev)
endpoint_t fs_e; 
dev_t dev;
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_FLUSH;
    m.REQ_DEV = dev;
    
    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_fstatfs	    			     *
 *===========================================================================*/
int req_fstatfs(fs_e, who_e, buf)
int fs_e;
int who_e;
char *buf;
{
  int r;
  cp_grant_id_t grant_id;
  kipc_msg_t m;

  grant_id = cpf_grant_magic(fs_e, who_e, (vir_bytes) buf, sizeof(struct statfs),
		CPF_WRITE);
  if(grant_id == -1) 
	  panic(__FILE__, "req_fstatfs: cpf_grant_magic failed", NO_NUM);

  /* Fill in request message */
  m.m_type = REQ_FSTATFS;
  m.REQ_GRANT = grant_id;

  /* Send/rec request */
  r= fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  return(r);
}


/*===========================================================================*
 *				req_ftrunc	     			     *
 *===========================================================================*/
int req_ftrunc(fs_e, inode_nr, start, end)
endpoint_t fs_e;
ino_t inode_nr;
off_t start;
off_t end;
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_FTRUNC;
  m.REQ_INODE_NR = inode_nr;
  m.REQ_TRC_START_LO = start;
  m.REQ_TRC_START_HI = 0;	/* Not used for now, so clear it. */
  m.REQ_TRC_END_LO = end;
  m.REQ_TRC_END_HI = 0;		/* Not used for now, so clear it. */

    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_getdents	     			     *
 *===========================================================================*/
int req_getdents(fs_e, inode_nr, pos, buf, size, new_pos)
endpoint_t fs_e;
ino_t inode_nr;
u64_t pos;
char *buf;
size_t size;
u64_t *new_pos;
{
	int r;
	kipc_msg_t m;
  cp_grant_id_t grant_id;
  
  grant_id = cpf_grant_magic(fs_e, who_e, (vir_bytes) buf, size, CPF_WRITE);
  if (grant_id < 0)
  	panic(__FILE__, "req_getdents: cpf_grant_magic failed", grant_id);

	m.m_type= REQ_GETDENTS;
  m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;
  m.REQ_MEM_SIZE = size;
  m.REQ_SEEK_POS_LO = ex64lo(pos);
  m.REQ_SEEK_POS_HI = 0;	/* Not used for now, so clear it. */

	r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  if (r == 0) {
	  *new_pos = cvul64(m.RES_SEEK_POS_LO);
	  r = m.RES_NBYTES;
	}

  return(r);
}


/*===========================================================================*
 *				req_inhibread	  			     *
 *===========================================================================*/
int req_inhibread(fs_e, inode_nr)
endpoint_t fs_e;
ino_t inode_nr;
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_INHIBREAD;
    m.REQ_INODE_NR = inode_nr;

    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_link	       			     *
 *===========================================================================*/
int req_link(fs_e, link_parent, lastc, linked_file)
endpoint_t fs_e;
ino_t link_parent;
char *lastc;
ino_t linked_file;
{
    int r;
  cp_grant_id_t grant_id;
    size_t len;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
  grant_id = cpf_grant_direct(fs_e, (vir_bytes)lastc, len, CPF_READ);
  if(grant_id == -1)
	panic(__FILE__, "req_link: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_LINK;
  m.REQ_INODE_NR = linked_file;
  m.REQ_DIR_INO = link_parent;
  m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  return(r);
}
    

/*===========================================================================*
 *				req_lookup	                   	     *
 *===========================================================================*/
int req_lookup(fs_e, dir_ino, root_ino, uid, gid, flags, res)
endpoint_t fs_e;
ino_t dir_ino;
ino_t root_ino;
uid_t uid;
gid_t gid;
int flags;
lookup_res_t *res;
{
    int r;
    size_t len;
  cp_grant_id_t grant_id, grant_id2;
    kipc_msg_t m;
  vfs_ucred_t credentials;

    grant_id= cpf_grant_direct(fs_e, (vir_bytes)user_fullpath,
	sizeof(user_fullpath), CPF_READ|CPF_WRITE);
    if (grant_id == -1)
	  panic(__FILE__, "req_lookup: cpf_grant_direct failed", NO_NUM);

  len = strlen(user_fullpath) + 1;

  m.m_type		= REQ_LOOKUP;
  m.REQ_GRANT		= grant_id;
  m.REQ_PATH_LEN 	= len;
  m.REQ_PATH_SIZE 	= sizeof(user_fullpath);
  m.REQ_DIR_INO 	= dir_ino;
  m.REQ_ROOT_INO 	= root_ino;

  if(fp->fp_ngroups > 0) { /* Is the process member of multiple groups? */
  	/* In that case the FS has to copy the uid/gid credentials */
  	int i;

  	/* Set credentials */
  	credentials.vu_uid = fp->fp_effuid;
  	credentials.vu_gid = fp->fp_effgid;
  	credentials.vu_ngroups = fp->fp_ngroups;
  	for (i = 0; i < fp->fp_ngroups; i++) 
  		credentials.vu_sgroups[i] = fp->fp_sgroups[i];

	grant_id2 = cpf_grant_direct(fs_e, (vir_bytes) &credentials,
				     sizeof(credentials), CPF_READ);
	if(grant_id2 == -1)
		panic(__FILE__, "req_lookup: cpf_grant_direct failed", NO_NUM);

	m.REQ_GRANT2	= grant_id2;
	m.REQ_UCRED_SIZE= sizeof(credentials);
  	flags		|= PATH_GET_UCRED;
  } else {
  	/* When there's only one gid, we can send it directly */
	m.REQ_UID	= uid;
	m.REQ_GID	= gid;
	flags		&= ~PATH_GET_UCRED;
  }

  m.REQ_FLAGS		= flags;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
    cpf_revoke(grant_id);
  if(fp->fp_ngroups > 0) cpf_revoke(grant_id2); 

    /* Fill in response according to the return value */
    res->fs_e = m.m_source;
    switch (r) {
        case 0:
            res->inode_nr = m.RES_INODE_NR;
            res->fmode = m.RES_MODE;
	  res->fsize = m.RES_FILE_SIZE_LO;
            res->dev = m.RES_DEV;
	    res->uid= m.RES_UID;
	    res->gid= m.RES_GID;
            break;
        case -EENTERMOUNT:
            res->inode_nr = m.RES_INODE_NR;
            res->char_processed = m.RES_OFFSET;
	  res->symloop = m.RES_SYMLOOP;
	    break;
        case -ELEAVEMOUNT:
            res->char_processed = m.RES_OFFSET;
	  res->symloop = m.RES_SYMLOOP;
	    break;
        case -ESYMLINK:
            res->char_processed = m.RES_OFFSET;
	  res->symloop = m.RES_SYMLOOP;
            break;
	default:
	    break;
    }

  return(r);
}


/*===========================================================================*
 *				req_mkdir	      			     *
 *===========================================================================*/
int req_mkdir(fs_e, inode_nr, lastc, uid, gid, dmode)
endpoint_t fs_e;
ino_t inode_nr;
char *lastc;
uid_t uid;
gid_t gid;
mode_t dmode;
{
    int r;
    cp_grant_id_t grant_id;
    size_t len;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
    grant_id= cpf_grant_direct(fs_e, (vir_bytes)lastc, len, CPF_READ);
    if (grant_id == -1)
	panic(__FILE__, "req_mkdir: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_MKDIR;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_MODE = dmode;
    m.REQ_UID = uid;
    m.REQ_GID = gid;
    m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
    cpf_revoke(grant_id);

  return(r);
}


/*===========================================================================*
 *				req_mknod	      			     *
 *===========================================================================*/
int req_mknod(fs_e, inode_nr, lastc, uid, gid, dmode, dev)
endpoint_t fs_e;
ino_t inode_nr;
char *lastc;
uid_t uid;
gid_t gid;
mode_t dmode;
dev_t dev;
{
    int r;
    size_t len;
    cp_grant_id_t grant_id;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
    grant_id= cpf_grant_direct(fs_e, (vir_bytes)lastc, len, CPF_READ);
    if (grant_id == -1)
	panic(__FILE__, "req_mknod: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_MKNOD;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_MODE = dmode;
  m.REQ_DEV = dev;
    m.REQ_UID = uid;
    m.REQ_GID = gid;
    m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
    cpf_revoke(grant_id);

  return(r);
}


/*===========================================================================*
 *				req_mountpoint	                 	     *
 *===========================================================================*/
int req_mountpoint(fs_e, inode_nr)
endpoint_t fs_e;
ino_t inode_nr;
{
    int r;
    kipc_msg_t m;

    /* Fill in request message */
  m.m_type = REQ_MOUNTPOINT;
    m.REQ_INODE_NR = inode_nr;

    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_newnode	      			     *
 *===========================================================================*/
int req_newnode(fs_e, uid, gid, dmode, dev, res)
endpoint_t fs_e;
uid_t uid;
gid_t gid;
mode_t dmode;
dev_t dev;
struct node_details *res;
{
    int r;
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_NEWNODE;
    m.REQ_MODE = dmode;
  m.REQ_DEV = dev;
    m.REQ_UID = uid;
    m.REQ_GID = gid;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);

    res->fs_e = m.m_source;
    res->inode_nr = m.RES_INODE_NR;
    res->fmode = m.RES_MODE;
  res->fsize	= m.RES_FILE_SIZE_LO;
    res->dev = m.RES_DEV;
    res->uid= m.RES_UID;
    res->gid= m.RES_GID;

  return(r);
}


/*===========================================================================*
 *				req_newdriver          			     *
 *===========================================================================*/
int req_newdriver(fs_e, dev, driver_e)
endpoint_t fs_e;
dev_t dev;
endpoint_t driver_e;
{
/* Note: this is the only request function that doesn't use the 
 * fs_sendrec internal routine, since we want to avoid the dead
 * driver recovery mechanism here. This function is actually called 
 * during the recovery.
 */
    kipc_msg_t m;
    int r;

    /* Fill in request message */
    m.m_type = REQ_NEW_DRIVER;
    m.REQ_DEV = dev;
    m.REQ_DRIVER_E = driver_e;

    /* Issue request */
    if ((r = kipc_sendrec(fs_e, &m)) != 0) {
	  printf("%s:%d VFS req_newdriver: error sending message %d to %d\n",
		 __FILE__, __LINE__, r, fs_e);
	util_stacktrace();
	  return(r);
    }

  return(0);
}



/*===========================================================================*
 *				req_putnode				     *
 *===========================================================================*/
int req_putnode(fs_e, inode_nr, count)
int fs_e;
ino_t inode_nr;
int count;
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_PUTNODE;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_COUNT = count;

    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_rdlink	     			     *
 *===========================================================================*/
int req_rdlink(fs_e, inode_nr, who_e, buf, len)
endpoint_t fs_e;
ino_t inode_nr;
endpoint_t who_e;
char *buf;
size_t len;
{
    kipc_msg_t m;
    int r;
  cp_grant_id_t grant_id;

  grant_id = cpf_grant_magic(fs_e, who_e, (vir_bytes) buf, len, CPF_WRITE);
  if(grant_id == -1)
	panic(__FILE__, "req_rdlink: cpf_grant_magic failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_RDLINK;
    m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;
  m.REQ_MEM_SIZE = len;

    /* Send/rec request */
    r= fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  if(r == 0) r = m.RES_NBYTES;

  return(r);
}


/*===========================================================================*
 *				req_readsuper	                  	     *
 *===========================================================================*/
int req_readsuper(fs_e, label, dev, readonly, isroot, res_nodep)
endpoint_t fs_e;
char *label;
dev_t dev;
int readonly;
int isroot;
struct node_details *res_nodep;
{
  int r;
  cp_grant_id_t grant_id;
  size_t len;
  kipc_msg_t m;

  len = strlen(label)+1;
  grant_id = cpf_grant_direct(fs_e, (vir_bytes) label, len, CPF_READ);
  if (grant_id == -1)
          panic(__FILE__, "req_readsuper: cpf_grant_direct failed", NO_NUM);

  /* Fill in request message */
  m.m_type = REQ_READSUPER;
  m.REQ_FLAGS = 0;
  if(readonly) m.REQ_FLAGS |= REQ_RDONLY;
  if(isroot)   m.REQ_FLAGS |= REQ_ISROOT;
  m.REQ_GRANT = grant_id;
  m.REQ_DEV = dev;
  m.REQ_PATH_LEN = len;

  /* Send/rec request */
  r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  if(r == 0) {
        /* Fill in response structure */
        res_nodep->fs_e = m.m_source;
        res_nodep->inode_nr = m.RES_INODE_NR;
        res_nodep->fmode = m.RES_MODE;
        res_nodep->fsize = m.RES_FILE_SIZE_LO;
        res_nodep->uid = m.RES_UID;
        res_nodep->gid = m.RES_GID;
  }

  return(r);
}


/*===========================================================================*
 *				req_readwrite				     *
 *===========================================================================*/
int req_readwrite(fs_e, inode_nr, pos, rw_flag, user_e,
	user_addr, num_of_bytes, new_posp, cum_iop)
endpoint_t fs_e;
ino_t inode_nr;
u64_t pos;
int rw_flag;
endpoint_t user_e;
char *user_addr;
unsigned int num_of_bytes;
u64_t *new_posp;
unsigned int *cum_iop;
{
    int r;
  cp_grant_id_t grant_id;
    kipc_msg_t m;

    if (ex64hi(pos) != 0)
	panic(__FILE__, "req_readwrite: pos too large", NO_NUM);

  grant_id = cpf_grant_magic(fs_e, user_e, (vir_bytes) user_addr, num_of_bytes,
  			     (rw_flag==READING ? CPF_WRITE:CPF_READ));
  if (grant_id == -1)
	panic(__FILE__, "req_readwrite: cpf_grant_magic failed", NO_NUM);

    /* Fill in request message */
  m.m_type = rw_flag == READING ? REQ_READ : REQ_WRITE;
  m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;
  m.REQ_SEEK_POS_LO = ex64lo(pos);
  m.REQ_SEEK_POS_HI = 0;	/* Not used for now, so clear it. */
  m.REQ_NBYTES = num_of_bytes;
    
    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  if (r == 0) {
    /* Fill in response structure */
	*new_posp = cvul64(m.RES_SEEK_POS_LO);
	*cum_iop = m.RES_NBYTES;
  }

  return(r);
}


/*===========================================================================*
 *				req_rename	     			     *
 *===========================================================================*/
int req_rename(fs_e, old_dir, old_name, new_dir, new_name)
endpoint_t fs_e;
ino_t old_dir;
char *old_name;
ino_t new_dir;
char *new_name;
{
    int r;
    cp_grant_id_t gid_old, gid_new;
    size_t len_old, len_new;
    kipc_msg_t m;

    len_old= strlen(old_name) + 1;
  gid_old = cpf_grant_direct(fs_e, (vir_bytes) old_name, len_old, CPF_READ);
    if (gid_old == -1)
	panic(__FILE__, "req_rename: cpf_grant_direct failed", NO_NUM);

    len_new= strlen(new_name) + 1;
  gid_new = cpf_grant_direct(fs_e, (vir_bytes) new_name, len_new, CPF_READ);
    if (gid_new == -1)
	panic(__FILE__, "req_rename: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_RENAME;
    m.REQ_REN_OLD_DIR = old_dir;
    m.REQ_REN_NEW_DIR = new_dir;
    m.REQ_REN_GRANT_OLD = gid_old;
    m.REQ_REN_LEN_OLD = len_old;
    m.REQ_REN_GRANT_NEW = gid_new;
    m.REQ_REN_LEN_NEW = len_new;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);

    cpf_revoke(gid_old);
    cpf_revoke(gid_new);

  return(r);
}


/*===========================================================================*
 *				req_rmdir	      			     *
 *===========================================================================*/
int req_rmdir(fs_e, inode_nr, lastc)
endpoint_t fs_e;
ino_t inode_nr;
char *lastc;
{
    int r;
  cp_grant_id_t grant_id;
    size_t len;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
  grant_id = cpf_grant_direct(fs_e, (vir_bytes) lastc, len, CPF_READ);
  if(grant_id == -1)
	panic(__FILE__, "req_rmdir: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_RMDIR;
    m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  return(r);
}


/*===========================================================================*
 *				req_slink	      			     *
 *===========================================================================*/
int req_slink(fs_e, inode_nr, lastc, who_e, path_addr, path_length,
	uid, gid)
endpoint_t fs_e;
ino_t inode_nr;
char *lastc;
endpoint_t who_e;
char *path_addr;
unsigned short path_length;
uid_t uid;
gid_t gid;
{
    int r;
    size_t len;
    cp_grant_id_t gid_name, gid_buf;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
  gid_name = cpf_grant_direct(fs_e, (vir_bytes) lastc, len, CPF_READ);
    if (gid_name == -1)
	panic(__FILE__, "req_slink: cpf_grant_direct failed", NO_NUM);

  gid_buf = cpf_grant_magic(fs_e, who_e, (vir_bytes) path_addr, path_length,
			    CPF_READ);
  if(gid_buf == -1) {
	cpf_revoke(gid_name);
	panic(__FILE__, "req_slink: cpf_grant_magic failed", NO_NUM);
    }

    /* Fill in request message */
  m.m_type = REQ_SLINK;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_UID = uid;
    m.REQ_GID = gid;
    m.REQ_GRANT = gid_name;
    m.REQ_PATH_LEN = len;
  m.REQ_GRANT3 = gid_buf;
  m.REQ_MEM_SIZE = path_length;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);

    cpf_revoke(gid_name);
    cpf_revoke(gid_buf);

  return(r);
}


/*===========================================================================*
 *				req_stat	       			     *
 *===========================================================================*/
int req_stat(fs_e, inode_nr, who_e, buf, pos)
int fs_e;
ino_t inode_nr;
int who_e;
char *buf;
int pos;
{
  cp_grant_id_t grant_id;
  int r;
  kipc_msg_t m;
  struct stat sb;

  if (pos != 0)
	  grant_id = cpf_grant_direct(fs_e, (vir_bytes) &sb,
	  			      sizeof(struct stat), CPF_WRITE);
  else
	  grant_id = cpf_grant_magic(fs_e, who_e, (vir_bytes) buf,
				sizeof(struct stat), CPF_WRITE);

  if (grant_id < 0)
	panic(__FILE__, "req_stat: cpf_grant_* failed", NO_NUM);

  /* Fill in request message */
  m.m_type = REQ_STAT;
  m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;

  /* Send/rec request */
  r= fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  if (r == 0 && pos != 0) {
	sb.st_size -= pos;
	r= sys_vircopy(ENDPT_SELF, D, (vir_bytes)&sb, who_e, D, (vir_bytes)buf, 
		sizeof(struct stat));
  }

  return(r);
}


/*===========================================================================*
 *				req_sync	       			     *
 *===========================================================================*/
int req_sync(fs_e)
endpoint_t fs_e; 
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_SYNC;
    
    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_unlink	     			     *
 *===========================================================================*/
int req_unlink(fs_e, inode_nr, lastc)
endpoint_t fs_e;
ino_t inode_nr;
char *lastc;
{
  cp_grant_id_t grant_id;
    size_t len;
    int r;
    kipc_msg_t m;

    len= strlen(lastc) + 1;
  grant_id = cpf_grant_direct(fs_e, (vir_bytes) lastc, len, CPF_READ);
  if(grant_id == -1)
	panic(__FILE__, "req_unlink: cpf_grant_direct failed", NO_NUM);

    /* Fill in request message */
  m.m_type = REQ_UNLINK;
    m.REQ_INODE_NR = inode_nr;
  m.REQ_GRANT = grant_id;
    m.REQ_PATH_LEN = len;

    /* Send/rec request */
    r = fs_sendrec(fs_e, &m);
  cpf_revoke(grant_id);

  return(r);
}


/*===========================================================================*
 *				req_unmount	    			     *
 *===========================================================================*/
int req_unmount(fs_e)
endpoint_t fs_e; 
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_UNMOUNT;
    
    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}


/*===========================================================================*
 *				req_utime	      			     *
 *===========================================================================*/
int req_utime(fs_e, inode_nr, actime, modtime)
endpoint_t fs_e;
ino_t inode_nr;
time_t actime;
time_t modtime;
{
    kipc_msg_t m;

    /* Fill in request message */
    m.m_type = REQ_UTIME;
    m.REQ_INODE_NR = inode_nr;
    m.REQ_ACTIME = actime;
    m.REQ_MODTIME = modtime;

    /* Send/rec request */
    return fs_sendrec(fs_e, &m);
}




/*===========================================================================*
 *				fs_sendrec				     *
 *===========================================================================*/
static int fs_sendrec_f(char *file, int line, endpoint_t fs_e, kipc_msg_t *reqm)
{
/* This is the low level function that sends requests to FS processes.
 * It also handles driver recovery mechanism and reissuing the
 * request which failed due to a dead driver.
 */
  int r, old_driver_e, new_driver_e;
  kipc_msg_t origm, m;
  struct vmnt *vmp;

  if(fs_e <= 0 || fs_e == ENDPT_NONE)
	panic(__FILE__, "talking to bogus endpoint", fs_e);

  /* Make a copy of the request so that we can load it back in
   * case of a dead driver */
  origm = *reqm;

  /* In response to the request we sent, some file systems may send back their
   * own VFS request, instead of a reply. VFS currently offers limited support
   * for this. As long as the FS keeps sending requests, we process them and
   * send back a reply. We break out of the loop as soon as the FS sends a
   * reply to the original request.
   *
   * There is no form of locking or whatever on global data structures, so it
   * is quite easy to mess things up; hence, 'limited' support. A future async
   * VFS will solve this problem for good.
   */
  for (;;) {
      /* Do the actual send, receive */
      if (0 != (r=kipc_sendrec(fs_e, reqm))) {
		printf("VFS:fs_sendrec:%s:%d: error sending message. "
		       "FS_e: %d req_nr: %d err: %d\n", file, line, fs_e,
		       reqm->m_type, r);
	  util_stacktrace();
		return(r);
      }

      /* If the type field is 0 0 or negative (E*), this is a reply. If it
       * contains a positive nonzero value, this is a request.
       */
      if (reqm->m_type <= 0)
          break; /* Reply */

      if (reqm->m_type == -EENTERMOUNT || reqm->m_type == -ELEAVEMOUNT ||
        reqm->m_type == -ESYMLINK) {

          reqm->m_type = -reqm->m_type;

          break; /* Reply */
      }

      /* Request */
      nested_fs_call(reqm);
  }

#if 0
      if(r == 0) {
      	/* Sendrec was okay */
      	break;
      }
      /* Dead driver */
      if (r == -EDEADSRCDST || r == -EDSTDIED || r == -ESRCDIED) {
          old_driver_e = ENDPT_NONE;
          /* Find old driver by endpoint */
          for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
              if (vmp->m_fs_e == fs_e) {   /* found FS */
#if 0
                  old_driver_e = vmp->m_driver_e;
#endif
                  dmap_unmap_by_endpt(old_driver_e); /* unmap driver */
                  break;
              }
          }
         
          /* No FS?? */
          if (old_driver_e == ENDPT_NONE)
              panic(__FILE__, "VFSdead_driver: couldn't find FS\n", fs_e);

          /* Wait for a new driver. */
          for (;;) {
              new_driver_e = 0;
              printf("VFSdead_driver: waiting for new driver\n");
              r = kipc_receive(RS_PROC_NR, &m);
              if (r != 0) {
                  panic(__FILE__, "VFSdead_driver: unable to receive from RS", 
				  r);
              }
              if (m.m_type == KCNR_DEVCTL) {
                  /* Map new driver */
                  r = fs_devctl(m.ctl_req, m.dev_nr, m.driver_nr,
                          m.dev_style, m.m_force);
                  if (m.ctl_req == DEV_MAP && r == 0) {
                      new_driver_e = m.driver_nr;
                      printf("VFSdead_driver: new driver endpoint: %d\n",
                              new_driver_e);
                  }
              }
              else {
                  panic(__FILE__, "VFSdead_driver: got message from RS, type", 
                          m.m_type);
              }
              m.m_type = r;
              if ((r = kipc_send(RS_PROC_NR, &m)) != 0) {
                  panic(__FILE__, "VFSdead_driver: unable to send to RS",
                          r);
              }
              /* New driver is ready */
              if (new_driver_e) break;
          }
          
          /* Copy back original request */
          *reqm = origm;  
          continue;
      }

       printf("fs_sendrec: unhandled error %d sending to %d\n", r, fs_e);
       panic(__FILE__, "fs_sendrec: unhandled error", NO_NUM);
  }
#endif

  /* Return message type */
  return(reqm->m_type);
}

