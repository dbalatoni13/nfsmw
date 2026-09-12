#include "ICEData.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"

void ICEData::GetEye(int i, ICE::Vector3 *p) {

    p->x = vEye[i][0];
    p->y = vEye[i][1];
    p->z = vEye[i][2];

    if (bMirrorICEData) {
        p->y = -vEye[i][1];
    }

    if (this->nSpaceEye == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}

void ICEData::GetLook(int i, struct ICE::Vector3 *p) {

    p->x = vLook[i][0];
    p->y = vLook[i][1];
    p->z = vLook[i][2];

    if (bMirrorICEData) {
        p->y = -vLook[i][1];
    }

    if (this->nSpaceLook == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}
