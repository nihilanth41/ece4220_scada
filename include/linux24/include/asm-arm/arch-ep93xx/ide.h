/*****************************************************************************
 *  linux/include/asm-arm/arch-ep93xx/ide.h
 *
 *  IDE definitions for the EP93XX architecture
 *
 *
 *  Copyright (c) 2003 Cirrus Logic, Inc., All rights reserved.
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
 *   
 ****************************************************************************/
#ifndef ASM_ARCH_IDE_H
#define ASM_ARCH_IDE_H
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm/scatterlist.h>

/*
 *  Maximum number of IDE interfaces for this architecture is 1.
 */
#undef  MAX_HWIFS
#define MAX_HWIFS	1 

/*
 *  Default PIO mode used for setting up DMA commands
 */
#define DEFAULT_PIO_MODE	    4

/*
 *  ATA Command Register addresses.
 */

#define DATAREGISTER            0x00
#define ERRORREGISTER           0x01
#define FEATURESREGISTER        0x01
#define SECTORCOUNTREGISTER     0x02
#define SECTORNUMBERREGISTER    0x03
#define CYLINDERLOWREGISTER     0x04
#define CYLINDERHIGHREGISTER    0x05
#define DEVICEHEADREGISTER      0x06
#define COMMANDREGISTER         0x07
#define STATUSREGISTER          0x07

/*
 *  ATA Control Register addresses.
 */
#define DEVICECONTROLREGISTER   0x06
#define ALTERNATESTATUSREGISTER 0x06

/*
 *  ATA Register Bit Masks
 */
#define ATASRST  		        0x04
#define ATAnIEN  		        0x02
#define ATADEV   		        0x10
#define ATAABRT  		        0x04
#define ATABSY   		        0x80
#define ATADRDY  		        0x40
#define ATADRQ   		        0x08
#define ATAERR   		        0x01
#define ATADEVFAULT		        0x20
#define	ATAWRITEFAULT	        0x20
#define ATASERVICE 		        0x10
#define ATACORRECTED	        0x04
#define ATAINDEX		        0x02

#if 0
/*****************************************************************************
 *
 * Redefine OUT_BYTE and IN_BYTE for the ide interface on the ep93xx.
 *
 ****************************************************************************/
#define HAVE_ARCH_OUT_BYTE      1
#define OUT_BYTE(b, p)          ep93xx_ide_outb((b), (p))
#define OUT_WORD(w, p)          ep93xx_ide_outw((w), (p))

#define HAVE_ARCH_IN_BYTE       1
#define IN_BYTE(p)              ep93xx_ide_inb((p))
#define IN_WORD(p)              ep93xx_ide_inw((p))

#ifdef CONFIG_BLK_DEV_IDEDMA_EP93XX

#define EP93XX_DMA_TODEVICE     1
#define EP93XX_DMA_FROMDEVICE   2

/*****************************************************************************
 *
 * If dma support for IDE is enabled, the generic ide dma function has
 * to be redefined with an ep93xx specific function.
 *
 ****************************************************************************/
#define ide_dmaproc ep93xx_dmaproc
#define ide_release_dma ep93xx_ide_release_dma

/****************************************************************************
 *
 * Map a set of buffers described by scatterlist in streaming
 * mode for DMA.  This is the scather-gather version of the
 * above pci_map_single interface.  Here the scatter gather list
 * elements are each tagged with the appropriate dma address
 * and length.  They are obtained via sg_dma_{address,length}(SG).
 *
 * NOTE: An implementation may be able to use a smaller number of
 *       DMA address/length pairs than there are SG table elements.
 *       (for example via virtual mapping capabilities)
 *       The routine returns the number of addr/length pairs actually
 *       used, at most nents.
 *
 * Device ownership issues as mentioned above for pci_map_single are
 * the same here.
 *
 ****************************************************************************/
static inline int
ep93xx_map_sg(struct scatterlist * sg, unsigned int entries,
              unsigned int direction)
{
	unsigned int loop;
    
    for(loop = 0; loop < entries; loop++, sg++)
    {
		consistent_sync(sg->address, sg->length, direction);
		sg->dma_address = virt_to_bus(sg->address);
	}

	return entries;
}

/*****************************************************************************
 *
 * ep93xx_unmap_sg()
 *
 * Unmap a set of streaming mode DMA translations.
 * Again, cpu read rules concerning calls here are the same as for
 * pci_unmap_single() above.
 *
 ****************************************************************************/
static inline void
ep93xx_unmap_sg(struct scatterlist *sg, unsigned int entries,
                unsigned int direction)
{
    /*
     *  nothing to do
     */
}

#endif /* CONFIG_BLK_DEV_IDEDMA_EP93XX */
#endif //0

#ifdef CONFIG_BLK_DEV_EP93XX
/*****************************************************************************
 *
 * Set up a hw structure for a specified data port, control port and IRQ.
 * This should follow whatever the default interface uses.
 *
 ****************************************************************************/
static __inline__ void
ide_init_hwif_ports(hw_regs_t *hw, int data_port, int ctrl_port, int *irq)
{
    ide_ioreg_t reg; 
    int i;
    printk("ide_init_hwif_ports\n");
    
    /*
     *  Set up the IDE interface for PIO transfers, using the default PIO
     *  mode.
     */
    ep93xx_ide_regs->IDECFG.Field.PIO = 1;
    ep93xx_ide_regs->IDECFG.Field.MODE = DEFAULT_PIO_MODE;
    ep93xx_ide_regs->IDECFG.Field.WST = 0;
    
    /*
     *  Enable the IDE interface.
     */
    ep93xx_ide_regs->IDECFG.Field.IDEEN = 1;
    
    /*
     *  Set up the ide device command register offsets in the io_port array.
     *  This offset includes the register offset and the bit settings for 
     *  CS0n and CS1n.
     */
    reg = (ide_ioreg_t)data_port;
    for(i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++)
    {
        hw->io_ports[i] = (reg << 2 ) + 2;
	    reg += 1;

            printk(" io_ports: %08lx\n",hw->io_ports[i]);
    }
    
    /*
     *  Set up the register offset for the device control registers.
     *  This offset includes the register offset and the bit settings for 
     *  CS0n and CS1n.
     */
    reg = (ide_ioreg_t)ctrl_port;
    hw->io_ports[IDE_CONTROL_OFFSET] = (reg << 2) + 1;
            printk("-io_ports: %08lx\n",hw->io_ports[IDE_CONTROL_OFFSET]);
    
    
    if(irq)
    {
        *irq = 0;
    }
}

extern void ep93xx_ide_init(struct hwif_s * hwif);

/*****************************************************************************
 *
 * This registers the standard ports for this architecture with the IDE
 * driver.
 *
 ****************************************************************************/
static __inline__ void
ide_init_default_hwifs(void)
{
    hw_regs_t hw;

    struct hwif_s *hwif;
    unsigned int uiTemp;

    /*
     *  Make sure the GPIO on IDE bits in the DEVCFG register are not set.
     */
    uiTemp = inl(SYSCON_DEVCFG) & ~(SYSCON_DEVCFG_EonIDE |
                                    SYSCON_DEVCFG_GonIDE |
                                    SYSCON_DEVCFG_HonIDE);
	
    SysconSetLocked( SYSCON_DEVCFG, uiTemp );
    
    /*
     *  Initialize the IDE interface
     */
    ide_init_hwif_ports(&hw, DATAREGISTER, DEVICECONTROLREGISTER, NULL);
        
    /*
     *  Get the interrupt.
     */
    hw.irq = IRQ_EIDE;
    
    /*
     * This is the dma channel number assigned to this IDE interface. Until
     * dma is enabled for this interface, we set it to NO_DMA.
     */
    hw.dma = NO_DMA;    
    
    /*
     *  Register the IDE interface, an ide_hwif_t pointer is passed in,
     *  which will get filled in with the hwif pointer for this interface.
     */
    ide_register_hw(&hw, &hwif);
    
    /*
     *  Set up a pointer to the ep93xx ideproc function.
     */
    ep93xx_ide_init(hwif);
    
    printk("Cirrus Logic EP93XX IDE initialization - driver version 1.0, 5/21/03. \n");
}
#endif // CONFIG_BLK_DEV_EP93XX

#ifdef CONFIG_BLK_DEV_TSCF
extern void tscf_ide_init(struct hwif_s * hwif);
/*****************************************************************************
 *
 * Set up a hw structure for a specified data port, control port and IRQ.
 * This should follow whatever the default interface uses.
 *
 ****************************************************************************/
static __inline__ void
ide_init_hwif_ports(hw_regs_t *hw, int data_port, int ctrl_port, int *irq)
{
}

/*****************************************************************************
 *
 * This registers the standard ports for this architecture with the IDE
 * driver.
 *
 ****************************************************************************/
static __inline__ void
ide_init_default_hwifs(void)
{
    hw_regs_t hw;

    struct hwif_s *hwif;

    /*
     *  Initialize the IDE interface
     */
    hw.io_ports[IDE_DATA_OFFSET] =    TS7XXX_IDE16_BASE;
    hw.io_ports[IDE_ERROR_OFFSET] =   TS7XXX_IDE8_BASE;
    hw.io_ports[IDE_NSECTOR_OFFSET] = TS7XXX_IDE8_BASE + 0x1;
    hw.io_ports[IDE_SECTOR_OFFSET] =  TS7XXX_IDE8_BASE + 0x2;
    hw.io_ports[IDE_LCYL_OFFSET] =    TS7XXX_IDE8_BASE + 0x3;
    hw.io_ports[IDE_HCYL_OFFSET] =    TS7XXX_IDE8_BASE + 0x4;
    hw.io_ports[IDE_SELECT_OFFSET] =  TS7XXX_IDE8_BASE + 0x5;
    hw.io_ports[IDE_STATUS_OFFSET] =  TS7XXX_IDE8_BASE + 0x6;
    hw.io_ports[IDE_CONTROL_OFFSET] = TS7XXX_IDEAUX_BASE;

    /*
     *  Get the interrupt.
     */
    hw.irq = IRQ_EXT0;
    
    /*
     * This is the dma channel number assigned to this IDE interface. Until
     * dma is enabled for this interface, we set it to NO_DMA.
     */
    hw.dma = NO_DMA;    
    
    /*
     *  Register the IDE interface, an ide_hwif_t pointer is passed in,
     *  which will get filled in with the hwif pointer for this interface.
     */
    ide_register_hw(&hw, &hwif);

    
    /*
     *  Set up a pointer to the tscf ideproc function.
     */
    tscf_ide_init(hwif);
    
    printk("Technologic Systems TS-7XXX IDE initialization - driver version 1.3, 6/21/04. \n");
}
#endif // CONFIG_BLK_DEV_TSCF
#endif /* ASM_ARCH_IDE_H */
