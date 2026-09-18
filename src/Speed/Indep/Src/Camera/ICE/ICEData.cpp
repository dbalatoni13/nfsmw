#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bool bMirrorICEData = 0; // size: 0x4, address: 0x80417044

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
        bPlatEndianSwap(&fFocalDistance[i]);
        bPlatEndianSwap(&fNoiseAmplitude[i]);
        bPlatEndianSwap(&fNoiseFrequency[i]);
    }
}

void ICEData::GetEye(int n, ICE::Vector3 *p) {

    p->x = vEye[n][0];
    p->y = vEye[n][1];
    p->z = vEye[n][2];

    if (bMirrorICEData) {
        p->y = -p->y;
    }

    if (nSpaceEye == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}

void ICEData::GetLook(int n, ICE::Vector3 *p) {

    p->x = vLook[n][0];
    p->y = vLook[n][1];
    p->z = vLook[n][2];

    if (bMirrorICEData) {
        p->y = -p->y;
    }

    if (nSpaceLook == 3) {
        TheICEManager.FixAnimElevation(p);
    }
}

int ICE::KeysShared(ICEData *pKey0, int n0, ICEData *pKey1, int n1) {

    if (ICE::Abs(pKey0->fTangentLength[n0] - pKey1->fTangentLength[n1]) > 0.001f) {
        return 0;
    }
    if (ICE::Abs(pKey0->fDutch[n0] - pKey1->fDutch[n1]) > 0.001f) {
        return 0;
    }
    if (ICE::Abs(pKey0->fLens[n0] - pKey1->fLens[n1]) > 0.001f) {
        return 0;
    }

    return ICE::KeysSharedSpace(pKey0, n0, pKey1, n1);
}

int ICE::KeysSharedSpace(ICEData *pKey0, int n0, ICEData *pKey1, int n1) {

    if (pKey0->nType != pKey1->nType) {
        return 0;
    }
    if (pKey0->nSpaceEye != pKey1->nSpaceEye) {
        return 0;
    }
    if (pKey0->nSpaceLook != pKey1->nSpaceLook) {
        return 0;
    }

    for (int i = 0; i < 3; i++) {
        if (ICE::Abs(pKey0->vEye[n0][i] - pKey1->vEye[n1][i]) > 0.001f) {
            return 0;
        }
        if (ICE::Abs(pKey0->vLook[n0][i] - pKey1->vLook[n1][i]) > 0.001f) {
            return 0;
        }
    }

    return 1;
}

void ICEGroup::FlushAllocatedTracks() {

    ICETrack *p_track = TrackList.GetHead();

    for (;;) {

        if (p_track == (ICETrack *)&TrackList) {
            break;
        }

        ICETrack *p_next = p_track->GetNext();

        if (p_track->IsAllocated()) {

            p_track->Remove();
            delete p_track;
            NumTracks--;
        }

        p_track = p_next;
    }
}

ICETrack *ICEGroup::GetTrack(int n) {

    ICETrack *p_track = TrackList.GetNode(n);

    if (p_track == (ICETrack *)&TrackList) {
        p_track = NULL;
    }

    return p_track;
}

ICETrack *ICEGroup::GetTrack(char *name) {

    ICETrack *p_track = TrackList.GetHead();

    for (;;) {

        if (p_track == (ICETrack *)&TrackList) {
            break;
        }

        if (bStrCmp(name, p_track->Name) == 0) {

            return p_track;
        }

        p_track = p_track->GetNext();
    }

    return NULL;
}

void ICETrack::PlatEndianSwap() {

    bPlatEndianSwap(&Start);
    bPlatEndianSwap(&Length);
    bPlatEndianSwap(&NumKeys);

    for (int i = 0; i < NumKeys; i++) {
        Keys[i].PlatEndianSwap();
    }
}

int ICETrack::GetContext() {

    if (Group == NULL) {
        return 4;
    }

    return Group->Context;
}

int ICETrack::GetKeyNumber(float f_parameter) {

    int n = NumKeys - 1;

    while (n > 0 && Keys[n].fParameter > f_parameter) {

        n--;
    }

    return n;
}

float ICETrack::GetParameter() {

    float f_param = 0.0f;

    switch (GetContext()) {

    case ICE_CONTEXT_NIS: {

        ICEScene *scene = ICE::FindAnimScene();

        if (scene) {

            f_param = (scene->GetTimeElapsed() - scene->GetTimeStart()) /
                      (scene->GetTimeTotalLength() - scene->GetTimeStart());
        }
    } break;

    case ICE_CONTEXT_FMV:
    case ICE_CONTEXT_GENERIC:

        if (Length > 0.0f) {

            f_param = ICE::Min(1.0f, (TheICEManager.GetTimerSeconds() - Start) / Length);
        }
        break;

    case ICE_CONTEXT_REPLAY:

        if (Length > 0.0f) {

            f_param = ICE::Min(1.0f, (TheICEManager.GetTimerSeconds() - Start) / Length);
        }
        break;

    default:
        break;
    }

    return f_param;
}

ICEData *ICETrack::GetCameraData(float *p_start, float *p_end, float *p_current) {

    float f_parameter = GetParameter();

    if (f_parameter != ICE::Clamp(f_parameter, 0.0f, 1.0f)) {

        return 0;
    }

    int n = GetKeyNumber(f_parameter);

    if (p_current != 0) {

        *p_current = f_parameter;
    }

    if (p_start != 0) {

        *p_start = GetKeyParameter(n);
    }

    if (p_end != 0) {

        *p_end = GetKeyParameter(n + 1);
    }

    return GetKey(n);
}

void ICEShakeData::PlatEndianSwap() {

    bPlatEndianSwap(&q[0]);
    bPlatEndianSwap(&q[1]);
    bPlatEndianSwap(&q[2]);
    bPlatEndianSwap(&p[0]);
    bPlatEndianSwap(&p[1]);
    bPlatEndianSwap(&p[2]);
}

void ICEShakeGroup::FlushAllocatedTracks() {

    ICEShakeTrack *p_track = TrackList.GetHead();

    for (;;) {

        if (p_track == (ICEShakeTrack *)&TrackList) {
            break;
        }

        ICEShakeTrack *p_next = p_track->GetNext();

        if (p_track->IsAllocated()) {

            p_track->Remove();
            delete p_track;
            NumTracks--;
        }

        p_track = p_next;
    }
}

ICEShakeTrack *ICEShakeGroup::GetTrack(int n) {

    ICEShakeTrack *p_track = TrackList.GetNode(n);

    if (p_track == (ICEShakeTrack *)&TrackList) {
        p_track = NULL;
    }

    return p_track;
}

void ICEShakeTrack::PlatEndianSwap() {

    bPlatEndianSwap(&NumKeys);

    for (int i = 0; i < NumKeys; i++) {
        Keys[i].PlatEndianSwap();
    }
}
