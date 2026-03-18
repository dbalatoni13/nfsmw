#include "Speed/Indep/Src/Gameplay/GMarker.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

GMarker::GMarker(const Attrib::Key &markerKey)
    : GRuntimeInstance(markerKey, kGameplayObjType_Marker) {
    UMath::Matrix4 rotMat;
    UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);
    const UMath::Vector3 *posPtr = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x9f743a0e, 0));
    const float *rotation = reinterpret_cast<const float *>(GetAttributePointer(0x5a6a57c6, 0));

    if (!posPtr) {
        posPtr = reinterpret_cast<const UMath::Vector3 *>(Attrib::DefaultDataArea(sizeof(UMath::Vector3)));
    }

    if (!rotation) {
        rotation = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    UMath::MultYRot(UMath::Matrix4::kIdentity, -*rotation * 0.00069444446f, rotMat);
    VU0_MATRIX3x4_vect3mult(initialVec, rotMat, initialVec);

    mPosition.x = -posPtr->y;
    mPosition.y = posPtr->z;
    mPosition.z = posPtr->x;
    mDirection = initialVec;
}

GMarker::~GMarker() {}
