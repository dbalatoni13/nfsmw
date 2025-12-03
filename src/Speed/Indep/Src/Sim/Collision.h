#ifndef SIM_COLLISION_H
#define SIM_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Sim {

namespace Collision {

// total size: 0x80
struct Info {
    enum CollisionType {
        NONE = 0,
        OBJECT = 1,
        WORLD = 2,
        GROUND = 3,
    };

    Info() {
        position = UMath::Vector3::kZero;
        objAsurface = nullptr;
        normal = UMath::Vector3::kZero;
        type = 0;
        objAImmobile = 0;
        objADetached = 0;
        objBImmobile = 0;
        objBDetached = 0;
        sliding = 0;
        closingVel = UMath::Vector3::kZero;
        force = 0.0f;
        armA = UMath::Vector3::kZero;
        objA = nullptr;
        armB = UMath::Vector3::kZero;
        objB = nullptr;
        objAVel = UMath::Vector3::kZero;
        impulseA = 0.0f;
        objBVel = UMath::Vector3::kZero;
        impulseB = 0.0f;
        slidingVel = UMath::Vector3::kZero;
        objBsurface = nullptr;
    }

    UMath::Vector3 position;               // offset 0x0, size 0xC
    const Attrib::Collection *objAsurface; // offset 0xC, size 0x4
    UMath::Vector3 normal;                 // offset 0x10, size 0xC
    int type : 3;                          // offset 0x1C, size 0x4
    int objAImmobile : 1;                  // offset 0x1C, size 0x4
    int objADetached : 1;                  // offset 0x1C, size 0x4
    int objBImmobile : 1;                  // offset 0x1C, size 0x4
    int objBDetached : 1;                  // offset 0x1C, size 0x4
    int sliding : 1;                       // offset 0x1C, size 0x4
    int unused : 24;                       // offset 0x1C, size 0x4
    UMath::Vector3 closingVel;             // offset 0x20, size 0xC
    float force;                           // offset 0x2C, size 0x4
    UMath::Vector3 armA;                   // offset 0x30, size 0xC
    HSIMABLE objA;                         // offset 0x3C, size 0x4
    UMath::Vector3 armB;                   // offset 0x40, size 0xC
    HSIMABLE objB;                         // offset 0x4C, size 0x4
    UMath::Vector3 objAVel;                // offset 0x50, size 0xC
    float impulseA;                        // offset 0x5C, size 0x4
    UMath::Vector3 objBVel;                // offset 0x60, size 0xC
    float impulseB;                        // offset 0x6C, size 0x4
    UMath::Vector3 slidingVel;             // offset 0x70, size 0xC
    const Attrib::Collection *objBsurface; // offset 0x7C, size 0x4
};

void AddListener(IListener *listener, HSIMABLE participant, const char *who);
void AddListener(IListener *listener, class UTL::COM::IUnknown *participant, const char *who);
void RemoveListener(IListener *listener);

}; // namespace Collision

}; // namespace Sim

typedef Sim::Collision::Info COLLISION_INFO;

#endif
