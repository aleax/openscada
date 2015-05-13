#ifndef _F_TYPES_H
#define _F_TYPES_H

#include <stddef.h>

// COMMON DATA TYPES
#if defined(__GNUC__) || (defined (__MSC_VER) && __MSC_VER >= 1600)
# include <stdint.h>
#else
# include <fbus/stdlib/pstdint.h>
# if !defined(_UINTPTR_T_DEFINED)
   typedef unsigned long uintptr_t;
# endif
#endif

#ifndef __cplusplus
  typedef char bool;
# define false 0
# define true  1
#endif

#ifndef NULL
# ifdef __cplusplus
#  define NULL    0
# else
#  define NULL    ((void *)0)
# endif
#endif 

typedef struct F_BUFFER
{
  void*   pData;
  size_t  len;
} 
F_BUFFER, *PF_BUFFER;

#endif // _F_TYPES_H


