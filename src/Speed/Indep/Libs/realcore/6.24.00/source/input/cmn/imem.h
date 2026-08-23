#ifndef REALCORE_SOURCE_INPUT_CMN_IMEM_H
#define REALCORE_SOURCE_INPUT_CMN_IMEM_H

#include "../../../include/common/realcore/std.h"

struct IMem {
    static void Copy(void *dest, const void *source, int count) {
        MEM_copy(dest, source, count);
    }

    static void Fill(void *dest, unsigned int val, int count) {
        MEM_fill(dest, val, count);
    }

    static void Clear(void *dest, int count) {
        Fill(dest, 0, count);
    }
};

#endif
