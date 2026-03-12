#include "Speed/Indep/bWare/Inc/bMath.hpp"

bQuaternion bIdentityQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

// UNSOLVED
bQuaternion &bQuaternion::Slerp(bQuaternion &r, const bQuaternion &target, float t) const {
    float cos_theta = bDot(reinterpret_cast<const bVector4 *>(this), reinterpret_cast<const bVector4 *>(&target));
    float scale1;
    float scale2;

    if ((1.0f - bAbs(cos_theta)) > 0.0001f) {
        unsigned short theta = bACos(bAbs(cos_theta));
        float sin_theta = bSin(theta);
        unsigned short a1 = static_cast<unsigned short>(static_cast<int>((1.0f - t) * static_cast<float>(theta)) & 0xffff);
        unsigned short a2 = static_cast<unsigned short>(static_cast<int>(t * static_cast<float>(theta)) & 0xffff);

        scale1 = bSin(a1) / sin_theta;
        scale2 = bSin(a2) / sin_theta;
    } else {
        scale1 = 1.0f - t;
        scale2 = t;
    }

    if (cos_theta < 0.0f) {
        scale1 = -scale1;
    }

    bQuaternion qtemp;
    bScale(reinterpret_cast<bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(this), scale1);
    bScaleAdd(reinterpret_cast<bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(&target), scale2);
    r = qtemp;
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
        int uVar3 = *param_2 < param_2[5];
        int uVar1 = uVar3 + 1;
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

        afStack_10[3] = (param_2[uVar1 * 4 + uVar3] - param_2[uVar3 * 4 + uVar1]) * fVar5;
        afStack_10[uVar1] =
            (*(float *)((int)param_2 + uVar1 * 4 + iVar2) + *(float *)((int)param_2 + iVar4 + uVar1 * 0x10)) * fVar5;
        afStack_10[uVar3] = (*(float *)((int)param_2 + uVar3 * 4 + iVar2) + *(float *)((int)param_2 + iVar4 + uVar3 * 0x10)) * fVar5;
        param_1[0] = afStack_10[0];
        param_1[1] = afStack_10[1];
        param_1[2] = afStack_10[2];
        param_1[3] = afStack_10[3];
    }
}
