#ifndef __ASM_X86_KERNEL_CLOCK_H
#define __ASM_X86_KERNEL_CLOCK_H

int init_8253A_timer(unsigned freq);
void stop_8253A_timer(void);
clock_t read_8253A_timer(void);

#endif /* __ASM_X86_KERNEL_CLOCK_H */
