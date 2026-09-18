#include "GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const unsigned int &markerKey) : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    const UMath::Vector3 &pos = Position();
    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 initialVec = {0.0f, 0.0f, 1.0f};

    UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
    MATRIX4_multyrot(&rotMat, -Rotation() / 360.0f, &rotMat);
    UMath::Rotate(initialVec, rotMat, initialVec);
    mPosition = UMath::Vector3Make(-pos.y, pos.z, pos.x);
    mDirection = initialVec;
}

GMarker::~GMarker() {
}
