#ifndef DAMAGE_ZONES_H
#define DAMAGE_ZONES_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

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

Attrib::StringKey GetSystemName(ID id);
UCrc32 GetDamageStimulus(unsigned int level);
UCrc32 GetImpactStimulus(unsigned int level);

// total size: 0x4
struct Info {
    Info() {
        this->Value = 0;
    }

    Info(const Info &from) {
        this->Value = from.Value;
    }

    void Set(ID id, unsigned int level) {
        this->Value &= ~(7 << (id * 3));
        this->Value |= (level & 7) << (id * 3);
    }

    unsigned int Get(ID id) const {
        return (this->Value >> (id * 3)) & 7;
    }

    void Clear() {
        this->Value = 0;
    }

    uint32 Value; // offset 0x0, size 0x4
};

}; // namespace DamageZone

#endif
