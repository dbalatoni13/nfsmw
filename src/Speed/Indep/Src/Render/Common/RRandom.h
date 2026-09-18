#ifndef RENDER_COMMON_RRANDOM_H
#define RENDER_COMMON_RRANDOM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct RRandom {
    static int Fixed() {
        int randtemp = fastRandom * randSeed;

        fastRandom = randtemp & 0xFFFF;

        return (randtemp >> 8) & 0xFFFF;
    }

    static int IntRange(int range) {
        return (range * Fixed()) >> 16;
    }

    static float FloatRange(float range) {
        int r = Fixed();

        float rf = r * range;

        return rf / 65536.0f;
    }

    static unsigned int fastRandom; // size: 0x4, address: 0x80435768
    static unsigned int randSeed;   // size: 0x4, address: 0x8043576C
};



#endif
