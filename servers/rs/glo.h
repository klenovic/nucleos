/* Global variables used in RS.
 */
#ifndef __RS_GLO_H
#define __RS_GLO_H

/* The boot image priv table. This table has entries for all system
 * services in the boot image.
 */
extern struct boot_image_priv boot_image_priv_table[];

/* The boot image sys table. This table has entries for system services in
 * the boot image that override default sys properties.
 */
extern struct boot_image_sys boot_image_sys_table[];

/* The boot image dev table. This table has entries for system services in
 * the boot image that support dev properties.
 */
extern struct boot_image_dev boot_image_dev_table[];

/* The system process table. This table only has entries for system
 * services (servers and drivers), and thus is not directly indexed by
 * slot number.
 */
extern struct rproc rproc[NR_SYS_PROCS];
extern struct rproc *rproc_ptr[NR_PROCS];       /* mapping for fast access */

/* Pipe for detection of exec failures. The pipe is close-on-exec, and
 * no data will be written to the pipe if the exec succeeds. After an 
 * exec failure, the slot number is written to the pipe. After each exit,
 * a non-blocking read retrieves the slot number from the pipe.
 */
extern int exec_pipe[2];

/* Enable/disable verbose output. */
extern long rs_verbose;

#endif /* __RS_GLO_H */
