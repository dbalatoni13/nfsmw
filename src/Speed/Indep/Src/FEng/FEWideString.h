#ifndef FENG_FEWIDESTRING_H
#define FENG_FEWIDESTRING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct FEWideString {
    int16 *mpsString;
    uint32 mulBufferLength;

    FEWideString &operator=(char *string);
};

#endif
