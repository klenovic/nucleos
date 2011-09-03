/* Helpers for initial module or kernel cmdline parsing
   Copyright (C) 2001 Rusty Russell.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <nucleos/moduleparam.h>
#include <nucleos/kernel.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <nucleos/ctype.h>
#include <kernel/proto.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt, a...)
#endif

/* @nucleos: Fake linux functions */
#define mutex_lock(lock)
#define mutex_unlock(lock)
#define EXPORT_SYMBOL(symbol)

static inline char dash2underscore(char c)
{
	if (c == '-')
		return '_';
	return c;
}

static inline int parameq(const char *input, const char *paramname)
{
	unsigned int i;
	for (i = 0; dash2underscore(input[i]) == paramname[i]; i++)
		if (input[i] == '\0')
			return 1;
	return 0;
}

static int parse_one(char *param,
		     char *val,
		     const struct kernel_param *params,
		     unsigned num_params,
		     int (*handle_unknown)(char *param, char *val))
{
	unsigned int i;
	int err;

	/* Find parameter */
	for (i = 0; i < num_params; i++) {
		if (parameq(param, params[i].name)) {
			/* No one handled NULL, so do it here. */
			if (!val && params[i].ops->set != param_set_bool)
				return -EINVAL;
			DEBUGP("They are equal!  Calling %p\n",
			       params[i].ops->set);
			mutex_lock(&param_lock);
			err = params[i].ops->set(val, &params[i]);
			mutex_unlock(&param_lock);
			return err;
		}
	}

	if (handle_unknown) {
		DEBUGP("Unknown argument: calling %p\n", handle_unknown);
		return handle_unknown(param, val);
	}

	DEBUGP("Unknown argument `%s'\n", param);
	return -ENOENT;
}

/* You can use " around spaces, but can't escape ". */
/* Hyphens and underscores equivalent in parameter names. */
static char *next_arg(char *args, char **param, char **val)
{
	unsigned int i, equals = 0;
	int in_quote = 0, quoted = 0;
	char *next;

	if (*args == '"') {
		args++;
		in_quote = 1;
		quoted = 1;
	}

	for (i = 0; args[i]; i++) {
		if (isspace(args[i]) && !in_quote)
			break;
		if (equals == 0) {
			if (args[i] == '=')
				equals = i;
		}
		if (args[i] == '"')
			in_quote = !in_quote;
	}

	*param = args;
	if (!equals)
		*val = NULL;
	else {
		args[equals] = '\0';
		*val = args + equals + 1;

		/* Don't include quotes in value. */
		if (**val == '"') {
			(*val)++;
			if (args[i-1] == '"')
				args[i-1] = '\0';
		}
		if (quoted && args[i-1] == '"')
			args[i-1] = '\0';
	}

	if (args[i]) {
		args[i] = '\0';
		next = args + i + 1;
	} else
		next = args + i;

	/* Chew up trailing spaces. */
	return skip_spaces(next);
}

/* Args looks like "foo=bar,bar2 baz=fuz wiz". */
int parse_args(const char *name,
	       char *args,
	       const struct kernel_param *params,
	       unsigned num,
	       int (*unknown)(char *param, char *val))
{
	char *param, *val;

	DEBUGP("Parsing ARGS: %s\n", args);

	/* Chew leading spaces */
	args = skip_spaces(args);

	while (*args) {
		int ret;

		args = next_arg(args, &param, &val);
		ret = parse_one(param, val, params, num, unknown);
		switch (ret) {
		case -ENOENT:
			printk("ERROR: %s: Unknown parameter `%s'\n",
			       name, param);
			return ret;
		case -ENOSPC:
			printk("ERROR: %s: `%s' too large for parameter `%s'\n",
			       name, val ?: "", param);
			return ret;
		case 0:
			break;
		default:
			printk("ERROR: %s: `%s' invalid for parameter `%s'\n",
			       name, val ?: "", param);
			return ret;
		}
	}

	/* All parsed OK. */
	return 0;
}

/* Lazy bastard, eh? */
#define STANDARD_PARAM_DEF(name, type, format, tmptype, strtolfn)      	\
	int param_set_##name(const char *val, const struct kernel_param *kp) \
	{								\
		tmptype l;						\
		int ret;						\
									\
		ret = strtolfn(val, 0, &l);				\
		if (ret == -EINVAL || ((type)l != l))			\
			return -EINVAL;					\
		*((type *)kp->arg) = l;					\
		return 0;						\
	}								\
	int param_get_##name(char *buffer, const struct kernel_param *kp) \
	{								\
		return sprintf(buffer, format, *((type *)kp->arg));	\
	}								\
	struct kernel_param_ops param_ops_##name = {			\
		.set = param_set_##name,				\
		.get = param_get_##name,				\
	};								\
	EXPORT_SYMBOL(param_set_##name);				\
	EXPORT_SYMBOL(param_get_##name);				\
	EXPORT_SYMBOL(param_ops_##name)


STANDARD_PARAM_DEF(byte, unsigned char, "%c", unsigned long, strict_strtoul);
STANDARD_PARAM_DEF(short, short, "%hi", long, strict_strtol);
STANDARD_PARAM_DEF(ushort, unsigned short, "%hu", unsigned long, strict_strtoul);
STANDARD_PARAM_DEF(int, int, "%i", long, strict_strtol);
STANDARD_PARAM_DEF(uint, unsigned int, "%u", unsigned long, strict_strtoul);
STANDARD_PARAM_DEF(long, long, "%li", long, strict_strtol);
STANDARD_PARAM_DEF(ulong, unsigned long, "%lu", unsigned long, strict_strtoul);

int param_set_charp(const char *val, const struct kernel_param *kp)
{
	if (strlen(val) > 1024) {
		printk("ERROR: %s: string parameter too long\n",
		       kp->name);
		return -ENOSPC;
	}

	*(const char **)kp->arg = val;

	return 0;
}
EXPORT_SYMBOL(param_set_charp);

int param_get_charp(char *buffer, const struct kernel_param *kp)
{
	return sprintf(buffer, "%s", *((char **)kp->arg));
}
EXPORT_SYMBOL(param_get_charp);

struct kernel_param_ops param_ops_charp = {
	.set = param_set_charp,
	.get = param_get_charp,
};
EXPORT_SYMBOL(param_ops_charp);

/* Actually could be a bool or an int, for historical reasons. */
int param_set_bool(const char *val, const struct kernel_param *kp)
{
	bool v;
	int ret;

	/* No equals means "set"... */
	if (!val) val = "1";

	/* One of =[yYnN01] */
	ret = strtobool(val, &v);
	if (ret)
		return ret;

	if (kp->flags & KPARAM_ISBOOL)
		*(bool *)kp->arg = v;
	else
		*(int *)kp->arg = v;
	return 0;
}
EXPORT_SYMBOL(param_set_bool);

int param_get_bool(char *buffer, const struct kernel_param *kp)
{
	bool val;
	if (kp->flags & KPARAM_ISBOOL)
		val = *(bool *)kp->arg;
	else
		val = *(int *)kp->arg;

	/* Y and N chosen as being relatively non-coder friendly */
	return sprintf(buffer, "%c", val ? 'Y' : 'N');
}
EXPORT_SYMBOL(param_get_bool);

struct kernel_param_ops param_ops_bool = {
	.set = param_set_bool,
	.get = param_get_bool,
};
EXPORT_SYMBOL(param_ops_bool);

/* This one must be bool. */
int param_set_invbool(const char *val, const struct kernel_param *kp)
{
	int ret;
	bool boolval;
	struct kernel_param dummy;

	dummy.arg = &boolval;
	dummy.flags = KPARAM_ISBOOL;
	ret = param_set_bool(val, &dummy);
	if (ret == 0)
		*(bool *)kp->arg = !boolval;
	return ret;
}
EXPORT_SYMBOL(param_set_invbool);

int param_get_invbool(char *buffer, const struct kernel_param *kp)
{
	return sprintf(buffer, "%c", (*(bool *)kp->arg) ? 'N' : 'Y');
}
EXPORT_SYMBOL(param_get_invbool);

struct kernel_param_ops param_ops_invbool = {
	.set = param_set_invbool,
	.get = param_get_invbool,
};
EXPORT_SYMBOL(param_ops_invbool);

/* We break the rule and mangle the string. */
static int param_array(const char *name,
		       const char *val,
		       unsigned int min, unsigned int max,
		       void *elem, int elemsize,
		       int (*set)(const char *, const struct kernel_param *kp),
		       u16 flags,
		       unsigned int *num)
{
	int ret;
	struct kernel_param kp;
	char save;

	/* Get the name right for errors. */
	kp.name = name;
	kp.arg = elem;
	kp.flags = flags;

	*num = 0;
	/* We expect a comma-separated list of values. */
	do {
		int len;

		if (*num == max) {
			printk("ERROR:" "%s: can only take %i arguments\n",
			       name, max);
			return -EINVAL;
		}
		len = strcspn(val, ",");

		/* nul-terminate and parse */
		save = val[len];
		((char *)val)[len] = '\0';
		ret = set(val, &kp);

		if (ret != 0)
			return ret;
		kp.arg += elemsize;
		val += len+1;
		(*num)++;
	} while (save == ',');

	if (*num < min) {
		printk("ERROR:" "%s: needs at least %i arguments\n",
		       name, min);
		return -EINVAL;
	}
	return 0;
}

static int param_array_set(const char *val, const struct kernel_param *kp)
{
	const struct kparam_array *arr = kp->arr;
	unsigned int temp_num;

	return param_array(kp->name, val, 1, arr->max, arr->elem,
			   arr->elemsize, arr->ops->set, kp->flags,
			   arr->num ?: &temp_num);
}

static int param_array_get(char *buffer, const struct kernel_param *kp)
{
	int i, off, ret;
	const struct kparam_array *arr = kp->arr;
	struct kernel_param p;

	p = *kp;
	for (i = off = 0; i < (arr->num ? *arr->num : arr->max); i++) {
		if (i)
			buffer[off++] = ',';
		p.arg = arr->elem + arr->elemsize * i;
		ret = arr->ops->get(buffer + off, &p);
		if (ret < 0)
			return ret;
		off += ret;
	}
	buffer[off] = '\0';
	return off;
}

static void param_array_free(void *arg)
{
	unsigned int i;
	const struct kparam_array *arr = arg;

	if (arr->ops->free)
		for (i = 0; i < (arr->num ? *arr->num : arr->max); i++)
			arr->ops->free(arr->elem + arr->elemsize * i);
}

struct kernel_param_ops param_array_ops = {
	.set = param_array_set,
	.get = param_array_get,
	.free = param_array_free,
};
EXPORT_SYMBOL(param_array_ops);

int param_set_copystring(const char *val, const struct kernel_param *kp)
{
	const struct kparam_string *kps = kp->str;

	if (strlen(val)+1 > kps->maxlen) {
		printk("ERROR:" "%s: string doesn't fit in %u chars.\n",
		       kp->name, kps->maxlen-1);
		return -ENOSPC;
	}
	strcpy(kps->string, val);
	return 0;
}
EXPORT_SYMBOL(param_set_copystring);

int param_get_string(char *buffer, const struct kernel_param *kp)
{
	const struct kparam_string *kps = kp->str;
	return strlcpy(buffer, kps->string, kps->maxlen);
}
EXPORT_SYMBOL(param_get_string);

struct kernel_param_ops param_ops_string = {
	.set = param_set_copystring,
	.get = param_get_string,
};
EXPORT_SYMBOL(param_ops_string);
