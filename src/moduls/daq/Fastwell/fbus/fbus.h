#ifndef __FBUS_H__
#define __FBUS_H__

#include <stdlib/f_types.h>

#if defined(_MSC_VER)
  #define FDLL_EXPORT __declspec(dllimport)
#else
  #define FDLL_EXPORT
#endif

#include <fb_node.h>
#include <fb_api.h>
#include <fiom.h>

#endif  // __FBUS_H__
