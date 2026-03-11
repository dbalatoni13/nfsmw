#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &r);

void WCollisionObject::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    const unsigned int *src = reinterpret_cast<const unsigned int *>(&fMat);
    unsigned int *dst = reinterpret_cast<unsigned int *>(&m);
    for (unsigned int i = 0; i < 0x10; ++i) {
        dst[i] = src[i];
    }

    if (addXLate) {
        m.v3.x = fPosRadius.x;
        m.v3.y = fPosRadius.y;
        m.v3.z = fPosRadius.z;
        m.v3.w = 1.0f;
        return;
    }

    m.v3.x = 0.0f;
    m.v3.y = 0.0f;
    m.v3.z = 0.0f;
    m.v3.w = 1.0f;
}

float WCollisionInstance::CalcSphericalRadius() const {
    float maxExtent = WWorldMath::wmax(fInvMatRow2Length.w, fInvPosRadius.w);
    maxExtent = WWorldMath::wmax(fHeight, maxExtent);
    return WWorldMath::wmax(fInvMatRow0Width.w, maxExtent);
}

void WCollisionInstance::CalcPosition(UMath::Vector3 &pos) const {
    bool needsCross = NeedsCrossProduct();
    pos.x = (-fInvPosRadius.x * fInvMatRow0Width.x - fInvPosRadius.y * fInvMatRow0Width.y) - fInvPosRadius.z * fInvMatRow0Width.z;
    pos.z = (-fInvPosRadius.x * fInvMatRow2Length.x - fInvPosRadius.y * fInvMatRow2Length.y) - fInvPosRadius.z * fInvMatRow2Length.z;

    if (needsCross) {
        UMath::Vector4 upVec;
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(fInvMatRow0Width),
                           upVec);
        pos.y = (-fInvPosRadius.x * upVec.x - fInvPosRadius.y * upVec.y) - fInvPosRadius.z * upVec.z;
    } else {
        pos.y = -fInvPosRadius.y;
    }
}

void WCollisionInstance::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    bool needsCross = NeedsCrossProduct();
    m.v0.x = fInvMatRow0Width.x;
    m.v0.y = fInvMatRow0Width.y;
    m.v0.z = fInvMatRow0Width.z;
    m.v0.w = 0.0f;

    if (needsCross) {
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(fInvMatRow0Width),
                           m.v1);
        m.v1.w = 0.0f;
    } else {
        m.v1.x = 0.0f;
        m.v1.y = 1.0f;
        m.v1.z = 0.0f;
        m.v1.w = 0.0f;
    }

    m.v2.x = fInvMatRow2Length.x;
    m.v2.y = fInvMatRow2Length.y;
    m.v2.z = fInvMatRow2Length.z;
    m.v2.w = 0.0f;

    if (addXLate) {
        m.v3.x = fInvPosRadius.x;
        m.v3.y = fInvPosRadius.y;
        m.v3.z = fInvPosRadius.z;
        m.v3.w = 1.0f;
    } else {
        m.v3.x = 0.0f;
        m.v3.y = 0.0f;
        m.v3.z = 0.0f;
        m.v3.w = 1.0f;
    }
}
