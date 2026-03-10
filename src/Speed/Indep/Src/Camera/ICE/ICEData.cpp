#include "ICEManager.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

bool bMirrorICEData;

void ICEData::PlatEndianSwap() {
    bPlatEndianSwap(&fParameter);
    bPlatEndianSwap(&nShakeType);

    for (int i = 0; i < 2; i++) {
        bPlatEndianSwap(&fTangentLength[i]);

        for (int j = 0; j < 3; j++) {
            bPlatEndianSwap(&vEye[i][j]);
            bPlatEndianSwap(&vLook[i][j]);
        }

        bPlatEndianSwap(&fDutch[i]);
        bPlatEndianSwap(&fLens[i]);
        bPlatEndianSwap(&fNearClip[i]);
        bPlatEndianSwap(&fNoiseAmplitude[i]);
        bPlatEndianSwap(&fFocalDistance[i]);
        bPlatEndianSwap(&fNoiseFrequency[i]);
    }
}

void ICEData::GetEye(int i, ICE::Vector3 *p) {
    p->x = vEye[i][0];
    p->y = vEye[i][1];
    p->z = vEye[i][2];

    if (bMirrorICEData) {
        p->y = -p->y;
    }
    if (nSpaceEye == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}

void ICEData::GetLook(int i, ICE::Vector3 *p) {
    p->x = vLook[i][0];
    p->y = vLook[i][1];
    p->z = vLook[i][2];

    if (bMirrorICEData) {
        p->y = -p->y;
    }
    if (nSpaceLook == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}

namespace ICE {

bool KeysSharedSpace(ICEData *p1, int n1, ICEData *p2, int n2);

bool KeysShared(ICEData *p1, int n1, ICEData *p2, int n2) {
    if (UMath::Abs(p1->fTangentLength[n1] - p2->fTangentLength[n2]) > 0.001f) {
        return false;
    }
    if (UMath::Abs(p1->fDutch[n1] - p2->fDutch[n2]) > 0.001f) {
        return false;
    }
    if (UMath::Abs(p1->fLens[n1] - p2->fLens[n2]) > 0.001f) {
        return false;
    }
    return KeysSharedSpace(p1, n1, p2, n2);
}

bool KeysSharedSpace(ICEData *p1, int n1, ICEData *p2, int n2) {
    if (p1->nType != p2->nType) {
        return false;
    }
    if (p1->nSpaceEye != p2->nSpaceEye) {
        return false;
    }
    if (p1->nSpaceLook != p2->nSpaceLook) {
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (UMath::Abs(p1->vEye[n1][i] - p2->vEye[n2][i]) > 0.001f) {
            return false;
        }
        if (UMath::Abs(p1->vLook[n1][i] - p2->vLook[n2][i]) > 0.001f) {
            return false;
        }
    }
    return true;
}

} // namespace ICE
