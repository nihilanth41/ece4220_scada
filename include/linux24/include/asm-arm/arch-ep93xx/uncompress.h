/*
 *  linux/include/asm-arm/arch-ep93xx/uncompress.h
 *
 *  Copyright (C) 1999 ARM Limited
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

#include <asm/io.h>
#include <asm/hardware.h>

#define BAUDRATE ((14745600/(16 * 115200)) - 1)

static void puts(const char *s)
{
    while (*s)
    {
	    while(*HW_REG(UART1FR) & UARTFR_TXFF );

        *HW_REG(UART1DR) = *s;
        
	    if (*s == '\n')
        {
	        while (*HW_REG(UART1FR) & UARTFR_TXFF);
	    
            *HW_REG(UART1DR) = '\r';
	    }
	    s++;
    }
    while(*HW_REG(UART1FR) & UARTFR_BUSY);
}

static void EnableUart(void)
{
    //
    // Ensure that the base clock to the UARTS is 14.7456Mhz
    // and that UART1 is enabled.
    //
    *HW_REG(SYSCON_SWLOCK) = 0xaa;
    *HW_REG(SYSCON_PWRCNT) = *HW_REG(SYSCON_PWRCNT) | SYSCON_PWRCNT_UARTBAUD;

    *HW_REG(SYSCON_SWLOCK) = 0xaa;
    *HW_REG(SYSCON_DEVCFG) = *HW_REG(SYSCON_DEVCFG) | SYSCON_DEVCFG_U1EN;

    //
    // Disable the UARTS and clear out the old settings.
    //
    *HW_REG(UART1MCR) = 0;
    *HW_REG(UART1CR) = 0;
    *HW_REG(UART1RSR) = 0;
    *HW_REG(UART1CR_L) = 0;
    *HW_REG(UART1CR_M) = 0;
    *HW_REG(UART1CR_H) = 0;

    //
    // Set the baud rate and framing
    //
    *HW_REG(UART1CR_L) = (BAUDRATE & 0x0FF);
    *HW_REG(UART1CR_M) = ((BAUDRATE >> 8) & 0x0FF);

    //
    // 8 Data bits, No Parity, 1 Stop bit.
    //
    *HW_REG(UART1CR_H) = (UARTLCR_H_WLEN_8_DATA | UARTLCR_H_FEN);

    //
    // Enable Uart
    //
    *HW_REG(UART1CR) = UARTCR_UARTE;
}

//
// Called early before the UART is used for output by the compressed loader.
//
static void arch_decomp_setup()
{
    EnableUart();
}

#define arch_decomp_wdog()
