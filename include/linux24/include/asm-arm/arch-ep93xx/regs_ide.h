/*****************************************************************************
 *
 *  linux/include/asm-arm/arch-ep93xx/regs_ide.h
 *
 *  Register definitions for the ep93xx ide registers.
 *
 *  Copyright (C) 2003 Cirrus Logic
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
#ifndef _REGS_IDE_H_
#define _REGS_IDE_H_

#ifndef __ASSEMBLY__ 

/*****************************************************************************
 *
 * IDE register definitions
 *
 ****************************************************************************/
typedef struct _ide_regs_s
{
    union
    {
        unsigned int Value;
    } IDECR;                         /* 00 */

    union
    {
        struct
        {
            unsigned int IDEEN:1;   /* IDE master enable                    */
            unsigned int PIO:1;     /* Polled IO operation selection        */
            unsigned int MDMA:1;    /* Multiword DMA operation selection    */
            unsigned int UDMA:1;    /* Ultra DMA operation selection        */
            unsigned int MODE:4;    /* Speed mode number (0 - 4 for PIO,    */
                                    /* 0 - 2 for MDMA, 0 - 4 for UDMA)      */
            unsigned int WST:2;     /* Wait State for Turn.  Number of      */
                                    /* HCLK cycles to hold the data bus     */
                                    /* after                                */
                                    /* PIO write operation                  */
            unsigned int RAZ:22;    /* read only always zero                */
        }Field;
        unsigned int Value;
    }IDECFG;                        /* 04 */

    union
    {
        struct
        {
            unsigned int MEN:1;     /* Enable Multiword DMA operation.      */
                                    /* 1 - start mdma, 0 - terminate        */
                                    /* mdma operation by host.              */
            unsigned int RWOP:1;    /* Read or write operation selection:   */
                                    /* 0 = read, 1 = write.                 */
            unsigned int RAZ:30;    /* read only always zero                */
        }Field;
        unsigned int Value;
    }IDEMDMAOP;                     /* 08 */

    union
    {
        struct
        {
            unsigned int UEN:1;     /* Enable Ultra DMA operation           */
                                    /* 1 - start udma, 0 - terminate        */
                                    /* udma operation by host.              */
            unsigned int RWOP:1;    /* Read or write operation selection:   */
                                    /* 0 = read, 1 = write.                 */
            unsigned int RAZ:30;    /* read only always zero                */
        }Field;
        unsigned int Value;
    }IDEUDMAOP;                     /* 0C */

    union
    {
        unsigned int Value;
    }IDEDATAOUT;                    /* 10 */

    union
    {
        unsigned int Value;
    }IDEDATAIN;                     /* 14 */

    union
    {
        struct
        {
            unsigned int IDEDD:32;  /* IDE output data in the output buffer */
                                    /* in MDMA mode.  This register should  */
                                    /* only be written to by the DMA engine.*/
        }Field;
        unsigned int Value;
    }IDEMDMADATAOUT;                /* 18 */

    union
    {
        struct
        {
            unsigned int IDEDD:32;  /* IDE input data in the input buffer   */
                                    /* in MDMA mode.                        */
        }Field;
        unsigned int Value;
    }IDEMDMADATAIN;                 /* 1C */

    union
    {
        struct
        {
            unsigned int IDEDD:32;  /* IDE output data at the tail of the   */
                                    /* output buffer in UDMA mode.          */
        }Field;
        unsigned int Value;
    }IDEUDMADATAOUT;                /* 20 */

    union
    {
        struct
        {
            unsigned int IDEDD:32;  /* IDE input data at teh head of the    */
                                    /* input buffer in UDMA mode.           */
        }Field;
        unsigned int Value;
    }IDEUDMADATAIN;                 /* 24 */

    union
    {
        struct
        {
            unsigned int CS0n:1;    /* Chip select pin 0 status. Should     */
                                    /* be driven to 1 (deasserted) in UDMA  */
            unsigned int CS1n:1;    /* Chip select pin 1 status. Should     */
                                    /* be driven to 1 (deasserted) in UDMA  */
            unsigned int DA:3;      /* Device address status, should be     */
                                    /* driven to 0 (deasserted) in UDMA     */
            unsigned int HSHD:1;    /* HSTROBE (during data out) status,    */
                                    /* HDMARDYn (during data in) status,    */
                                    /* driven by UDMA state machine.        */
            unsigned int STOP:1;    /* STOP (during data out) status, driven*/
                                    /* by UDMA state machine.               */
            unsigned int DM:1;      /* DMACKn status, driven by UDMA state  */
                                    /* machine.                             */
            unsigned int DDOE:1;    /* DD bus output enable as controlled   */
                                    /* by UDMA state machine.               */
            unsigned int DMARQ:1;   /* Synchronized version of DMARQ input  */
                                    /* from device.                         */
            unsigned int DSDD:1;    /* DSTROBE (during data in) and DDMARDYn*/
                                    /* (during data out) status from device.*/
            unsigned int RAZ1:5;    /* reserved, always zero                */
            unsigned int DMAIDE:1;  /* DMA request signal from UDMA state   */
                                    /* machine.                             */
            unsigned int INTIDE:1;  /* INT line generated by UDMA state     */
                                    /* machine.                             */
            unsigned int SBUSY:1;   /* UDMA state machine busy, not in idle */
                                    /* state.                               */
            unsigned int RAZ2:5;    /* Reserved, always zero.               */
            unsigned int NDO:1;     /* Error for data out not completed     */
            unsigned int NDI:1;     /* Error for data in not completed      */
            unsigned int N4X:1;     /* Error for data transferred not       */
                                    /* multiples of 4 32 bit words.         */
            unsigned int RAZ3:5;    /* Reserved, always zero                */
        }Field;
        unsigned int Value;
    }IDEUDMASTATUS;                 /* 28 */
                                                                            
    union                                                                   
    {                                                                       
        struct                                                              
        {
            unsigned int RWOE:1;    /* Reset UDMA write data out error.     */
            unsigned int RWPTR:1;   /* Reset UDMA write buffer ptr to 0.    */
            unsigned int RWDR:1;    /* Reset UDMA write DMA request.        */
            unsigned int RROE:1;    /* Reset UDMA read data in error.       */
            unsigned int RRPTR:1;   /* Reset UDMA read buffer ptr to 0.     */
            unsigned int RRDR:1;    /* Reset UDMA read DMA request.         */
            unsigned int RAZ:26;    /* reserved, always zero                */
        }Field;
        unsigned int Value;
    }IDEUDMADEBUG;                  /* 2C */

    union
    {
        struct
        {
            unsigned int HPTR:4;    /* Head pointer in the write buffer     */
            unsigned int TPTR:4;    /* Tail pointer in the write buffer     */
            unsigned int EMPTY:1;   /* Write buffer empty status            */
            unsigned int HOM:1;     /* Half or more entries in write buffer */
                                    /* filled status                        */
            unsigned int NFULL:1;   /* Write buffer near full status        */
            unsigned int FULL:1;    /* Write buffer full status             */
            unsigned int RAZ:4;     /* Reserved, always zero                */
            unsigned int CRC:16;    /* CRC result for data operation        */
                                    /* TODO: is there a CRC?                */
        }Field;
        unsigned int Value;
    }IDEUDMAWFST;                   /* 30 */

    union
    {
        struct
        {
            unsigned int HPTR:4;    /* Head pointer in the read buffer      */
            unsigned int TPTR:4;    /* Tail pointer in the read buffer      */
            unsigned int EMPTY:1;   /* Read buffer empty status             */
            unsigned int HOM:1;     /* Half or more entries in read buffer  */
                                    /* filled status                        */
            unsigned int NFULL:1;   /* Read buffer near full status         */
            unsigned int FULL:1;    /* Read buffer full status              */
            unsigned int RAZ:4;     /* Reserved, always zero                */
            unsigned int CRC:16;    /* CRC result for data operation        */
                                    /* TODO: is there a CRC?                */
        } Field;
        unsigned int Value;
    } IDEUDMARFST;                   /* 34 */

} IDEREGISTERS;


/*****************************************************************************
 *
 * Global Register Anchor Definitions (these address values will change)
 *
 ****************************************************************************/
static volatile IDEREGISTERS * const ep93xx_ide_regs = (IDEREGISTERS *)(IO_ADDRESS(IDE_BASE));

#endif  /* Not __ASSEMBLY__ */

#define IDECtrl             HW_REG(0x800a0000)
#define IDECfg              HW_REG(0x800a0004)
#define IDEMDMAOp           HW_REG(0x800a0008)
#define IDEUDMAOp           HW_REG(0x800a000c)
#define IDEDataOut          HW_REG(0x800a0010)
#define IDEDataIn           HW_REG(0x800a0014)
#define IDEMDMADataOut      HW_REG(0x800a0018)
#define IDEMDMADataIn       HW_REG(0x800a001c)
#define IDEUDMADataOut      HW_REG(0x800a0020)
#define IDEUDMADataIn       HW_REG(0x800a0024)
#define IDEUDMASts          HW_REG(0x800a0028)
#define IDEUDMADebug        HW_REG(0x800a002c)
#define IDEUDMAWrBufSts     HW_REG(0x800a0030)
#define IDEUDMARdBufSts     HW_REG(0x800a0034)

/*****************************************************************************
 *
 *  Bit definitions for use with assembly code for the ide control register.
 *
 ****************************************************************************/
#define IDECtrl_CS0n          0x00000001
#define IDECtrl_CS1n          0x00000002
#define IDECtrl_DA_MASK       0x0000001c
#define IDECtrl_DA_SHIFT      2
#define IDECtrl_DIORn         0x00000020
#define IDECtrl_DIOWn         0x00000040
#define IDECtrl_DASPn         0x00000080
#define IDECtrl_DMARQ         0x00000100
#define IDECtrl_INTRQ         0x00000200
#define IDECtrl_IORDY         0x00000400

#endif /* _REGS_IDE_H_ */
