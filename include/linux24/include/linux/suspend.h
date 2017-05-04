/* $Id: suspend.h,v 1.3 2005/03/03 18:14:33 joff Exp $ */

#ifndef __MTD_COMPAT_VERSION_H__
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include_next <linux/suspend.h>
#endif

#endif /* __MTD_COMPAT_VERSION_H__ */
