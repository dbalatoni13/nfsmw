#include "Speed/Indep/bWare/Inc/bMath.hpp"

bQuaternion bIdentityQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

// UNSOLVED
bQuaternion &bQuaternion::Slerp(bQuaternion &r, const bQuaternion &target, float t) const {
    float dot = this->x * target.x + this->y * target.y + this->z * target.z + this->w * target.w;
    float scale0 = 1.0f - t;
    float scale1 = t;

    if ((1.0f - bAbs(dot)) > 0.0001f) {
        unsigned short angle = static_cast<unsigned short>(0x4000 - bASin(dot));
        float sin_angle = bSin(angle);
        float fangle = static_cast<float>(angle);

        scale0 = bSin(static_cast<unsigned short>(static_cast<int>((1.0f - t) * fangle) & 0xffff)) / sin_angle;
        scale1 = bSin(static_cast<unsigned short>(static_cast<int>(t * fangle) & 0xffff)) / sin_angle;
    }

    if (dot < 0.0f) {
        scale0 = -scale0;
    }

    bVector4 temp(this->x * scale0, this->y * scale0, this->z * scale0, this->w * scale0);
    bScaleAdd(reinterpret_cast<bVector4 *>(&r), &temp, reinterpret_cast<const bVector4 *>(&target), scale1);
    return r;
}

void bMatrixToQuaternion(bQuaternion &quat, const bMatrix4 &m) {
    float *param_1 = reinterpret_cast<float *>(&quat);
    float *param_2 = const_cast<float *>(reinterpret_cast<const float *>(&m));
    float afStack_10[4];
    float fVar5 = *param_2 + param_2[5] + param_2[10];

    if (fVar5 > 0.0f) {
        fVar5 = bSqrt(fVar5 + 1.0f);
        float fVar6 = 0.5f / fVar5;
        param_1[3] = fVar5 * 0.5f;
        *param_1 = (param_2[6] - param_2[9]) * fVar6;
        param_1[1] = (param_2[8] - param_2[2]) * fVar6;
        param_1[2] = (param_2[1] - param_2[4]) * fVar6;
    } else {
        unsigned int uVar3 = static_cast<unsigned int>(*param_2 < param_2[5]);
        unsigned int uVar1 = uVar3 + 1;
        int iVar2 = uVar3 * 0x10;
        int iVar4 = uVar3 * 4;

        if (param_2[uVar3 * 5] < param_2[10]) {
            iVar2 = 0x20;
            iVar4 = 8;
            uVar1 = 3;
        }

        uVar1 = uVar1 % 3;
        uVar3 = (uVar1 + 1) % 3;
        fVar5 = bSqrt(((*(float *)((int)param_2 + iVar4 + iVar2) - param_2[uVar1 * 5]) - param_2[uVar3 * 5]) + 1.0f);
        *(float *)((int)afStack_10 + iVar4) = fVar5 * 0.5f;
        if (fVar5 != 0.0f) {
            fVar5 = 0.5f / fVar5;
        }

        float fVar6 = *(float *)((int)param_2 + uVar3 * 4 + iVar2);
        float fVar7 = *(float *)((int)param_2 + iVar4 + uVar3 * 0x10);
        afStack_10[3] = (param_2[uVar1 * 4 + uVar3] - param_2[uVar3 * 4 + uVar1]) * fVar5;
        afStack_10[uVar1] =
            (*(float *)((int)param_2 + uVar1 * 4 + iVar2) + *(float *)((int)param_2 + iVar4 + uVar1 * 0x10)) * fVar5;
        afStack_10[uVar3] = (fVar6 + fVar7) * fVar5;
        *param_1 = afStack_10[0];
        param_1[3] = afStack_10[3];
        param_1[1] = afStack_10[1];
        param_1[2] = afStack_10[2];
    }
}
