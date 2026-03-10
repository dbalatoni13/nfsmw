#include "ICEManager.hpp"
#include "ICEAnimScene.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Animation/AnimDirectory.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern Timer RealTimer;
extern AnimDirectory *TheAnimDirectory;

struct ICEAnchor;
ICEAnchor *GetICEAnchor();

namespace ICEReplay {
ICETrack *ChooseGoodCamera(ICEAnchor *p_car, ICEGroup *p_replay_cameras, int num_replay_cameras);
}

namespace ICE {
ICEScene *FindAnimScene();
unsigned int GetSceneCount();
}

ICEManager TheICEManager;

void ICEGroup::FlushAllocatedTracks() {
    ICETrack *track = TrackList.GetHead();

    while (track != TrackList.EndOfList()) {
        ICETrack *next = track->GetNext();

        if (track->Allocated != 0) {
            TrackList.Remove(track);
            delete track;
            NumTracks--;
        }

        track = next;
    }
}

ICETrack *ICEGroup::GetTrack(int n) {
    ICETrack *track = TrackList.GetNode(n);

    if (track == TrackList.EndOfList()) {
        track = 0;
    }

    return track;
}

ICETrack *ICEGroup::GetTrack(char *s) {
    ICETrack *track = TrackList.GetHead();

    while (track != TrackList.EndOfList()) {
        if (bStrCmp(s, track->Name) == 0) {
            return track;
        }

        track = track->GetNext();
    }

    return 0;
}

void ICEShakeGroup::FlushAllocatedTracks() {
    ICEShakeTrack *track = TrackList.GetHead();

    while (track != TrackList.EndOfList()) {
        ICEShakeTrack *next = track->GetNext();

        if (track->Allocated != 0) {
            TrackList.Remove(track);
            delete track;
            NumTracks--;
        }

        track = next;
    }
}

ICEShakeTrack *ICEShakeGroup::GetTrack(int n) {
    ICEShakeTrack *track = TrackList.GetNode(n);

    if (track == TrackList.EndOfList()) {
        track = 0;
    }

    return track;
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
    if (Group == 0) {
        return 4;
    }

    return Group->Context;
}

int ICETrack::GetKeyNumber(float f_param) {
    int i = NumKeys - 1;
    float *parameters = &Keys[0].fParameter;

    if (i < 1) {
        return i;
    }
    if (!(f_param < parameters[i * 33])) {
        return i;
    }

    do {
        i--;

        if (i < 1) {
            return i;
        }
    } while (parameters[i * 33] > f_param);

    return i;
}

float ICETrack::GetParameter() {
    float parameter = 0.0f;
    int context = GetContext();

    if (context == 0) {
        ICEScene *scene = ICE::FindAnimScene();

        if (scene != 0) {
            parameter = (scene->GetTimeElapsed() - scene->GetTimeStart()) / (scene->GetTimeTotalLength() - scene->GetTimeStart());
        }
    } else if (context == 1 || context == 2 || context == 3) {
        if (0.0f < Length) {
            parameter = (TheICEManager.GetTimerSeconds() - Start) / Length;

            if (1.0f < parameter) {
                parameter = 1.0f;
            }
        }
    }

    return parameter;
}

ICEData *ICETrack::GetCameraData(float *p_start, float *p_end, float *p_current) {
    float current = GetParameter();

    if (current < 0.0f || 1.0f < current) {
        return 0;
    }

    int i = GetKeyNumber(current);

    if (p_current != 0) {
        *p_current = current;
    }
    if (p_start != 0) {
        float start = 0.0f;

        if (i > -1 && i < NumKeys) {
            start = Keys[i].fParameter;
        }

        *p_start = start;
    }
    if (p_end != 0) {
        int next = i + 1;
        float end = 1.0f;

        if (next > -1 && next < NumKeys) {
            end = Keys[next].fParameter;
        }

        *p_end = end;
    }

    int clamped = i;

    if (clamped < 0) {
        clamped = 0;
    }

    int max = NumKeys - 1;

    if (max < clamped) {
        clamped = max;
    }

    if (i == clamped) {
        return &Keys[i];
    }

    return 0;
}

void ICEShakeData::PlatEndianSwap() {
    for (int i = 0; i < 3; i++) {
        bPlatEndianSwap(&q[i]);
        bPlatEndianSwap(&p[i]);
    }
}

void ICEShakeTrack::PlatEndianSwap() {
    bPlatEndianSwap(&NumKeys);

    for (int i = 0; i < NumKeys; i++) {
        Keys[i].PlatEndianSwap();
    }
}

ICEManager::ICEManager() {
    pNisCameras = 0;
    pFmvCameras = 0;
    pReplayCameras = 0;
    pGenericCameras = 0;
    pShakeGroup = 0;
    nNisCameras = 0;
    nFmvCameras = 0;
    nReplayCameras = 0;
    nGenericCameras = 0;
    pPlaybackTrack = 0;
    nState = 0;
    nTrack = 0;
    nHandle = 0;
    nOption = 0;
    nSetting = 0;
    nExitConfirmOption = 0;
    nDeleteConfirmOption = 0;
    nContext = 3;
    nCopyMode = 0;
    nSceneHash = 0;
    fAnimElevation = 0.0f;
    fParameterStart = 0.0f;
    fParameterLength = 0.0f;
    fParameterLengthBackup = 0.0f;
    nPlayGenericGroupHash = 0;
    bSmoothExit = false;
    nMarkerIndex = -1;
    bUseRealTime = false;
    ICEReplay::ClearRecentlyUsed();
}

float ICEManager::GetTimerSeconds() {
    int packed_time;

    if (!bUseRealTime) {
        packed_time = WorldTimer.GetPackedTime();
    } else {
        packed_time = RealTimer.GetPackedTime();
    }

    return packed_time / 4000.0f;
}

bool ICEManager::RefreshCameraSplines() {
    return false;
}

void ICEManager::Update() {}

ICEGroup *ICEManager::GetNisCameraGroup(unsigned int scene_hash) {
    for (int i = 0; i < nNisCameras; i++) {
        if (scene_hash == pNisCameras[i].Handle) {
            return &pNisCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetFmvCameraGroup(unsigned int scene_hash) {
    for (int i = 0; i < nFmvCameras; i++) {
        if (scene_hash == pFmvCameras[i].Handle) {
            return &pFmvCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetReplayCameraGroup(unsigned int category_hash) {
    for (int i = 0; i < nReplayCameras; i++) {
        if (category_hash == pReplayCameras[i].Handle) {
            return &pReplayCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetGenericCameraGroup(unsigned int name_hash) {
    for (int i = 0; i < nGenericCameras; i++) {
        if (name_hash == pGenericCameras[i].Handle) {
            return &pGenericCameras[i];
        }
    }

    return 0;
}

ICEShakeTrack *ICEManager::GetShakeTrack(unsigned int shake_type) {
    if (shake_type != 0 && pShakeGroup != 0) {
        for (int i = 0; i < pShakeGroup->NumTracks; i++) {
            ICEShakeTrack *track = pShakeGroup->GetTrack(i);

            if (track != 0 && bStringHash(track->Name) == shake_type) {
                return track;
            }
        }
    }

    return 0;
}

int ICEManager::GetCameraIndex(float f_param, ICETrack *track) {
    int camera_index = 0;

    if (track != 0) {
        camera_index = track->GetKeyNumber(f_param);
    }

    return camera_index;
}

float ICEManager::GetParameter() {
    float parameter = 0.0f;
    ICEScene *scene = ICE::FindAnimScene();

    if (scene != 0 && 0.0f < fParameterLength) {
        parameter = (scene->GetTimeElapsed() - fParameterStart) / fParameterLength;
    }

    return parameter;
}

float ICEManager::GetParameter(int i, ICETrack *track) {
    if (track != 0 && i > -1) {
        if (track->NumKeys <= i) {
            return 1.0f;
        }

        return track->Keys[i].fParameter;
    }

    return 0.0f;
}

float ICEManager::GetIntervalSize(ICEData *data, ICETrack *track) {
    int i = 0;

    if (track != 0) {
        i = data - track->Keys;
    }

    return GetParameter(i + 1, track) - GetParameter(i, track);
}

ICETrack *ICEManager::ChooseGenericCamera() {
    ICETrack *track = 0;
    ICEGroup *group = GetGenericCameraGroup(nPlayGenericGroupHash);

    if (group != 0) {
        track = group->GetTrack(nPlayGenericTrackName);

        if (track != 0) {
            track->Start = GetTimerSeconds();
        }
    }

    return track;
}

int ICEManager::GetNumSceneCameraTrack(unsigned int scene_hash) {
    ICEGroup *group = GetNisCameraGroup(scene_hash);
    int available_tracks = 0;

    if (group != 0) {
        available_tracks = group->NumTracks;
    }

    return available_tracks;
}

void ICEManager::ChooseReplayCamera() {
    float parameter = 0.0f;

    if (pPlaybackTrack != 0) {
        parameter = (GetTimerSeconds() - pPlaybackTrack->Start) / pPlaybackTrack->Length;
    }

    if (0.0f <= parameter) {
        ICETrack *track = ICEReplay::ChooseGoodCamera(GetICEAnchor(), pReplayCameras, nReplayCameras);

        if (track != 0) {
            pPlaybackTrack = track;
            pPlaybackTrack->Start = GetTimerSeconds();
        }
    }
}

namespace ICE {

ICEScene *FindAnimScene() {
    INIS *nis = INIS::Get();

    if (nis != 0) {
        return nis->GetScene();
    }

    return 0;
}

unsigned int GetSceneCount() {
    AnimDirectory *directory = TheAnimDirectory;

    if (directory == 0) {
        return 0;
    }

    return directory->GetSceneCount();
}

} // namespace ICE
