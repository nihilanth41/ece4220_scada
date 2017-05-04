/*
 *  linux/include/asm-arm/arch-ep93xx/time.h
 *
 * (c) Copyright 2001 LynuxWorks, Inc., San Jose, CA.  All rights reserved.
 *
 * Copyright (C) 2002-2003 Cirrus Logic, Inc.
 *
 * Copyright (C) 2004-2005 Michael Neuhauser, Firmix Software GmbH (mike@firmix.at)
 * 	more Adeos/RTAI friendly
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <asm/system.h>
#include <asm/leds.h>
#include <asm/arch/hardware.h>
#include <asm/arch/ep93xx_tsc.h>

/* First timer channel used for timer interrupt */

/*
 * IRQ handler for the timer
 *
 * interrupts are disabled because timer_irq has SA_INTERRUPT set (see
 * arch/arm/time.c) (of course not if Adeos is active)
 */
static void
ep93xx_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    /* Note, that the irq needs to be acked on a timer-hardware specific
     * level (i.e. write 1 to TIMER1CLEAR). But because
     * irq_desc[IRQ_TIMER1].mask_ack does that (non-standard modification), it
     * does not have to be done here. This makes this function safe to use when
     * RTAI/Adeos is active and handles the interrupts */
    do_leds();
    {
	/* full jiffies according to TSC (i.e. absolute clock) */
	unsigned long long realtime_jiffies =
	    (ep93xx_read_and_update_tsc() * HZ) / FREQ_EP93XX_TSC;
	/* call do_timer() as often as it is necessary to keep jiffies in sync
	 * with TSC, take care that jiffies are never ahead the TSC
	 * (gettimeoffset is unsigned) */
	while (ep93xx_jiffies_done < realtime_jiffies) {
    do_timer(regs);
	    ++ep93xx_jiffies_done;
	}
    }
    do_profile(regs);
}

/*
 * Set up timer interrupt
 */
extern inline void
setup_timer(void)
{
    ep93xx_setup_timer();
    timer_irq.handler = ep93xx_timer_interrupt;
    setup_arm_irq(IRQ_TIMER1, &timer_irq);

    /*
     * Start timer 1, leave others alone
     */
    outl( 0, TIMER1CONTROL );

    /*
     * Since the clock is giving u 2 uSeconds per tick,
     * the timer load value is the timer interval
     * divided by 2. 
     */
    outl( 5085, TIMER1LOAD );
    outl( 5085, TIMER1VALUE);  /* countdown */

    /*
     * Set up Timer 1 for 508 kHz clock and periodic mode.
     */ 
    outl( 0xC8, TIMER1CONTROL );
}


#ifdef CONFIG_ADEOS_CORE
static inline void
__adeos_set_timer(unsigned long hz)
{
    unsigned long delay = (CLOCK_TICK_RATE + hz/2) / hz - 1;
    outl(0, TIMER1CONTROL);         	/* stop timer */
    outl(delay, TIMER1LOAD);		/* set load value */
    outl(0xc8, TIMER1CONTROL);		/* set 508 kHz clock, periodic mode & enable timer */
}
#endif /* CONFIG_ADEOS_CORE */
