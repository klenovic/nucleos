/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Function prototypes. */

/* exec.c */
int dev_execve(int proc_e, char *exec, size_t exec_len,  char *argv[], char **env);

/* main.c */
int main(void);

/* manager.c */
int do_up(message *m, int do_copy, int flags);
int do_start(message *m);
int do_down(message *m);
int do_refresh(message *m);
int do_restart(message *m);
int do_lookup(message *m);
int do_shutdown(message *m);
void do_period(message *m);
void do_exit(message *m);
int do_getsysinfo(message *m);


