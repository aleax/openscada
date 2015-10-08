/**********************************************************************
 *
 *  timer.h
 *
 *  Header of timer.c
 *
 *  v 0.0.1 2000.4.20 by Reed Lai
 *
 *  History:
 *
 *  v 0.0.2 2000.9.21 by Reed Lai
 *    compilation error on kernel 2.2.13
 *    change condiction
 *    "LINUX_VERSION_CODE > KERNEL_VERSION(2,2,13)"
 *    to "LINUX_VERSION_CODE > KERNEL_VERSION(2,2,12)"
 *
 *  v 0.0.1 2000.4.20 by Reed Lai
 *    change condiction
 *    "LINUX_VERSION_CODE == KERNEL_VERSION(2,2,13)"
 *    to "LINUX_VERSION_CODE > KERNEL_VERSION(2,2,13)"
 *    to  include <sys/time.h>, and tested OK under kernel 2.2.14.
 *
 *  v 0.00 1998.10.21 by Reed Lai
 *    create
 *
 **********************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include <linux/version.h>

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,2,6)
#include <time.h>
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,2,12)
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "common.h"

extern __tod_t _time_get_tod();
extern void _time_set_deadline(unsigned long usec,
							   struct timeval *deadline);
extern int _time_out(struct timeval *deadline);
// extern int _set_timeout(long usec, __sighandler_t h_sigalrm);

#endif							/* _TIMER_H */
