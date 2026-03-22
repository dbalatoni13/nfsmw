#ifndef GAMEPLAY_GUTILITY_H
#define GAMEPLAY_GUTILITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

static inline void AlignPointer(unsigned char *&ptr, unsigned int bound) {
    ptr = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(ptr) + (bound - 1)) & ~(bound - 1));
}

#endif
