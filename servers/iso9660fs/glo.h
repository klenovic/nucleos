#ifndef __SERVERS_ISO9660FS_GLO_H
#define __SERVERS_ISO9660FS_GLO_H

/* The following variables are used for returning results to the caller. */

extern int err_code;		/* temporary storage for error number */
extern int rdwt_err;		/* status of last disk i/o request */

extern int (*fs_call_vec[])(void); /* fs call table */

extern message fs_m_in;		/* contains the input message of the request */
extern message fs_m_out;	/* contains the output message of the 
				 * request */
extern int FS_STATE;

extern uid_t caller_uid;
extern gid_t caller_gid;

extern int req_nr;		/* request number to the server */

extern int SELF_E;		/* process number */

extern short path_processed;      /* number of characters processed */
extern char user_path[];  	/* pathname to be processed */
extern char *vfs_slink_storage;
extern int symloop;

extern dev_t fs_dev;    /* the device that is handled by this FS proc */
extern char fs_dev_label[]; /* Name of the device driver that is handled */

extern int use_getuptime2; /* Should be removed togetherwith boottime */

#endif /* __SERVERS_ISO9660FS_GLO_H */
