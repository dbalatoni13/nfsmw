#ifndef __EXPLOSION_H
#define __EXPLOSION_H

#include "Speed/Indep/Src/Sim/SimTypes.h"

// total size: 0x30
struct ExplosionParams : public Sim::Param {
    // TODO macro
    static UCrc32 TypeName() {
        static UCrc32 value = "ExplosionParams";
        return value;
    }

    ExplosionParams(float expansion_speed, float start_radius, float radius, const UMath::Vector3 &position, HMODEL source, bool effect_source,
                    bool damage, unsigned int targets)
        : fPosition(position) {
        // TODO
    }

    const UMath::Vector3 &fPosition; // offset 0x10, size 0x4
    float fExpansionSpeed;           // offset 0x14, size 0x4
    float fRadius;                   // offset 0x18, size 0x4
    float fStartRadius;              // offset 0x1C, size 0x4
    HMODEL fSource;                  // offset 0x20, size 0x4
    bool fEffectSource;              // offset 0x24, size 0x1
    bool fDamage;                    // offset 0x28, size 0x1
    unsigned int fTargets;           // offset 0x2C, size 0x4
};

#endif
