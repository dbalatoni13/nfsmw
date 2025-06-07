#ifndef _TYPES_H_
#define _TYPES_H_

#include "dolphin/types.h"

#ifdef __GNUC__
#define LT(a, b) (!((a) >= (b)))
#define GT(a, b) (!((a) <= (b)))
#else
#define LT(a, b) ((a) < (b))
#define GT(a, b) ((a) > (b))
#endif

#endif
