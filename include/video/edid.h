#ifndef _VIDEO_EDID_H
#define _VIDEO_EDID_H

#if !defined(__KERNEL__) || defined(CONFIG_X86)

struct edid_info {
	unsigned char dummy[128];
};

#ifdef __KERNEL__
extern struct edid_info edid_info;
#endif /* __KERNEL__ */

#endif

#endif /* _VIDEO_EDID_H */
