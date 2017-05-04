/*
 *  linux/include/asm-arm/arch-integrator/timex.h
 *
 *  Integrator architecture timex specifications
 *
 *  Copyright (C) 1999 ARM Limited
 *
 * Copyright (C) 2004-2005 Michael Neuhauser, Firmix Software GmbH (mike@firmix.at)
 * 	corrected CLOCK_TICK_RATE, added defines for exact frequency
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

/*
 * ??
 */
#ifndef _ASM_ARM_ARCH_EP93XX_H
#define _ASM_ARM_ARCH_EP93XX_H

/*
 * The original value of 508000 is completly wrong. Table 4-2 on page 74 of the
 * EP9301 User's Manual gives the frequency as 508.4689 kHz and specifies that
 * all timer clocks are derived from the external 14.7456 MHz oscillator by
 * division. Hence, the correct frequency is most likely
 * 14745.6 / 29 = 508.468965... kHz (rounded integer: 508469 Hz).
 * -- Michael Neuhauser <mike@firimix.at>
 */
#define CLOCK_TICK_RATE		(508469)

/* exact value (non-standard macros) */
#define CLOCK_TICK_RATE_NUM	(14745600)
#define CLOCK_TICK_RATE_DEN	(29)

#endif /* _ASM_ARM_ARCH_EP93XX_H */
