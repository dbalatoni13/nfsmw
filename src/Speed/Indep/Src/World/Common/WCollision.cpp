#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &r);

void WCollisionObject::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    const unsigned int *src = reinterpret_cast<const unsigned int *>(&this->fMat);
    unsigned int *dst = reinterpret_cast<unsigned int *>(&m);
    for (unsigned int i = 0; i < 0x10; ++i) {
        dst[i] = src[i];
    }

    if (addXLate) {
        m.v3.x = this->fPosRadius.x;
        m.v3.y = this->fPosRadius.y;
        m.v3.z = this->fPosRadius.z;
        m.v3.w = 1.0f;
        return;
    }

    m.v3.x = 0.0f;
    m.v3.y = 0.0f;
    m.v3.z = 0.0f;
    m.v3.w = 1.0f;
}

float WCollisionInstance::CalcSphericalRadius() const {
    float maxExtent = WWorldMath::wmax(this->fInvMatRow2Length.w, this->fInvPosRadius.w);
    maxExtent = WWorldMath::wmax(this->fHeight, maxExtent);
    return WWorldMath::wmax(this->fInvMatRow0Width.w, maxExtent);
}

void WCollisionInstance::CalcPosition(UMath::Vector3 &pos) const {
    bool needsCross = this->NeedsCrossProduct();
    pos.x = (-this->fInvPosRadius.x * this->fInvMatRow0Width.x - this->fInvPosRadius.y * this->fInvMatRow0Width.y) - this->fInvPosRadius.z * this->fInvMatRow0Width.z;
    pos.z = (-this->fInvPosRadius.x * this->fInvMatRow2Length.x - this->fInvPosRadius.y * this->fInvMatRow2Length.y) - this->fInvPosRadius.z * this->fInvMatRow2Length.z;

    if (needsCross) {
        UMath::Vector4 upVec;
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow0Width),
                           upVec);
        pos.y = (-this->fInvPosRadius.x * upVec.x - this->fInvPosRadius.y * upVec.y) - this->fInvPosRadius.z * upVec.z;
    } else {
        pos.y = -this->fInvPosRadius.y;
    }
}

void WCollisionInstance::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    bool needsCross = this->NeedsCrossProduct();
    m.v0.x = this->fInvMatRow0Width.x;
    m.v0.y = this->fInvMatRow0Width.y;
    m.v0.z = this->fInvMatRow0Width.z;
    m.v0.w = 0.0f;

    if (needsCross) {
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow0Width),
                           m.v1);
        m.v1.w = 0.0f;
    } else {
        m.v1.x = 0.0f;
        m.v1.y = 1.0f;
        m.v1.z = 0.0f;
        m.v1.w = 0.0f;
    }

    m.v2.x = this->fInvMatRow2Length.x;
    m.v2.y = this->fInvMatRow2Length.y;
    m.v2.z = this->fInvMatRow2Length.z;
    m.v2.w = 0.0f;

    if (addXLate) {
        m.v3.x = this->fInvPosRadius.x;
        m.v3.y = this->fInvPosRadius.y;
        m.v3.z = this->fInvPosRadius.z;
        m.v3.w = 1.0f;
    } else {
        m.v3.x = 0.0f;
        m.v3.y = 0.0f;
        m.v3.z = 0.0f;
        m.v3.w = 1.0f;
    }
}
