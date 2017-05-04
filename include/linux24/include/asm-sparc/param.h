/* $Id: param.h,v 1.2 2004/08/04 01:17:23 joff Exp $ */
#ifndef _ASMSPARC_PARAM_H
#define _ASMSPARC_PARAM_H

#ifndef HZ
#define HZ 100
#ifdef __KERNEL__
#define hz_to_std(a) (a)
#endif
#endif

#define EXEC_PAGESIZE	8192    /* Thanks for sun4's we carry baggage... */

#ifndef NGROUPS
#define NGROUPS		32
#endif

#ifndef NOGROUP
#define NOGROUP		(-1)
#endif

#define MAXHOSTNAMELEN	64	/* max length of hostname */

#ifdef __KERNEL__
# define CLOCKS_PER_SEC	HZ	/* frequency at which times() counts */
#endif

#endif
