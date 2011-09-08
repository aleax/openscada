/**********************************************************************
 *
 *  common.h
 *
 *  v 0.0.1 2000.1.11 by Reed Lai
 *
 *  common types and definitions
 *
 *  History:
 *
 *  v 0.0.1 2000.1.11 by Reed Lai
 *      re-typedef, Linux kernel 2.2.13, Slackware 7
 *
 *  v 0.00 1999.11.03 by Reed Lai
 *      create, Linux kernel 2.2.6, Slackware 4
 *
 **********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef _COMMON_H
#define _COMMON_H

#include <linux/version.h>
#include <sys/types.h>

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,2,6)
typedef __u8 u_byte_t;
typedef __u16 u_word_t;
typedef __u32 u_dword_t;
typedef __u64 u_qword_t;
typedef __s8 byte_t;
typedef __s16 word_t;
typedef __s32 dword_t;
typedef __s64 qword_t;
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2,2,13)
typedef u_int8_t u_byte_t;
typedef u_int16_t u_word_t;
typedef u_int32_t u_dword_t;
typedef u_int64_t u_qword_t;
typedef int8_t byte_t;
typedef int16_t word_t;
typedef int32_t dword_t;
typedef int64_t qword_t;
#else
typedef unsigned char u_byte_t;
typedef unsigned short int u_word_t;
typedef unsigned int u_dword_t;
typedef unsigned long long int u_qword_t;
typedef char byte_t;
typedef short int word_t;
typedef int dword_t;
typedef long long int qword_t;
#endif

#ifndef FALSE
#  define FALSE  0
#endif

#ifndef TRUE
#  define TRUE   1
#endif

typedef int __bool_t;			/* Boolean, true or false */

typedef qword_t __tod_t;

#ifndef MIN
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#endif
#ifndef ABS
#define ABS(a)    ( (a) < 0 ? -(a) : (a) )
#endif

#ifndef CTRL
#  define CTRL(x)    ((x) & 0x1f)
#endif

#endif							/* _COMMON_H */

#ifdef __cplusplus
}
#endif
