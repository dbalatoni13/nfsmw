#ifndef BTYPES_HPP
#define BTYPES_HPP

#define START_ALIGN(d)
#define END_ALIGN(d) __attribute__((aligned(d)))
#define NUM_ELEMENTS(a) ((int)(sizeof(a) / sizeof(a[0])))
#define OFFSET_OF(type, mem) ((uintptr)(&((type *)1)->mem) - 1)
#define Ptr32Wrapper(ptype) ptype

#endif
