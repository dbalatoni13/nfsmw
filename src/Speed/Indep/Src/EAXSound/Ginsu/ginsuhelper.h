//
//
#ifndef GINSUHELPER_H
#define GINSUHELPER_H

//
#define GINSU_USE_BMATH

inline int IntRound(float x) {
    if (0.0f < x) {
        x += 0.5f;
    } else {
        x -= 0.5f;
    }
    return static_cast<int>(x);
}

inline int IntCeil(float x) {
    if (0.0f < x) {
        int i = static_cast<int>(x) + 1;
        if (static_cast<float>(i - 1) == x) {
            i--;
        }

        return i;
    }

    return static_cast<int>(x);
}

inline int IntFloor(float x) {
    if (0.0f <= x) {
        return static_cast<int>(x);
    } else {
        int i = static_cast<int>(x) - 1;
        if (static_cast<float>(i + 1) == x) {
            return i + 1;
        } else {
            return i;
        }
    }
}

#endif
