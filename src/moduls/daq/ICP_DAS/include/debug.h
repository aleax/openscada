/*
 *      debug.h
 *
 *      v 0.00 1999.9.15 by Reed Lai
 *
 *      for debug only
 *
 * History:
 *      0.00    Reed Lai
 */
#undef PDEBUG
#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef I7K_DEBUG
# ifdef __KERNEL__

	/* debug on for kernel mode */
#	include <linux/kernel.h>
#	define PDEBUG(fmt, args...) printk(KERN_DEBUG fmt, ## args)
# else
	/* debug on for user space */
#	define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
# endif							/* __KERNEL__ */
#else
# define PDEBUG(fmt, args...)	/* no debug message */
#endif							/* I7K_DEBUG */

#endif							/* _DEBUG_H */
