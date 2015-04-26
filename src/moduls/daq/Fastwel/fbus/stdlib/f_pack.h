#ifndef _F_PACK_H
#define _F_PACK_H

// Packing specifier for structs
#ifdef _MSC_VER
#  define PACKED_STRUCT(declaration) \
  __pragma(pack(push, 1)) declaration __pragma(pack(pop))
#elif defined(__GNUC__)
#  define PACKED_STRUCT(declaration) declaration __attribute__((packed))
#else
# error Specify PACKED_STRUCT for used compiler
#endif 

#endif


