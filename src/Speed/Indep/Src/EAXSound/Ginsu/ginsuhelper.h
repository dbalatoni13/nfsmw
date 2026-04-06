#ifndef EAXSOUND_GINSU_GINSUHELPER_H
#define EAXSOUND_GINSU_GINSUHELPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

static inline int IntFloor(float x) {
    int i = static_cast<int>(x);
    if (x < 0.0f && static_cast<float>(i) != x) {
        i--;
    }
    return i;
}

static inline int IntCeil(float x) {
    int i = static_cast<int>(x);
    if (0.0f < x && static_cast<float>(i) != x) {
        i++;
    }
    return i;
}

static inline int IntRound(float x) {
    if (0.0f < x) {
        x += 0.5f;
    } else {
        x -= 0.5f;
    }
    return static_cast<int>(x);
}

#endif
