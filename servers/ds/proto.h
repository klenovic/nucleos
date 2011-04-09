/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Function prototypes. */

/* main.c */
int main(int argc, char **argv);

/* store.c */
int do_publish(kipc_msg_t *m_ptr);
int do_retrieve(kipc_msg_t *m_ptr);
int do_subscribe(kipc_msg_t *m_ptr);
int do_check(kipc_msg_t *m_ptr);
int do_getsysinfo(kipc_msg_t *m_ptr);
void ds_init(void);
