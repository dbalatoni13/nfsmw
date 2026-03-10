#include "ICEManager.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
