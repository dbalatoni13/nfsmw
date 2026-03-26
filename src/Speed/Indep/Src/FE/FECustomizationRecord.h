#ifndef FE_FECUSTOMIZATIONRECORD_H
#define FE_FECUSTOMIZATIONRECORD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

namespace Attrib {
namespace Gen {
struct pvehicle;
}
} // namespace Attrib
struct PresetCar;

struct FECustomizationRecord {
    short InstalledPartIndices[139];                       // offset 0x000, size 0x116
    Physics::Upgrades::Package InstalledPhysics;           // offset 0x118, size 0x20
    Physics::Tunings Tunings[Physics::NUM_CUSTOM_TUNINGS]; // offset 0x138, size 0x54
    Physics::eCustomTuningType ActiveTuning;               // offset 0x18C, size 0x4
    int Preset;                                            // offset 0x190, size 0x4
    unsigned char Handle;                                  // offset 0x194, size 0x1

    FECustomizationRecord();

    static void operator delete(void *mem, unsigned int size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    void SetTuning(Physics::Tunings::Path id, float value) {
        Tunings[ActiveTuning].Value[id] = value;
    }

    float GetTuning(Physics::Tunings::Path id) const {
        return Tunings[ActiveTuning].Value[id];
    }

    const Physics::Tunings *GetTunings() const {
        return &Tunings[ActiveTuning];
    }

    Physics::Tunings *GetTunings() {
        return &Tunings[ActiveTuning];
    }

    void Default();
    void BecomePreset(PresetCar *preset);
    bool WriteRecordIntoPhysics(Attrib::Gen::pvehicle &vehicle) const;
    bool WritePhysicsIntoRecord(const Attrib::Gen::pvehicle &vehicle);
};

#endif
