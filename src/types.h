#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef TARGET_GC

#include "dolphin/types.h"

#elif TARGET_X360

#include <cstddef>

typedef bool BOOL;
typedef bool Bool;

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

typedef unsigned int uintptr_t;
typedef int intptr_t;

#endif
