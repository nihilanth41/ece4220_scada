#ifndef __ASM_ARM_IRQ_H
#define __ASM_ARM_IRQ_H

#include <linux/linkage.h>
#include <asm/arch/irqs.h>

#ifndef irq_cannonicalize
#define irq_cannonicalize(i)	(i)
#endif

#ifndef NR_IRQS
#define NR_IRQS	128
#endif

/*
 * Use this value to indicate lack of interrupt
 * capability
 */
#ifndef NO_IRQ
#define NO_IRQ	((unsigned int)(-1))
#endif

#define disable_irq_nosync(i) disable_irq(i)

extern void disable_irq(unsigned int);
extern void enable_irq(unsigned int);
extern asmlinkage void asm_do_IRQ(int irq, struct pt_regs *regs);


#endif

