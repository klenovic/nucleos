#ifndef _NUCLEOS_DEBUG_H
#define _NUCLEOS_DEBUG_H

/* For reminders of things to be fixed. */
#define FIXME(str) { \
	static int fixme_warned = 0; \
	if(!fixme_warned) { \
		printk("FIXME: %s:%d: %s\n", __FILE__, __LINE__, str); \
		fixme_warned = 1; \
	} \
}

#endif /* _NUCLEOS_DEBUG_H */
