#ifndef __NUCLEOS_DEBUG_H
#define __NUCLEOS_DEBUG_H

/* For reminders of things to be fixed. */
#define FIXME(str) { \
	static int fixme_warned = 0; \
	if(!fixme_warned) { \
		printk("FIXME: %s:%d: %s\n", __FILE__, __LINE__, str); \
		fixme_warned = 1; \
	} \
}

#endif /* __NUCLEOS_DEBUG_H */
