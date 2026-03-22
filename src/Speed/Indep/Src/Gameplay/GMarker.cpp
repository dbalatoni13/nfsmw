#include "Speed/Indep/Src/Gameplay/GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const Attrib::Key &markerKey)
    : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    const UMath::Vector3 &pos = Position(0);
    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);

    MATRIX4_multyrot(&rotMat, -Rotation(0) * 0.0027777778f, &rotMat);
    VU0_MATRIX3x4_vect3mult(initialVec, rotMat, initialVec);

    mPosition = UMath::Vector3Make(-pos.y, pos.z, pos.x);
    mDirection = initialVec;
}

GMarker::~GMarker() {}
