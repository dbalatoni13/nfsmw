#ifndef BTYPES_HPP
#define BTYPES_HPP

#include <types.h>

#ifdef _MSC_VER
#define START_ALIGN(d) __declspec(align(d))
#define END_ALIGN(d)
#else
#define START_ALIGN(d)
#define END_ALIGN(d) __attribute__((aligned(d)))
#endif
#define NUM_ELEMENTS(a) ((int)(sizeof(a) / sizeof(a[0])))
#define OFFSET_OF(type, mem) ((uintptr)(&((type *)1)->mem) - 1)
#define Ptr32Wrapper(ptype) ptype

// TODO add typedefs

#endif
