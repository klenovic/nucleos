#ifndef __KERNEL_CLOCK_H
#define __KERNEL_CLOCK_H

#include <kernel/kernel.h>
#include <asm/kernel/clock.h>

int boot_cpu_init_timer(unsigned freq);

int bsp_timer_int_handler(void);
int ap_timer_int_handler(void);

int arch_init_local_timer(unsigned freq);
void arch_stop_local_timer(void);
int arch_register_local_timer_handler(irq_handler_t handler);

#endif /* __KERNEL_CLOCK_H */
