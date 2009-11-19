#ifndef __ASM_X86_APIC_INTR_H
#define __ASM_X86_APIC_INTR_H

#ifndef __ASSEMBLY__
#include <kernel/kernel.h>

void apic_hwint00(void);
void apic_hwint01(void);
void apic_hwint02(void);
void apic_hwint03(void);
void apic_hwint04(void);
void apic_hwint05(void);
void apic_hwint06(void);
void apic_hwint07(void);
void apic_hwint08(void);
void apic_hwint09(void);
void apic_hwint10(void);
void apic_hwint11(void);
void apic_hwint12(void);
void apic_hwint13(void);
void apic_hwint14(void);
void apic_hwint15(void);

/* The local APIC timer tick handlers */
void lapic_bsp_timer_int_handler(void);
void lapic_ap_timer_int_handler(void);

#endif

#ifdef CONFIG_DEBUG_X86_LOCAL_APIC

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN char lapic_intr_dummy_handles_start;
EXTERN char lapic_intr_dummy_handles_end;
#endif

#endif /* CONFIG_DEBUG_X86_LOCAL_APIC */

#endif /* __ASM_X86_APIC_ASM_H */
