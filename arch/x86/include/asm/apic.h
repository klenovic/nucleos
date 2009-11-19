#ifndef __ASM_X86_APIC_H
#define __ASM_X86_APIC_H

#define LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#define IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#define LAPIC_ID	(lapic_addr + 0x020)
#define LAPIC_VERSION	(lapic_addr + 0x030)
#define LAPIC_TPR	(lapic_addr + 0x080)
#define LAPIC_EOI	(lapic_addr + 0x0b0)
#define LAPIC_LDR	(lapic_addr + 0x0d0)
#define LAPIC_DFR	(lapic_addr + 0x0e0)
#define LAPIC_SIVR	(lapic_addr + 0x0f0)
#define LAPIC_ESR	(lapic_addr + 0x280)
#define LAPIC_ICR1	(lapic_addr + 0x300)
#define LAPIC_ICR2	(lapic_addr + 0x310)
#define LAPIC_LVTTR	(lapic_addr + 0x320)
#define LAPIC_LVTTMR	(lapic_addr + 0x330)
#define LAPIC_LVTPCR	(lapic_addr + 0x340)
#define LAPIC_LINT0	(lapic_addr + 0x350)
#define LAPIC_LINT1	(lapic_addr + 0x360)
#define LAPIC_LVTER	(lapic_addr + 0x370)
#define LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#define LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#define LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#define IOAPIC_ID			0x0
#define IOAPIC_VERSION			0x1
#define IOAPIC_ARB			0x2
#define IOAPIC_REDIR_TABLE		0x10

#define APIC_TIMER_INT_VECTOR		0xfe
#define APIC_SPURIOUS_INT_VECTOR	0xff

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

#ifdef CONFIG_DEBUG_X86_LOCAL_APIC

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
extern char lapic_intr_dummy_handles_start;
extern char lapic_intr_dummy_handles_end;
#endif

#endif /* CONFIG_DEBUG_X86_LOCAL_APIC */

/* The local APIC timer tick handlers */
void lapic_bsp_timer_int_handler(void);
void lapic_ap_timer_int_handler(void);

extern int ioapic_enabled;
extern u32_t lapic_addr;
extern u32_t lapic_eoi_addr;
extern u32_t lapic_taskpri_addr;
extern int bsp_lapic_id;

#define MAX_NR_IOAPICS			32
#define MAX_NR_BUSES			32
#define MAX_NR_APICIDS			255
#define MAX_NR_LCLINTS			2

extern u8_t apicid2cpuid[MAX_NR_APICIDS+1];
extern unsigned apic_imcrp;
extern unsigned nioapics;
extern unsigned nbuses;
extern unsigned nintrs;
extern unsigned nlints;

extern u32_t ioapic_id_mask[8];
extern u32_t lapic_id_mask[8];
extern u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */
extern u32_t lapic_addr;
extern u32_t lapic_eoi_addr;
extern u32_t lapic_taskpri_addr;

void calc_bus_clock(void);
u32_t lapic_errstatus(void);
/*
u32_t ioapic_read(u32_t addr, u32_t offset);
void ioapic_write(u32_t addr, u32_t offset, u32_t data);
void lapic_eoi(void);
*/
void lapic_microsec_sleep(unsigned count);
void smp_ioapic_unmask(void);
void ioapic_disable_irqs(u32_t irq);
void ioapic_enable_irqs(u32_t irq);
u32_t ioapic_irqs_inuse(void);
void smp_recv_ipi(int arg);
void ioapic_config_pci_irq(u32_t data);

int lapic_enable(void);
void lapic_disable(void);

void ioapic_disable_all(void);
int ioapic_enable_all(void);

void apic_idt_init(int reset);

int apic_single_cpu_init(void);

void lapic_set_timer_periodic(unsigned freq);
void lapic_stop_timer(void);

#include <asm/cpufeature.h>

#define cpu_feature_apic_on_chip() cpufeature(_CPUF_I386_APIC_ON_CHIP)

#define lapic_read(what)	(*((u32_t *)((char*)(what))))
#define lapic_write(what, data)	do {					\
	(*((u32_t *)((char*)(what)))) = data;			\
} while(0)

#endif /* __ASSEMBLY__ */

#endif /* __ASM_X86_KERNEL_APIC_H */
