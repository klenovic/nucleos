/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	queryparam.h - query program parameters		Author: Kees J. Bot
 *								22 Apr 1994
 */
#ifndef _NUCLEOS_QUERYPARAM_H
#define _NUCLEOS_QUERYPARAM_H

typedef size_t _mnx_size_t;

struct export_param_list {
	char	*name;		/* "variable", "[", ".field", or NULL. */
	void	*offset;	/* Address of a variable or field offset. */
	size_t	size;		/* Size of the resulting object. */
};

struct export_params {
	struct export_param_list *list;	/* List of exported parameters. */
	struct export_params	 *next;	/* Link several sets of parameters. */
};

#define qp_stringize(var)	#var
#define qp_dotstringize(var)	"." #var
#define QP_VARIABLE(var)	{ qp_stringize(var), &(var), sizeof(var) }
#define QP_ARRAY(var)		{ "[", 0, sizeof((var)[0]) }
#define QP_VECTOR(var,ptr,len)	{ qp_stringize(var), &(ptr), -1 },\
				{ "[", &(len), sizeof(*(ptr)) }
#define QP_FIELD(field, type)	{ qp_dotstringize(field), \
					(void *)offsetof(type, field), \
					sizeof(((type *)0)->field) }
#define QP_END()		{ 0, 0, 0 }

void qp_export(struct export_params *_ex_params);
int queryparam(int (*_qgetc)(void), void **_paddress, _mnx_size_t *_psize);
_mnx_size_t paramvalue(char **_value, void *_address, _mnx_size_t _size);
#endif /* _NUCLEOS_QUERYPARAM_H */
