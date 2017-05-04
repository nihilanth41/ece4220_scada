/*
 *  linux/include/asm-arm/arch-ep93xx/system.h
 *
 *  Copyright (C) 1999 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd
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
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <linux/config.h>
#include <asm/arch/platform.h>
#include <asm/hardware.h>
#include <asm/io.h>

static inline void arch_idle(void)
{
	cpu_do_idle();
}

extern inline void arch_reset(char mode)
{
    //
    // Enable the watchdog timer so that it will reset the part.
    //

    // Use TS-7200 watchdog
    outw(0x5, TS7XXX_WATCHDOG_FEED);
    outw(0x1, TS7XXX_WATCHDOG_CTRL);

    // Sometimes it seems Cirrus EP9301 watchdog reset puts the processor in
    // extremely bad states drawing lots of current/overheating...  This
    // is reproducible 1 in 20 resets!  This is why we implemented our
    // own watchdog in the TS-7200.  Cirrus knows of problems with their
    // watchdog, but as of 9/10/2004, still has not released public errata
    // concerning this... sigh...  --Jesse Off, Technologic Systems
    // 
    // outl(0xaaaa, WATCHDOG); // Cirrus watchdog doesn't always work

    while (1);
}
#endif
