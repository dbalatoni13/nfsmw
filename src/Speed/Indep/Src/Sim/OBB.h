#ifndef SIM_OBB_H
#define SIM_OBB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"


#include <cstddef>

// total size: 0xB0
class OBB {
  public:
    enum eShape {
        BOX = 1,
        SPHERE = 2,
    };

    OBB();
    ~OBB();
    void Reset(const UMath::Matrix4 &orient, const UMath::Vector3 &position, const UMath::Vector3 &dimension);
    bool CheckOBBOverlap(OBB *other);
    bool CheckOBBOverlapAndFindIntersection(OBB *other);

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

  private:
    static bool BoxVsBox(OBB *obbA, OBB *obbB, OBB *result);
    static bool SphereVsBox(OBB *sphereA, OBB *boxB, OBB *result);
    static bool SphereVsSphere(OBB *a, OBB *b, OBB *result);

    UMath::Vector4 position;         // offset 0x0, size 0x10
    UMath::Vector4 normal[3];        // offset 0x10, size 0x30
    UMath::Vector4 extent[3];        // offset 0x40, size 0x30
    UMath::Vector4 collision_point;  // offset 0x70, size 0x10
    UMath::Vector4 collision_normal; // offset 0x80, size 0x10
    union {
        float best_projected_interval; // offset 0x0, size 0x4
        float penetration_depth;       // offset 0x0, size 0x4
    }; // offset 0x90, size 0x4
    unsigned char shape; // offset 0x94, size 0x1
    char pad[11];        // offset 0x95, size 0xB
    float dimension[4];  // offset 0xA0, size 0x10
};

#endif
