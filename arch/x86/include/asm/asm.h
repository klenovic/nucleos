#ifndef _ASM_X86_ASM_H
#define _ASM_X86_ASM_H

#ifdef __ASSEMBLY__
# define __ASM_FORM(x)	x
#else
# define __ASM_FORM(x)	" " #x " "
#endif

#ifdef CONFIG_X86_32
# define __ASM_SEL(a,b)	__ASM_FORM(a)
#else
# define __ASM_SEL(a,b)	__ASM_FORM(b)
#endif

#define _ASM_PTR	__ASM_SEL(.long, .quad)
#define _ASM_ALIGN	__ASM_SEL(.balign 4, .balign 8)

#endif /* _ASM_X86_ASM_H */
