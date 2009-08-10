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

/* main.c */
int main(int argc, char **argv);

/* store.c */
int do_publish(message *m_ptr);
int do_retrieve(message *m_ptr);
int do_subscribe(message *m_ptr);
int do_check(message *m_ptr);
int do_getsysinfo(message *m_ptr);
void ds_init(void);
