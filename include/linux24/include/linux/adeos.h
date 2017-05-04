/*
 *   include/linux/adeos.h
 *
 *   Copyright (C) 2002,2003,2004 Philippe Gerum.
 *
 *   Copyright (C) 2004-2005 Michael Neuhauser, Firmix Software GmbH (mike@firmix.at)
 *   	various tweaks, fixes and optimizations, backport of unthreaded
 *   	support from Adeos/x86 for 2.6
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

#ifndef __LINUX_ADEOS_H
#define __LINUX_ADEOS_H

#include <linux/kernel.h>

/* be paranoid and check assumptions (mostly if hw-irqs are off)? */
#undef ADEOS_PARANOIA
//#define ADEOS_PARANOIA	1
#ifdef ADEOS_PARANOIA
#define ADEOS_PARANOIA_ASSERT(cond)	BUG_ON(!(cond))
#else
#define ADEOS_PARANOIA_ASSERT(cond)	do { /* nop */ } while (0)
#endif

#include <asm/adeos.h>

#define ADEOS_VERSION_PREFIX  "2.4"
#define ADEOS_VERSION_STRING  (ADEOS_VERSION_PREFIX ADEOS_ARCH_STRING)
#define ADEOS_RELEASE_NUMBER  (0x02040000|((ADEOS_MAJOR_NUMBER&0xff)<<8)|(ADEOS_MINOR_NUMBER&0xff))

#define ADEOS_ROOT_PRI       100
#define ADEOS_ROOT_ID        0
#define ADEOS_ROOT_NPTDKEYS  4	/* Must be <= 32 */

#define ADEOS_RESET_TIMER  0x1
#define ADEOS_SAME_HANDLER ((void (*)(unsigned))(-1))

/* Global domain flags */
#define ADEOS_SPRINTK_FLAG 0	/* Synchronous printk() allowed */
#define ADEOS_PPRINTK_FLAG 1	/* Asynchronous printk() request pending */

/* Per-cpu pipeline flags.
   WARNING: some implementation might refer to those flags
   non-symbolically in assembly portions (e.g. x86). */
#define IPIPE_STALL_FLAG   0	/* Stalls a pipeline stage */
#define IPIPE_XPEND_FLAG   1	/* Exception notification is pending */
#define IPIPE_SLEEP_FLAG   2	/* Domain has self-suspended */

#define IPIPE_HANDLE_FLAG    0
#define IPIPE_PASS_FLAG      1
#define IPIPE_ENABLE_FLAG    2
#define IPIPE_DYNAMIC_FLAG   IPIPE_HANDLE_FLAG
#define IPIPE_EXCLUSIVE_FLAG 3
#define IPIPE_STICKY_FLAG    4
#define IPIPE_SYSTEM_FLAG    5
#define IPIPE_LOCK_FLAG      6
#define IPIPE_SHARED_FLAG    7
#define IPIPE_CALLASM_FLAG   8	/* Arch-dependent -- might be unused. */

#define IPIPE_HANDLE_MASK    (1 << IPIPE_HANDLE_FLAG)
#define IPIPE_PASS_MASK      (1 << IPIPE_PASS_FLAG)
#define IPIPE_ENABLE_MASK    (1 << IPIPE_ENABLE_FLAG)
#define IPIPE_DYNAMIC_MASK   IPIPE_HANDLE_MASK
#define IPIPE_EXCLUSIVE_MASK (1 << IPIPE_EXCLUSIVE_FLAG)
#define IPIPE_STICKY_MASK    (1 << IPIPE_STICKY_FLAG)
#define IPIPE_SYSTEM_MASK    (1 << IPIPE_SYSTEM_FLAG)
#define IPIPE_LOCK_MASK      (1 << IPIPE_LOCK_FLAG)
#define IPIPE_SHARED_MASK    (1 << IPIPE_SHARED_FLAG)
#define IPIPE_CALLASM_MASK   (1 << IPIPE_CALLASM_FLAG)

#define IPIPE_DEFAULT_MASK  (IPIPE_HANDLE_MASK|IPIPE_PASS_MASK)

typedef struct adattr {

    unsigned domid;		/* Domain identifier -- Magic value set by caller */
    const char *name;		/* Domain name -- Warning: won't be dup'ed! */
    int priority;		/* Priority in interrupt pipeline */
    void (*entry)(int);		/* Domain entry point */
    int estacksz;		/* Stack size for entry context -- 0 means unspec */
    void (*dswitch)(void);	/* Handler called each time the domain is switched in */
    int nptdkeys;		/* Max. number of per-thread data keys */
    void (*ptdset)(int,void *);	/* Routine to set pt values */
    void *(*ptdget)(int);	/* Routine to get pt values */

} adattr_t;

typedef struct admutex {

    spinlock_t lock;

#ifdef CONFIG_ADEOS_THREADS
    adomain_t *sleepq, /* Pending domain queue */
	      *owner;	/* Domain owning the mutex */
#ifdef CONFIG_SMP
    volatile int owncpu;
#define ADEOS_MUTEX_UNLOCKED { SPIN_LOCK_UNLOCKED, NULL, NULL, -1 }
#else  /* !CONFIG_SMP */
#define ADEOS_MUTEX_UNLOCKED { SPIN_LOCK_UNLOCKED, NULL, NULL }
#endif /* CONFIG_SMP */
#else /* !CONFIG_ADEOS_THREADS */
#define ADEOS_MUTEX_UNLOCKED { SPIN_LOCK_UNLOCKED }
#endif /* CONFIG_ADEOS_THREADS */

} admutex_t;

#ifdef CONFIG_ADEOS_MODULE
extern int adp_pipelined;
#else  /* !CONFIG_ADEOS_MODULE */
#define adp_pipelined		(1)	/* optimize away flag tests if not compiled as module */
#endif /* CONFIG_ADEOS_MODULE */

#ifdef SMP

extern adomain_t *adp_cpu_current[],
                 *adp_root;

#else

extern adomain_t adeos_root_domain;
extern adomain_t *adp_cpu_current[];
#define adp_root (&adeos_root_domain)

#endif

extern int __adeos_event_monitors[];

extern unsigned __adeos_printk_virq;

extern unsigned long __adeos_virtual_irq_map;

extern struct list_head __adeos_pipeline;

extern spinlock_t __adeos_pipelock;

#ifdef CONFIG_ADEOS_PROFILING

typedef struct adprofdata {

    struct {
	unsigned long long t_handled;
	unsigned long long t_synced;
	unsigned long n_handled;
	unsigned long n_synced;
    } irqs[IPIPE_NR_IRQS];

} adprofdata_t;

extern adprofdata_t __adeos_profile_data[ADEOS_NR_CPUS];

#endif /* CONFIG_ADEOS_PROFILING */

/* Private interface */

#ifdef CONFIG_PROC_FS
void __adeos_init_proc(void);
#endif /* CONFIG_PROC_FS */

void __adeos_takeover(void);

asmlinkage int __adeos_handle_event(unsigned event,
				    void *evdata);

void __adeos_sync_console(unsigned irq);

void __adeos_dump_state(void);

static inline void __adeos_schedule_head(void *evdata) {

    if (__adeos_event_monitors[ADEOS_SCHEDULE_HEAD] > 0)
	__adeos_handle_event(ADEOS_SCHEDULE_HEAD,evdata);
}

static inline int __adeos_schedule_tail(void *evdata) {

    if (__adeos_event_monitors[ADEOS_SCHEDULE_TAIL] > 0)
	return __adeos_handle_event(ADEOS_SCHEDULE_TAIL,evdata);

    return 0;
}

#ifdef CONFIG_ADEOS_EVENT_ENTER_PROCESS
static inline void __adeos_enter_process(void) {

    if (__adeos_event_monitors[ADEOS_ENTER_PROCESS] > 0)
	__adeos_handle_event(ADEOS_ENTER_PROCESS,NULL);
}
#endif /* CONFIG_ADEOS_EVENT_ENTER_PROCESS */

static inline void __adeos_exit_process(void *evdata) {

    if (__adeos_event_monitors[ADEOS_EXIT_PROCESS] > 0)
	__adeos_handle_event(ADEOS_EXIT_PROCESS,evdata);
}

static inline int __adeos_signal_process(void *evdata) {

    if (__adeos_event_monitors[ADEOS_SIGNAL_PROCESS] > 0)
	return __adeos_handle_event(ADEOS_SIGNAL_PROCESS,evdata);

    return 0;
}

static inline void __adeos_kick_process(void *evdata) {

    if (__adeos_event_monitors[ADEOS_KICK_PROCESS] > 0)
	__adeos_handle_event(ADEOS_KICK_PROCESS,evdata);
}

#ifdef CONFIG_ADEOS_EVENT_RENICE_PROCESS
static inline int __adeos_renice_process(void *evdata) {

    if (__adeos_event_monitors[ADEOS_RENICE_PROCESS] > 0)
	return __adeos_handle_event(ADEOS_RENICE_PROCESS,evdata);

    return 0;
}
#endif /* CONFIG_ADEOS_EVENT_RENICE_PROCESS */

void __adeos_stall_root(void);

void __adeos_unstall_root(void);

unsigned long __adeos_test_root(void);

unsigned long __adeos_test_and_stall_root(void);

void FASTCALL(__adeos_restore_root(unsigned long flags));

void __adeos_schedule_back_root(struct task_struct *prev);

int FASTCALL(__adeos_schedule_irq(unsigned irq,
				  struct list_head *head));

#define __adeos_pipeline_head_p(adp) (&(adp)->p_link == __adeos_pipeline.next)

#ifdef CONFIG_ADEOS_THREADS

static inline int __adeos_domain_work_p (adomain_t *adp, int cpuid)

{
    return (!test_bit(IPIPE_SLEEP_FLAG,&adp->cpudata[cpuid].status) ||
	    (!test_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status) &&
	     adp->cpudata[cpuid].irq_pending_hi != 0) ||
	    test_bit(IPIPE_XPEND_FLAG,&adp->cpudata[cpuid].status));
}

#else /* !CONFIG_ADEOS_THREADS */

/* Called with hw interrupts off. */
static inline void __adeos_switch_to (adomain_t *out, adomain_t *in, int cpuid)

{
    void adeos_suspend_domain(void);
    int s;

    ADEOS_PARANOIA_ASSERT(adeos_hw_irqs_disabled());

    adp_cpu_current[cpuid] = in;

    if (in->dswitch)
	in->dswitch();

    /* Make sure adeos_suspend_domain() will not sync beyond the
       current domain. "in" is guaranteed to be closer than "out" from
       the head of the pipeline (and obviously different). */
    s = __test_and_clear_bit(IPIPE_SLEEP_FLAG,&out->cpudata[cpuid].status);

    adeos_suspend_domain(); /* Sync stage and propagate interrupts. */

    if (s)
	__set_bit(IPIPE_SLEEP_FLAG,&out->cpudata[cpuid].status);

    adeos_load_cpuid(); /* Processor might have changed. */

    if (adp_cpu_current[cpuid] == in)
	/* Otherwise, something has changed the current domain under
	   our feet recycling the register set; do not override. */
	adp_cpu_current[cpuid] = out;
}

#endif /* CONFIG_ADEOS_THREADS */

/* Public interface */

int adeos_register_domain(adomain_t *adp,
			  adattr_t *attr);

int adeos_unregister_domain(adomain_t *adp);

void adeos_suspend_domain(void);

int adeos_virtualize_irq_from(adomain_t *adp,
			      unsigned irq,
			      void (*handler)(unsigned irq),
			      int (*acknowledge)(unsigned irq),
			      unsigned modemask);

static inline int adeos_virtualize_irq(unsigned irq,
				       void (*handler)(unsigned irq),
				       int (*acknowledge)(unsigned irq),
				       unsigned modemask) {

    return adeos_virtualize_irq_from(adp_current,
				     irq,
				     handler,
				     acknowledge,
				     modemask);
}

int adeos_control_irq(unsigned irq,
		      unsigned clrmask,
		      unsigned setmask);

cpumask_t adeos_set_irq_affinity(unsigned irq,
				 cpumask_t cpumask);

static inline int adeos_share_irq (unsigned irq, int (*acknowledge)(unsigned irq)) {

    return adeos_virtualize_irq(irq,
				ADEOS_SAME_HANDLER,
				acknowledge,
				IPIPE_SHARED_MASK|IPIPE_HANDLE_MASK|IPIPE_PASS_MASK);
}

unsigned adeos_alloc_irq(void);

int adeos_free_irq(unsigned irq);

int FASTCALL(adeos_trigger_irq(unsigned irq));

static inline int adeos_propagate_irq(unsigned irq) {

    return __adeos_schedule_irq(irq,adp_current->p_link.next);
}

static inline int adeos_schedule_irq(unsigned irq) {

    return __adeos_schedule_irq(irq,&adp_current->p_link);
}

int FASTCALL(adeos_send_ipi(unsigned ipi,
			    cpumask_t cpumask));

static inline void adeos_stall_pipeline_from (adomain_t *adp)

{
    adeos_declare_cpuid;
#ifdef CONFIG_SMP
    unsigned long flags;

    adeos_lock_cpu(flags);

    __set_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);

    if (!__adeos_pipeline_head_p(adp))
	adeos_unlock_cpu(flags);
#else /* CONFIG_SMP */
    set_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);

    if (__adeos_pipeline_head_p(adp))
	adeos_hw_cli();
#endif /* CONFIG_SMP */
}

static inline unsigned long adeos_test_pipeline_from (adomain_t *adp)

{
    unsigned long flags, s;
    adeos_declare_cpuid;
    
    adeos_get_cpu(flags);
    s = test_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);
    adeos_put_cpu(flags);

    return s;
}

static inline unsigned long adeos_test_and_stall_pipeline_from (adomain_t *adp)

{
    adeos_declare_cpuid;
    unsigned long s;
#ifdef CONFIG_SMP
    unsigned long flags;

    adeos_lock_cpu(flags);

    s = __test_and_set_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);

    if (!__adeos_pipeline_head_p(adp))
	adeos_unlock_cpu(flags);
#else /* CONFIG_SMP */
    s = test_and_set_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);

    if (__adeos_pipeline_head_p(adp))
	adeos_hw_cli();
#endif /* CONFIG_SMP */
    
    return s;
}

void FASTCALL(adeos_unstall_pipeline_from(adomain_t *adp));

static inline unsigned long adeos_test_and_unstall_pipeline_from(adomain_t *adp)

{
    unsigned long flags, s;
    adeos_declare_cpuid;
    
    adeos_get_cpu(flags);
    s = test_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);
    adeos_unstall_pipeline_from(adp);
    adeos_put_cpu(flags);

    return s;
}

static inline void adeos_unstall_pipeline(void)

{
    adeos_unstall_pipeline_from(adp_current);
}

static inline unsigned long adeos_test_and_unstall_pipeline(void)

{
    return adeos_test_and_unstall_pipeline_from(adp_current);
}

static inline unsigned long adeos_test_pipeline (void)

{
    return adeos_test_pipeline_from(adp_current);
}

static inline unsigned long adeos_test_and_stall_pipeline (void)

{
    return adeos_test_and_stall_pipeline_from(adp_current);
}

static inline void adeos_restore_pipeline_from (adomain_t *adp, unsigned long flags)

{
    if (flags)
	adeos_stall_pipeline_from(adp);
    else
	adeos_unstall_pipeline_from(adp);
}

static inline void adeos_stall_pipeline (void)

{
    adeos_stall_pipeline_from(adp_current);
}

static inline void adeos_restore_pipeline (unsigned long flags)

{
    adeos_restore_pipeline_from(adp_current,flags);
}

static inline void adeos_restore_pipeline_nosync (adomain_t *adp, unsigned long flags, int cpuid)

{
    /* If cpuid is current, then it must be held on entry
       (adeos_get_cpu/adeos_hw_local_irq_save/adeos_hw_cli). */

    if (flags)
	set_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);
    else
	clear_bit(IPIPE_STALL_FLAG,&adp->cpudata[cpuid].status);
}

int adeos_catch_event_from(adomain_t *adp,
			   unsigned event,
			   void (*handler)(adevinfo_t *));

static inline int adeos_catch_event (unsigned event, void (*handler)(adevinfo_t *))

{
    return adeos_catch_event_from(adp_current,event,handler);
}

static inline void adeos_propagate_event(adevinfo_t *evinfo)

{
    evinfo->propagate = 1;
}

void adeos_init_attr(adattr_t *attr);

int adeos_get_sysinfo(adsysinfo_t *sysinfo);

int adeos_tune_timer(unsigned long ns,
		     int flags);

int adeos_alloc_ptdkey(void);

int adeos_free_ptdkey(int key);

int adeos_set_ptd(int key,
		  void *value);

void *adeos_get_ptd(int key);

unsigned long adeos_critical_enter(void (*syncfn)(void));

void adeos_critical_exit(unsigned long flags);

int adeos_init_mutex(admutex_t *mutex);

int adeos_destroy_mutex(admutex_t *mutex);

unsigned long FASTCALL(adeos_lock_mutex(admutex_t *mutex));

void FASTCALL(adeos_unlock_mutex(admutex_t *mutex,
				 unsigned long flags));

static inline void adeos_set_printk_sync (adomain_t *adp) {
    set_bit(ADEOS_SPRINTK_FLAG,&adp->flags);
}

static inline void adeos_set_printk_async (adomain_t *adp) {
    clear_bit(ADEOS_SPRINTK_FLAG,&adp->flags);
}

#endif /* !__LINUX_ADEOS_H */
