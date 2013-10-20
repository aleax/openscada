/*
 * libiec61850_platform_includes.h
 */

#ifndef LIBIEC61850_PLATFORM_INCLUDES_H_
#define LIBIEC61850_PLATFORM_INCLUDES_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "hal.h"
#include "string_utilities.h"

#include "platform_endian.h"

#ifdef _WIN32
#ifdef BUILD_DLL
  #define API __declspec(dllexport)
#else
  #define API __declspec(dllimport)
#endif

#define STDCALL __cdecl
#else
#define API
#define STDCALL
#endif

#endif /* LIBIEC61850_PLATFORM_INCLUDES_H_ */
