#ifndef WORLD_DAMAGEZONES_H
#define WORLD_DAMAGEZONES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace DamageZone {

enum ID {
    DZ_FRONT = 0,
    DZ_REAR = 1,
    DZ_LEFT = 2,
    DZ_RIGHT = 3,
    DZ_LFRONT = 4,
    DZ_RFRONT = 5,
    DZ_LREAR = 6,
    DZ_RREAR = 7,
    DZ_TOP = 8,
    DZ_BOTTOM = 9,
    DZ_MAX = 10,
};

// total size: 0x4
struct Info {
    void Set(ID id, unsigned int level) {
        Value &= ~(7 << (id * 3));
        Value |= (level & 7) << (id * 3);
    }

    void Clear() {
        Value = 0;
    }

    unsigned int Value; // offset 0x0, size 0x4
};

}; // namespace DamageZone

#endif
