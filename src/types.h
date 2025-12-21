#ifndef _TYPES_H_
#define _TYPES_H_

#if defined(EA_PLATFORM_GAMECUBE)

#include "dolphin/types.h"

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int type_operator_new;

#elif defined(EA_PLATFORM_XENON)

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

// TODO
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int type_operator_new;

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef short unsigned int uint16_t;
typedef unsigned int uint32_t;
typedef float float_t;
typedef double double_t;
typedef long int int64_t;
typedef long unsigned int uint64_t;
typedef int8_t bool8_t;

#elif defined(EA_PLATFORM_PLAYSTATION2)
#include <cstddef>

// TODO or is the first one unsigned char?
typedef int BOOL;
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
#define ATTRIBUTE_ALIGN(num) __attribute__((aligned(num)))
#endif

#ifndef AT_ADDRESS
#define AT_ADDRESS(xyz)
#endif

typedef signed char int8;
typedef unsigned char uint8;
typedef short int int16;
typedef short unsigned int uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int int64;
typedef long unsigned int uint64;
typedef unsigned int type_operator_new;

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef short unsigned int uint16_t;
typedef unsigned int uint32_t;
typedef float float_t;
typedef double double_t;
typedef long int int64_t;
typedef long unsigned int uint64_t;
typedef int8_t bool8_t;
// typedef int32_t intptr_t;
// typedef uint32_t uintptr_t;
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
typedef char char8_t;
// typedef __wchar_t char16_t; // TODO
typedef uint32_t char32_t;

#endif

#include <cmath>

#ifndef M_TWOPI
#define M_TWOPI (2.0 * M_PI)
#endif

typedef unsigned int uintptr_t;
typedef int intptr_t;

#define REFRESH_RATE (60.0f)

#endif // _TYPES_H_
