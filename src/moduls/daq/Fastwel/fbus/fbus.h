#ifndef __FBUS_H__
#define __FBUS_H__

#include <stdlib/f_types.h>

#if defined(_MSC_VER)
  #define FDLL_EXPORT __declspec(dllimport)
#else
  #define FDLL_EXPORT
#endif

#include "../../Fastwel/fbus/fb_node.h"
#include "../../Fastwel/fbus/fb_api.h"
#include "../../Fastwel/fbus/fiom.h"

#endif  // __FBUS_H__
