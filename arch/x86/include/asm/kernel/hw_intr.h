#ifndef __ASM_X86_KERNEL_HW_INTR_H
#define __ASM_X86_KERNEL_HW_INTR_H

#include <kernel/kernel.h>

/* legacy PIC */

int irq_8259_unmask(int irq);
int irq_8259_mask(int irq);
void irq_handle(int irq);

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)

#endif /* __ASM_X86_KERNEL_HW_INTR_H */
