//
//
#ifndef GINSUHELPER_H
#define GINSUHELPER_H

//
#define GINSU_USE_BMATH

inline int IntRound(float x) {
    if (0.0f < x) {
        return static_cast<int>(x + 0.5f);
    } else {
        return static_cast<int>(x - 0.5f);
    }
}

inline int IntCeil(float x) {
    if (0.0f < x) {
        int i = static_cast<int>(x);
        if (static_cast<float>(i) == x) {
            return i;
        } else {
            return i + 1;
        }
    }

    return static_cast<int>(x);
}

inline int IntFloor(float x) {
    if (0.0f <= x) {
        return static_cast<int>(x);
    } else {
        int i = static_cast<int>(x);
        if (static_cast<float>(i) == x) {
            return i;
        } else {
            return i - 1;
        }
    }
}

#endif
