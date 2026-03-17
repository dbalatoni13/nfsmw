#include "Speed/Indep/Src/Gameplay/GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const Attrib::Key &markerKey)
    : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    Attrib::Gen::gameplay gameplayObj(markerKey, 0, nullptr);
    UMath::Matrix4 rotMat;
    UMath::Vector3 position;
    UMath::Vector3 forward = {0.0f, 0.0f, 1.0f};

    position = gameplayObj.Position(0);
    UMath::MultYRot(UMath::Matrix4::kIdentity, -gameplayObj.Rotation(0) * 0.00069444446f, rotMat);
    VU0_MATRIX3x4_vect3mult(forward, rotMat, mDirection);

    mPosition.x = -position.y;
    mPosition.y = position.z;
    mPosition.z = position.x;
}

GMarker::~GMarker() {}
