/*
 * linux/include/asm-arm/arch-ep93xx/ep93xx_rdtsc.h
 *
 * 64 bit time-stamp counter (TSC) for absolute real-time clock without
 * overflow-problems (>1/2 million years).
 *
 * Copyright (C) 2004-2005 Michael Neuhauser, Firmix Software GmbH <mike@firmix.at>
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
#ifndef __ASM_ARM_ARCH_EP93XX_H_
#define __ASM_ARM_ARCH_EP93XX_H_

/* tsc's clock frequency (EP93XX's timer 4) */
#define FREQ_EP93XX_TSC	(983040)

#ifdef __KERNEL__

#include <linux/config.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

/* jiffies counter without overflow problems (overflow time: ~6 billion years
 * @ HZ=100) to keep Linux-jiffies in sync with 64 bit TSC */
extern volatile unsigned long long ep93xx_jiffies_done;

/*
 * 64 bit tsc from hardware's 32 bit tsc, hardware has actually 40 bits, but
 * even this is not enough to avoid an overflow during a reboot-less time (for
 * a TSC clock frequency of 983040 Hz):
 *	bits	overflow
 *	32	1.2 hours
 *	40	13 days
 *	64	595034 years
 */
typedef union _ep93xx_tsc_t {
    unsigned long long	ll;
    struct {
	unsigned long	low;
	unsigned long	high;
    } u;
} ep93xx_tsc_t;
extern volatile ep93xx_tsc_t ep93xx_tsc;

/* read EP93XX-based 64 bit TSC irq-safe (overflow time: ~600000 years) */
extern inline unsigned long long
ep93xx_rdtsc(void)
{
    register ep93xx_tsc_t tsc;
    register unsigned long low_now;
    
    /* get snapshot of global tsc (ldm is irq-safe!) */
    __asm__(
	"ldmia %[glbl_tsc], %M[tsc]\n"
	: /* output */	[tsc] "=r" (tsc.ll)
	: /* input  */	[glbl_tsc] "r" (&ep93xx_tsc.ll),
			"m" (ep93xx_tsc)
    );
    /* read current low tsc (i.e. low 32 bits of hardware timer) */
    low_now = inl(TIMER4VALUELOW);
    /* check for overflow in relation to global ep93xx_tsc and correct high 32
     * bits if necessary */
    if (low_now < tsc.u.low)
	++tsc.u.high;
    tsc.u.low = low_now;
    return tsc.ll;
}

/* irq-safe read & update in-memory tsc (good place to call this is the 100 HZ
 * interrupt) */
extern inline unsigned long long
ep93xx_read_and_update_tsc(void)
{
    register ep93xx_tsc_t tsc;
    register unsigned long low_now;
    int do_update;

    /* get snapshot of global tsc (ldm is irq safe!) */
    __asm__(
	"ldmia %[glbl_tsc], %M[tsc]\n"
	: /* output */	[tsc] "=r" (tsc.ll)
	: /* input  */	[glbl_tsc] "r" (&ep93xx_tsc.ll),
			"m" (ep93xx_tsc)
    );
    /* read current low tsc (i.e. low 32 bits of hardware timer) */
    low_now = inl(TIMER4VALUELOW);
    /* check for overflow in relation to global ep93xx_tsc and correct high 32
     * bits if necessary */
    if (low_now < tsc.u.low)
	++tsc.u.high;
    /* update at least 4 times per period (i.e. each 18 minutes) */
    do_update = (low_now - tsc.u.low >= 0x40000000);
    tsc.u.low = low_now;
    if (do_update) {
	/* update global tsc (stm is irq-safe!) */
	__asm__(
	    "stmia %[glbl_tsc], %M[tsc]\n"
	    : /* output */	"=m" (ep93xx_tsc)
	    : /* input  */	[tsc] "r" (tsc.ll),
	    			[glbl_tsc] "r" (&ep93xx_tsc.ll)
	);
    }

    return tsc.ll;
}

#endif /* __KERNEL */

#endif /* __ASM_ARM_ARCH_EP93XX_H_ */
