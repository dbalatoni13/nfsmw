#ifndef WORLD_WCOLISSION_TRI_H
#define WORLD_WCOLISSION_TRI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "WSurfaceTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x30
struct WCollisionTri {
    WCollisionTri() {}

    float MinY() const {
        float minY = UMath::Min(this->fPt0.y, this->fPt1.y);
        return UMath::Min(minY, this->fPt2.y);
    }

    void GetNormal(UMath::Vector3 *norm) const {
        UMath::Vector3 vecX;
        UMath::Vector3 vecZ;
        UMath::Vector3 normal;

        vecZ.x = this->fPt1.x - this->fPt0.x;
        vecZ.y = this->fPt1.y - this->fPt0.y;
        vecZ.z = this->fPt1.z - this->fPt0.z;

        vecX.x = this->fPt0.x - this->fPt2.x;
        vecX.y = this->fPt0.y - this->fPt2.y;
        vecX.z = this->fPt0.z - this->fPt2.z;
        UMath::Cross(vecZ, vecX, normal);

        if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
            norm->x = 0.0f;
            norm->y = 1.0f;
            norm->z = 0.0f;
        } else {
            v3unit(&normal, norm);
        }
    }

    UMath::Vector3 fPt0;                  // offset 0x0, size 0xC
    const struct SimSurface *fSurfaceRef; // offset 0xC, size 0x4
    UMath::Vector3 fPt1;                  // offset 0x10, size 0xC
    unsigned int fFlags;                  // offset 0x1C, size 0x4
    UMath::Vector3 fPt2;                  // offset 0x20, size 0xC
    WSurface fSurface;                    // offset 0x2C, size 0x2
    unsigned short PAD;                   // offset 0x2E, size 0x2
};

#endif
