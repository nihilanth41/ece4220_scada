/*
 *   include/asm-arm/adeos.h
 *
 *   Copyright (C) 2002,2003,2004 Philippe Gerum.
 *
 *   Copyright (C) 2004-2005 Michael Neuhauser, Firmix Software GmbH (mike@firmix.at)
 *	support for EP9301, backport of unthreaded support from Adeos/x86 for 2.6
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __ARM_ADEOS_H
#define __ARM_ADEOS_H

struct task_struct;

#include <asm/irq.h>
#include <asm/siginfo.h>
#include <asm/ptrace.h>
#include <asm/bitops.h>
#include <linux/list.h>
#include <linux/threads.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>

#ifdef CONFIG_ARCH_EP9301
#define ADEOS_ARCH_STRING	"r17c3/arm-ep9301"
#define ADEOS_MAJOR_NUMBER	17
#define ADEOS_MINOR_NUMBER	3
#define ADEOS_TIMER_IRQ		IRQ_TIMER1
#else
#error "Adeos: unsupported ARM architecture, sorry..."
#endif /* CONFIG_ARCH_EP9301 */

typedef unsigned long cpumask_t;
#define CPU_MASK_NONE		((cpumask_t)0)

#define ADEOS_NR_CPUS          1
#define adeos_processor_id()   0
/* Array references using this index should be optimized out. */
#define adeos_declare_cpuid    const int cpuid = 0
#define adeos_load_cpuid()      do { /* nop */ } while(0)
#define adeos_lock_cpu(flags)   adeos_hw_local_irq_save(flags)
#define adeos_unlock_cpu(flags) adeos_hw_local_irq_restore(flags)
#define adeos_get_cpu(flags)    do { flags = flags; } while(0)
#define adeos_put_cpu(flags)    do { /* nop */ } while(0)
#define adp_current             (adp_cpu_current[0])

/* ARM fault traps */
#define ADEOS_NR_FAULTS         32
/* Pseudo-vectors used for kernel events */
#define ADEOS_FIRST_KEVENT      ADEOS_NR_FAULTS
#define ADEOS_SYSCALL_PROLOGUE  (ADEOS_FIRST_KEVENT)
#define ADEOS_SYSCALL_EPILOGUE  (ADEOS_FIRST_KEVENT + 1)
#define ADEOS_SCHEDULE_HEAD     (ADEOS_FIRST_KEVENT + 2)
#define ADEOS_SCHEDULE_TAIL     (ADEOS_FIRST_KEVENT + 3)
#ifdef CONFIG_ADEOS_EVENT_ENTER_PROCESS
#define ADEOS_ENTER_PROCESS     (ADEOS_FIRST_KEVENT + 4)
#endif /* CONFIG_ADEOS_EVENT_ENTER_PROCESS */
#define ADEOS_EXIT_PROCESS      (ADEOS_FIRST_KEVENT + 5)
#define ADEOS_SIGNAL_PROCESS    (ADEOS_FIRST_KEVENT + 6)
#define ADEOS_KICK_PROCESS      (ADEOS_FIRST_KEVENT + 7)
#define ADEOS_LAST_KEVENT       (ADEOS_KICK_PROCESS)
#ifdef CONFIG_ADEOS_EVENT_RENICE_PROCESS
#define ADEOS_RENICE_PROCESS    (ADEOS_FIRST_KEVENT + 8)
#undef ADEOS_LAST_KEVENT
#define ADEOS_LAST_KEVENT       (ADEOS_RENICE_PROCESS)
#endif /* CONFIG_ADEOS_EVENT_RENICE_PROCESS */
#ifdef CONFIG_ADEOS_EVENT_USER_PROCESS
#define ADEOS_USER_EVENT        (ADEOS_FIRST_KEVENT + 9)
#undef ADEOS_LAST_KEVENT
#define ADEOS_LAST_KEVENT       (ADEOS_USER_EVENT)
#endif /* !CONFIG_ADEOS_EVENT_USER_PROCESS */

#define ADEOS_NR_EVENTS         (ADEOS_LAST_KEVENT + 1)

typedef struct adevinfo {

    unsigned domid;
    unsigned event;
    void *evdata;

    volatile int propagate;	/* Private */

} adevinfo_t;

typedef struct adsysinfo {

    int ncpus;			/* Number of CPUs on board */

    unsigned long long cpufreq;	/* CPU frequency (in Hz) */

    /* Arch-dependent block */

    struct {
	unsigned tmirq;		/* Timer tick IRQ */
    } archdep;

} adsysinfo_t;

#define IPIPE_NR_XIRQS   NR_IRQS
/* Number of virtual IRQs */
#define IPIPE_NR_VIRQS   BITS_PER_LONG
/* First virtual IRQ # */
#define IPIPE_VIRQ_BASE  (((IPIPE_NR_XIRQS + BITS_PER_LONG - 1) / BITS_PER_LONG) * BITS_PER_LONG)
/* Total number of IRQ slots */
#define IPIPE_NR_IRQS     (IPIPE_VIRQ_BASE + IPIPE_NR_VIRQS)
/* Number of indirect words needed to map the whole IRQ space. */
#define IPIPE_IRQ_IWORDS  ((IPIPE_NR_IRQS + BITS_PER_LONG - 1) / BITS_PER_LONG)
#define IPIPE_IRQ_IMASK   (BITS_PER_LONG - 1)
#define IPIPE_IRQ_ISHIFT  5	/* 2^5 for 32bits arch. */

#define IPIPE_IRQMASK_ANY   (~0UL)
#define IPIPE_IRQMASK_VIRT  (IPIPE_IRQMASK_ANY << (IPIPE_VIRQ_BASE / BITS_PER_LONG))

typedef struct adomain {

    /* -- Section: offset-based references may be made on these fields
       from inline assembly code. Please don't move or reorder. */
    void (*dswitch)(void)	/* Domain switch hook */
	__attribute__ ((__aligned__ (32))); /* align every element of array (gcc bug workaround) */
#ifdef CONFIG_ADEOS_THREADS
    int *esp[ADEOS_NR_CPUS];	/* Domain stack pointers */
#endif /* CONFIG_ADEOS_THREADS */
    /* -- End of section. */

    struct list_head p_link;	/* Link in pipeline */

    struct adcpudata {
	volatile unsigned long status;
	volatile unsigned long irq_pending_hi;
	volatile unsigned long irq_pending_lo[IPIPE_IRQ_IWORDS];
	volatile unsigned irq_hits[IPIPE_NR_IRQS];
#ifdef CONFIG_ADEOS_THREADS
	adevinfo_t event_info;
#endif /* CONFIG_ADEOS_THREADS */
    } cpudata[ADEOS_NR_CPUS];

    struct {
	int (*acknowledge)(unsigned irq);
	void (*handler)(unsigned irq);
	unsigned long control;
    } irqs[IPIPE_NR_IRQS];

    struct {
	void (*handler)(adevinfo_t *evinfo);
    } events[ADEOS_NR_EVENTS];

    unsigned long flags;

    struct adomain *m_link;	/* Link in mutex sleep queue */

    unsigned domid;

    const char *name;

    int priority;

    int ptd_keymax;
    int ptd_keycount;
    unsigned long ptd_keymap;
    void (*ptd_setfun)(int, void *);
    void *(*ptd_getfun)(int);

#ifdef CONFIG_ADEOS_THREADS
    int *estackbase[ADEOS_NR_CPUS];
#endif /* CONFIG_ADEOS_THREADS */

} adomain_t __attribute__ ((__aligned__ (32)));

#define adeos_hw_test_iflag(x)        (!((x) & I_BIT)) /* We don't use the FIRQs */

#define adeos_hw_irqs_disabled()	\
({					\
	unsigned long flags;		\
	adeos_hw_local_irq_flags(flags);	\
	!adeos_hw_test_iflag(flags);	\
})

/* The following macros must be used hw interrupts off. */

#define __adeos_set_irq_bit(adp,cpuid,irq) \
do { \
    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled()); \
    if (!test_bit(IPIPE_LOCK_FLAG,&(adp)->irqs[irq].control)) { \
        __set_bit(irq & IPIPE_IRQ_IMASK,&(adp)->cpudata[cpuid].irq_pending_lo[irq >> IPIPE_IRQ_ISHIFT]); \
        __set_bit(irq >> IPIPE_IRQ_ISHIFT,&(adp)->cpudata[cpuid].irq_pending_hi); \
       } \
} while(0)

#define __adeos_clear_pend(adp,cpuid,irq) \
do { \
    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled()); \
    __clear_bit(irq & IPIPE_IRQ_IMASK,&(adp)->cpudata[cpuid].irq_pending_lo[irq >> IPIPE_IRQ_ISHIFT]); \
    if ((adp)->cpudata[cpuid].irq_pending_lo[irq >> IPIPE_IRQ_ISHIFT] == 0) \
        __clear_bit(irq >> IPIPE_IRQ_ISHIFT,&(adp)->cpudata[cpuid].irq_pending_hi); \
} while(0)

#define __adeos_lock_irq(adp,cpuid,irq) \
do { \
    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled()); \
    if (!__test_and_set_bit(IPIPE_LOCK_FLAG,&(adp)->irqs[irq].control)) \
	__adeos_clear_pend(adp,cpuid,irq); \
} while(0)

#define __adeos_unlock_irq(adp,irq) \
do { \
    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled()); \
    int __cpuid, __nr_cpus = smp_num_cpus;			       \
    if (__test_and_clear_bit(IPIPE_LOCK_FLAG,&(adp)->irqs[irq].control)) \
	for (__cpuid = 0; __cpuid < __nr_cpus; __cpuid++)      \
         if ((adp)->cpudata[__cpuid].irq_hits[irq] > 0) { \
           __set_bit(irq & IPIPE_IRQ_IMASK,&(adp)->cpudata[__cpuid].irq_pending_lo[irq >> IPIPE_IRQ_ISHIFT]); \
           __set_bit(irq >> IPIPE_IRQ_ISHIFT,&(adp)->cpudata[__cpuid].irq_pending_hi); \
         } \
} while(0)

#define __adeos_clear_irq(adp,irq) \
do { \
    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled()); \
    int __cpuid, __nr_cpus = smp_num_cpus; \
    __clear_bit(IPIPE_LOCK_FLAG,&(adp)->irqs[irq].control); \
    for (__cpuid = 0; __cpuid < __nr_cpus; __cpuid++) {	\
       (adp)->cpudata[__cpuid].irq_hits[irq] = 0; \
       __adeos_clear_pend(adp,__cpuid,irq); \
    } \
} while(0)

#define adeos_virtual_irq_p(irq)		((irq) >= IPIPE_VIRQ_BASE && (irq) < IPIPE_NR_IRQS)

#define adeos_hw_save_flags_and_sti(x)		do { adeos_hw_local_irq_flags(x); adeos_hw_sti(); } while(0)

#define adeos_hw_tsc(t)				0
#define adeos_cpu_freq()			CLOCK_TICK_RATE

#ifdef CONFIG_PREEMPT
#define adeos_spin_lock(x)   			_raw_spin_lock(x)
#define adeos_spin_unlock(x) 			_raw_spin_unlock(x)
#define adeos_spin_trylock(x)			_raw_spin_trylock(x)
#else /* !CONFIG_PREEMPT */
#define adeos_spin_lock(x)   			spin_lock(x)
#define adeos_spin_unlock(x) 			spin_unlock(x)
#define adeos_spin_trylock(x)			spin_trylock(x)
#endif /* CONFIG_PREEMPT */

#define adeos_spin_lock_irqsave(x,flags)	do { adeos_hw_local_irq_save(flags); adeos_spin_lock(x); } while (0)
#define adeos_spin_unlock_irqrestore(x,flags)	do { adeos_spin_unlock(x); adeos_hw_local_irq_restore(flags); } while (0)
#define adeos_spin_lock_disable(x)		do { adeos_hw_cli(); adeos_spin_lock(x); } while (0)
#define adeos_spin_unlock_enable(x)		do { adeos_spin_unlock(x); adeos_hw_sti(); } while (0)

/* Private interface -- Internal use only */

struct adattr;

void __adeos_init(void);

void __adeos_init_domain(adomain_t *adp,
			 struct adattr *attr);

void __adeos_cleanup_domain(adomain_t *adp);

#define __adeos_check_machine() do { } while(0)

void __adeos_enable_pipeline(void);

void __adeos_disable_pipeline(void);

void __adeos_init_stage(adomain_t *adp);

void FASTCALL(__adeos_sync_stage(unsigned long syncmask));

void __adeos_tune_timer(unsigned long hz);

asmlinkage int __adeos_handle_irq(int irq,
				   struct pt_regs *regs);

#ifdef CONFIG_ADEOS_THREADS

asmlinkage int __adeos_switch_domain(adomain_t *adp,
				     adomain_t **currentp);

/* Called with hw interrupts off. */
extern inline void __adeos_switch_to(adomain_t *out,
				     adomain_t *in,
				     int cpuid)
{
    extern adomain_t *adp_cpu_current[];

    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled());

    __adeos_switch_domain(in,&adp_cpu_current[cpuid]);

    if (out->dswitch != NULL)
	out->dswitch();
}

#endif /* CONFIG_ADEOS_THREADS */

extern struct pt_regs __adeos_irq_regs;

/* hook for non-Linux syscalls (i.e. RTAI syscall) (see linux/arch/arm/kernel/entry-common.S) */
extern int (*adeos_syscall_entry)(struct pt_regs *regs);

/* hook for interrupt handling (default value: __adeos_handle_irq, see
 * linux/arch/arm/kernel/entry-armv.S) */
extern int (*adeos_irq_entry)(int irq, struct pt_regs *regs);

#endif /* !__ARM_ADEOS_H */
