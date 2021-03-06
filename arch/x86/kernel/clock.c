/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* i386-specific clock functions. */

#include <ibm/ports.h>
#include <nucleos/portio.h>

#include <kernel/clock.h>

#ifdef CONFIG_X86_LOCAL_APIC
#include <asm/apic.h>
#endif

#define CLOCK_ACK_BIT   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#define COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#define LATCH_COUNT     0x00    /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#define TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#define TIMER_COUNT(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

static irq_hook_t pic_timer_hook;		/* interrupt handler hook */

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
int init_8253A_timer(unsigned freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
   */
	outb(TIMER_MODE, SQUARE_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_COUNT(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_COUNT(freq) >> 8); /* timer high byte */

	return 0;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
void stop_8253A_timer(void)
{
/* Reset the clock to the BIOS rate. (For rebooting.) */
  outb(TIMER_MODE, 0x36);
  outb(TIMER0, 0);
  outb(TIMER0, 0);
}

/*===========================================================================*
 *				read_8235A_timer			     *
 *===========================================================================*/
clock_t read_8253A_timer(void)
{
	/* Read the counter of channel 0 of the 8253A timer.  This counter
	 * counts down at a rate of TIMER_FREQ and restarts at
	 * TIMER_COUNT-1 when it reaches zero. A hardware interrupt
	 * (clock tick) occurs when the counter gets to zero and restarts
	 * its cycle.
 */
	u32_t count;

  outb(TIMER_MODE, LATCH_COUNT);
  count = inb(TIMER0);
  count |= (inb(TIMER0) << 8);
  
  return count;
}

int arch_init_local_timer(unsigned freq)
{
#ifdef CONFIG_X86_LOCAL_APIC
	/* if we know the address, lapic is enabled and we should use it */
	if (lapic_addr) {
		lapic_set_timer_periodic(freq);
	} else
	{
		BOOT_VERBOSE(printk("Initiating legacy i8253 timer\n"));
#else
	{
#endif
		init_8253A_timer(freq);
	}

	return 0;
}

void arch_stop_local_timer(void)
{
#ifdef CONFIG_X86_LOCAL_APIC
	if (lapic_addr) {
		lapic_stop_timer();
	} else
#endif
	{
		stop_8253A_timer();
	}
}

int arch_register_local_timer_handler(irq_handler_t handler)
{
#ifdef CONFIG_X86_LOCAL_APIC
	if (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(printk("Using LAPIC timer as tick source\n"));
	} else
#endif
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = ENDPT_NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	return 0;
}
