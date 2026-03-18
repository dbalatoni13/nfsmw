#include "Speed/Indep/Src/Gameplay/GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const Attrib::Key &markerKey)
    : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    const UMath::Vector3 &pos = Position(0);
    UMath::Matrix4 rotMat;
    UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);
    UMath::MultYRot(UMath::Matrix4::kIdentity, -Rotation(0) * 0.00069444446f, rotMat);
    UMath::Rotate(initialVec, rotMat, initialVec);

    mPosition.x = -pos.y;
    mPosition.y = pos.z;
    mPosition.z = pos.x;
    mDirection = initialVec;
}

GMarker::~GMarker() {}
