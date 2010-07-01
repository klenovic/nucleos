/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
int env_parse(char *env, char *fmt, int field, long *param, long min, long max);
void env_panic(char *env);
int env_prefix(char *env, char *prefix);
int env_memory_parse(struct memory *chunks, int nchunks);

