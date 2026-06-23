#ifndef PHYSICS_SMACKABLE_H
#define PHYSICS_SMACKABLE_H

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"

// total size: 0x5C
struct SmackableParams : public Sim::Param {
    // TODO macro
    static UCrc32 TypeName() {
        static UCrc32 value = "SuspensionParams";
        return value;
    }

    SmackableParams(const UMath::Matrix4 &mat, bool virginspawn, IModel *scenery, bool simple_physics) {
        // TODO
    }

    struct UMath::Matrix4 fMatrix; // offset 0x10, size 0x40
    bool fVirginSpawn;             // offset 0x50, size 0x1
    IModel *fScenery;              // offset 0x54, size 0x4
    bool fSimplePhysics;           // offset 0x58, size 0x1
};

#endif
