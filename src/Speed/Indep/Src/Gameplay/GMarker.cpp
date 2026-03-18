#include "Speed/Indep/Src/Gameplay/GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const Attrib::Key &markerKey)
    : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    const UMath::Vector3 *pos = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x9F743A0E, 0));
    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);
    const float *rotation = reinterpret_cast<const float *>(GetAttributePointer(0x5A6A57C6, 0));

    if (!pos) {
        pos = reinterpret_cast<const UMath::Vector3 *>(Attrib::DefaultDataArea(sizeof(UMath::Vector3)));
    }

    if (!rotation) {
        rotation = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    MATRIX4_multyrot(&rotMat, -*rotation * 0.0027777778f, &rotMat);
    VU0_MATRIX3x4_vect3mult(initialVec, rotMat, initialVec);

    mPosition.x = -pos->y;
    mPosition.y = pos->z;
    mPosition.z = pos->x;
    mDirection = initialVec;
}

GMarker::~GMarker() {}
