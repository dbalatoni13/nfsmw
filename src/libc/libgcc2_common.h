/* Tipos de gcc-2.95/gcc/libgcc2.c para PowerPC big-endian. */
#ifndef _LIBGCC2_COMMON_H_
#define _LIBGCC2_COMMON_H_

#define BITS_PER_UNIT 8

typedef int SItype;
typedef unsigned int USItype;
typedef long long DItype;
typedef unsigned long long UDItype;
typedef int word_type;

struct DIstruct {
    SItype high, low;
};

typedef union {
    struct DIstruct s;
    DItype ll;
} DIunion;

#endif
