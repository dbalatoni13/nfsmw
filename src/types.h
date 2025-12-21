#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef EA_PLATFORM_GAMECUBE

#include "dolphin/types.h"

#elif defined(EA_PLATFORM_XENON) || defined(EA_PLATFORM_PLAYSTATION2)

#include <cstddef>

// TODO
typedef int BOOL;
typedef int Bool;

#if !defined(__cplusplus) || __cplusplus < 201103L

#ifndef nullptr
#define nullptr NULL
#endif

#if __cplusplus < 201103L
#ifndef override
#define override
#endif
#endif

#endif

#ifndef ATTRIBUTE_ALIGN
#define ATTRIBUTE_ALIGN(num) __declspec(align(num))
#endif

#ifndef AT_ADDRESS
#define AT_ADDRESS(xyz)
#endif

#endif

#include <cmath>

#ifndef M_TWOPI
#define M_TWOPI (2.0 * M_PI)
#endif

typedef unsigned int uintptr_t;
typedef int intptr_t;

#endif
