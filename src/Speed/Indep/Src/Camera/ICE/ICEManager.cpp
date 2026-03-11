#include "ICEManager.hpp"
#include "ICEAnimScene.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Animation/AnimDirectory.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern Timer RealTimer;
extern AnimDirectory *TheAnimDirectory;
extern int bUseOldDutch;

int bStrNICmp(const char *s1, const char *s2, int n);

struct ICEAnchor;
ICEAnchor *GetICEAnchor();

static float GetGroundElevation(const ICE::Vector3 *position);
static void ICEGetPlayerCarTransform(ICE::Matrix4 *matrix);

namespace ICEReplay {
ICETrack *ChooseGoodCamera(ICEAnchor *p_car, ICEGroup *p_replay_cameras, int num_replay_cameras);
}

namespace ICE {
ICEScene *FindAnimScene();
unsigned int GetSceneCount();
unsigned int GetSceneHash(unsigned int slot);
void GetNameOfSceneHash(unsigned int hash, char *name);
bool KeysShared(ICEData *p1, int n1, ICEData *p2, int n2);
}

static const char *GenericCategoryNames[2] = {"Cinematics", "Debug"};

ICEManager TheICEManager;

void ICEGroup::FlushAllocatedTracks() {
    ICETrack *track = TrackList.GetHead();

    while (track != TrackList.EndOfList()) {
        ICETrack *next = track->GetNext();

        if (track->IsAllocated()) {
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

        if (track->IsAllocated()) {
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
    if (parameters[i * 33] <= f_param) {
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
    bPlatEndianSwap(&q[0]);
    bPlatEndianSwap(&q[1]);
    bPlatEndianSwap(&q[2]);
    bPlatEndianSwap(&p[0]);
    bPlatEndianSwap(&p[1]);
    bPlatEndianSwap(&p[2]);
}

void ICEShakeTrack::PlatEndianSwap() {
    bPlatEndianSwap(&NumKeys);

    for (int i = 0; i < NumKeys; i++) {
        Keys[i].PlatEndianSwap();
    }
}

ICEManager::ICEManager() {
    nState = 0;
    nTrack = 0;
    nHandle = 0;
    nOption = 0;
    nSetting = 0;
    nSceneHash = 0;
    nExitConfirmOption = 0;
    nDeleteConfirmOption = 0;
    nCopyMode = 0;
    nNisCameras = 0;
    nFmvCameras = 0;
    nReplayCameras = 0;
    nGenericCameras = 0;
    pNisCameras = 0;
    pFmvCameras = 0;
    pReplayCameras = 0;
    pGenericCameras = 0;
    pShakeGroup = 0;
    nContext = 3;
    fAnimElevation = 0.0f;
    fParameterStart = 0.0f;
    fParameterLength = 0.0f;
    fParameterLengthBackup = 0.0f;
    nPlayGenericGroupHash = bStringHash("");
    nPlayGenericTrackName[0] = 0;
    pPlaybackTrack = 0;
    ICEReplay::ClearRecentlyUsed();
    bSmoothExit = false;
    nMarkerIndex = -1;
    bUseRealTime = false;
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
    if (track != 0) {
        return track->GetKeyNumber(f_param);
    }

    return 0;
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
    if (track == 0) {
        return 0.0f;
    }
    if (i < 0) {
        return 0.0f;
    }
    if (i < track->NumKeys) {
        return track->Keys[i].fParameter;
    }

    return 1.0f;
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

float ICEManager::GetAnimElevationFixup(ICE::Vector3 *position) {
    float elevation = GetGroundElevation(position);
    if (0.0f < elevation) {
        return elevation - fAnimElevation;
    }
    return 0.0f;
}

void ICEManager::FixAnimElevation(ICE::Vector3 *position) {
    ICEScene *scene = ICE::FindAnimScene();
    if (scene != 0 && scene->IsCameraFixingElevation()) {
        ICE::Vector3 world_position;
        ICE::MulVector(&world_position, reinterpret_cast<const ICE::Matrix4 *>(&scene->GetSceneTransformMatrix()), position);
        if (IsEditorOff()) {
            const ICE::Vector3 *scene_origin = reinterpret_cast<const ICE::Vector3 *>(&scene->GetSceneTransformMatrix().v3);
            fAnimElevation = GetGroundElevation(scene_origin);
        }
        position->z += GetAnimElevationFixup(&world_position);
    }
}

void ICEManager::SetGenericCameraToPlay(const char *group_name, const char *track_name) {
    nPlayGenericGroupHash = Attrib::StringHash32(group_name);
    bStrNCpy(nPlayGenericTrackName, track_name, 13);
    nPlayGenericTrackName[13] = 0;
}

ICEData *ICEManager::GetCameraData(unsigned int scene_hash, int camTrack) {
    ICEGroup *group = GetNisCameraGroup(scene_hash);
    if (group != 0) {
        char name[14];
        bSPrintf(name, "cam%d", camTrack);
        pPlaybackTrack = group->GetTrack(name);
        if (pPlaybackTrack != 0) {
            return pPlaybackTrack->GetCameraData(0, 0, 0);
        }
    }
    return 0;
}

ICEData *ICEManager::GetCameraData(ICETrack **p_track, float *p_start, float *p_end) {
    if (p_track != 0) {
        *p_track = pPlaybackTrack;
    }
    if (pPlaybackTrack != 0) {
        return pPlaybackTrack->GetCameraData(p_start, p_end, 0);
    }
    return 0;
}

ICEData *ICEManager::GetNeighbour(ICEData *data, int key, ICETrack *track) {
    if (track == 0) {
        return 0;
    }
    int camera = track->GetKeyNumber(data);
    int offset = camera - 1;
    if (key) {
        offset = camera + 1;
    }
    return track->GetKey(offset);
}

ICEGroup *ICEManager::GetCameraGroup(ICEContext context, unsigned int handle) {
    int num_groups = 0;
    ICEGroup *groups = 0;

    switch (context) {
    case eDCE_NIS:
        groups = pNisCameras;
        num_groups = nNisCameras;
        break;
    case eDCE_FMV:
        groups = pFmvCameras;
        num_groups = nFmvCameras;
        break;
    case eDCE_REPLAY:
        groups = pReplayCameras;
        num_groups = nReplayCameras;
        break;
    case eDCE_GENERIC:
        groups = pGenericCameras;
        num_groups = nGenericCameras;
        break;
    default:
        break;
    }

    for (int i = 0; i < num_groups; i++) {
        ICEGroup *group = &groups[i];
        if (handle == group->GetHandle()) {
            return group;
        }
    }
    return 0;
}

ICEGroup *ICEManager::AddCameraGroup(ICEContext context, unsigned int handle) {
    ICEGroup *group = GetCameraGroup(context, handle);
    if (group != 0) {
        return group;
    }

    switch (context) {
    case eDCE_NIS: {
        int index = nNisCameras;
        if (index > 0xff) {
            return group;
        }
        group = &pNisCameras[index];
        nNisCameras = index + 1;
        break;
    }
    case eDCE_FMV: {
        int index = nFmvCameras;
        if (index > 9) {
            return group;
        }
        group = &pFmvCameras[index];
        nFmvCameras = index + 1;
        break;
    }
    case eDCE_REPLAY: {
        int index = nReplayCameras;
        if (index > 0x31) {
            return group;
        }
        group = &pReplayCameras[index];
        nReplayCameras = index + 1;
        break;
    }
    case eDCE_GENERIC: {
        int index = nGenericCameras;
        if (index > 0x31) {
            return group;
        }
        group = &pGenericCameras[index];
        nGenericCameras = index + 1;
        break;
    }
    default:
        return group;
    }

    group->SetContext(context);
    group->SetHandle(handle);
    return group;
}

void ICEManager::LoadCameraSet(bChunk *set_chunk) {
    ICEContext context = eDCE_NOCONTEXT;
    unsigned int id = set_chunk->GetID();

    switch (id) {
    case 0x8003B200:
        context = eDCE_NIS;
        break;
    case 0x8003B201:
        context = eDCE_FMV;
        break;
    case 0x8003B202:
        context = eDCE_REPLAY;
        break;
    case 0x8003B203:
        context = eDCE_GENERIC;
        break;
    }

    for (bChunk *chunk = set_chunk->GetFirstChunk(); chunk != set_chunk->GetLastChunk(); chunk = chunk->GetNext()) {
        bPlatEndianSwap(reinterpret_cast<unsigned int *>(chunk->GetData()));
        bPlatEndianSwap(reinterpret_cast<unsigned int *>(chunk->GetData() + 4));
        ICEGroup *group = AddCameraGroup(context, *reinterpret_cast<unsigned int *>(chunk->GetData()));
        if (group != 0) {
            int num_tracks = *reinterpret_cast<int *>(chunk->GetData() + 4);
            ICETrack *track = reinterpret_cast<ICETrack *>(chunk->GetData() + 8);
            for (int i = 0; i < num_tracks; i++) {
                track->PlatEndianSwap();
                group->AddTrack(track);
                track = reinterpret_cast<ICETrack *>(reinterpret_cast<char *>(track) + track->MemoryImageSize());
            }
        }
    }
}

void ICEManager::UnloadCameraSet(bChunk *set_chunk) {
    ICEContext context = eDCE_NOCONTEXT;
    unsigned int id = set_chunk->GetID();

    switch (id) {
    case 0x8003B200:
        context = eDCE_NIS;
        break;
    case 0x8003B201:
        context = eDCE_FMV;
        break;
    case 0x8003B202:
        context = eDCE_REPLAY;
        break;
    case 0x8003B203:
        context = eDCE_GENERIC;
        break;
    }

    for (bChunk *chunk = set_chunk->GetFirstChunk(); chunk != set_chunk->GetLastChunk(); chunk = chunk->GetNext()) {
        ICEGroup *group = GetCameraGroup(context, *reinterpret_cast<unsigned int *>(chunk->GetData()));
        if (group != 0) {
            group->NumTracks = 0;
            group->FlushTracks();
        }
    }

    int num_groups = 0;
    ICEGroup *groups = 0;

    switch (context) {
    case eDCE_NIS:
        groups = pNisCameras;
        num_groups = nNisCameras;
        break;
    case eDCE_FMV:
        groups = pFmvCameras;
        num_groups = nFmvCameras;
        break;
    case eDCE_REPLAY:
        groups = pReplayCameras;
        num_groups = nReplayCameras;
        break;
    case eDCE_GENERIC:
        groups = pGenericCameras;
        num_groups = nGenericCameras;
        break;
    }

    for (int i = 0; i < num_groups; i++) {
        groups[i].FlushAllocatedTracks();
    }
}

void ICEManager::LoadCameraShakes(bChunk *set_chunk) {
    bPlatEndianSwap(reinterpret_cast<unsigned int *>(set_chunk->GetData()));
    ICEShakeGroup *group = pShakeGroup;
    if (group != 0) {
        int num_tracks = *reinterpret_cast<int *>(set_chunk->GetData());
        ICEShakeTrack *track = reinterpret_cast<ICEShakeTrack *>(set_chunk->GetData() + 4);
        for (int i = 0; i < num_tracks; i++) {
            track->PlatEndianSwap();
            group->AddTrack(track);
            track = reinterpret_cast<ICEShakeTrack *>(reinterpret_cast<char *>(track) + track->MemoryImageSize());
        }
    }
}

void ICEManager::UnloadCameraShakes(bChunk *set_chunk) {
    ICEShakeGroup *group = pShakeGroup;
    group->NumTracks = 0;
    while (!group->TrackList.IsEmpty()) {
        ICEShakeTrack *track = group->TrackList.RemoveHead();
        if (track->IsAllocated() && track != 0) {
            delete track;
        }
    }
    group->FlushAllocatedTracks();
}

void ICEManager::Init() {
    pNisCameras = new ICEGroup[256];
    pFmvCameras = new ICEGroup[10];
    pReplayCameras = new ICEGroup[50];
    pGenericCameras = new ICEGroup[50];
    pShakeGroup = new ICEShakeGroup;
}

void ICEManager::Resolve() {
    unsigned int num_scenes = ICE::GetSceneCount();

    {
        for (unsigned int scene = 0; scene < num_scenes; scene++) {
            unsigned int scene_hash = ICE::GetSceneHash(scene);
            char scene_name[16];
            ICE::GetNameOfSceneHash(scene_hash, scene_name);
            char *name_ptr = scene_name;

            if (bStrNICmp(name_ptr, "FMV", 3) != 0 &&
                bStrNICmp(name_ptr, "replay", 6) != 0 &&
                bStrNICmp(name_ptr, "clip", 4) != 0) {
                if (GetNisCameraGroup(scene_hash) == 0) {
                    if (nNisCameras <= 0xff) {
                        pNisCameras[nNisCameras].Context = eDCE_NIS;
                        pNisCameras[nNisCameras].Handle = scene_hash;
                        nNisCameras++;
                    }
                }
            }
        }
    }

    {
        for (unsigned int fmv = 0; fmv < num_scenes; fmv++) {
            unsigned int scene_hash = ICE::GetSceneHash(fmv);
            char scene_name[16];
            ICE::GetNameOfSceneHash(scene_hash, scene_name);

            if (bStrNICmp(scene_name, "FMV", 3) == 0) {
                if (GetFmvCameraGroup(scene_hash) == 0) {
                    if (nFmvCameras <= 9) {
                        pFmvCameras[nFmvCameras].Context = eDCE_FMV;
                        pFmvCameras[nFmvCameras].Handle = scene_hash;
                        nFmvCameras++;
                    }
                }
            }
        }
    }

    {
        int num_categories = ICE::GetReplayCategoryNumElements();
        for (int category = 0; category < num_categories; category++) {
            unsigned int category_hash = ICE::GetReplayCategoryHash(category);

            if (GetReplayCameraGroup(category_hash) == 0) {
                if (nReplayCameras <= 0x31) {
                    pReplayCameras[nReplayCameras].Context = eDCE_REPLAY;
                    pReplayCameras[nReplayCameras].Handle = category_hash;
                    nReplayCameras++;
                }
            }
        }
    }

    {
        for (int name = 0; name < 2; name++) {
            unsigned int name_hash = bStringHash(GenericCategoryNames[name]);

            if (GetGenericCameraGroup(name_hash) == 0) {
                if (nGenericCameras <= 0x31) {
                    pGenericCameras[nGenericCameras].Context = eDCE_GENERIC;
                    pGenericCameras[nGenericCameras].Handle = name_hash;
                    nGenericCameras++;
                }
            }
        }
    }
}

bool ICEManager::ChooseCameraPlaybackTrack() {
    pPlaybackTrack = 0;
    bUseOldDutch = 0;
    ICEScene *scene = ICE::FindAnimScene();
    if (scene != 0) {
        unsigned int scene_hash = scene->GetSceneHash();
        ICEGroup *group = GetNisCameraGroup(scene_hash);
        if (group != 0) {
            char name[14];
            bSPrintf(name, "Track %d", scene->GetCameraTrackNumber());
            pPlaybackTrack = group->GetTrack(name);
            if (pPlaybackTrack == 0) {
                char scene_name[16];
                ICE::GetNameOfSceneHash(scene_hash, scene_name);
            }
            bUseOldDutch = 1;
        }
    } else {
        pPlaybackTrack = ChooseGenericCamera();
        if (pPlaybackTrack == 0) {
            pPlaybackTrack = ICEReplay::ChooseGoodCamera(GetICEAnchor(), pReplayCameras, nReplayCameras);
            if (pPlaybackTrack != 0) {
                pPlaybackTrack->Start = GetTimerSeconds();
            }
        }
    }
    return pPlaybackTrack != 0;
}

int ICEManager::ChooseGoodSceneCameraTrackIndex(unsigned int scene_hash, const ICE::Matrix4 *scene_origin) {
    bMatrix4 mCarToWorld;
    int bestTrack = 0;

    ICEGetPlayerCarTransform(reinterpret_cast<ICE::Matrix4 *>(&mCarToWorld));

    for (int i = 0; i < nNisCameras; i++) {
        if (scene_hash == pNisCameras[i].Handle) {
            ICEGroup *group = &pNisCameras[i];
            int numTracks = group->GetNumTracks();

            if (numTracks < 2) {
                break;
            }

            float bestDot = -1.0f;

            for (int k = 0; k < numTracks; k++) {
                ICETrack *track = group->GetTrack(k);

                if (track->GetNumKeys() > 0) {
                    ICEData *key = track->GetKey(0);
                    int n = 0;
                    if (key->bSmooth != 0) {
                        n = 1;
                    }

                    bVector3 v_eye;
                    key[n].GetEye(n, reinterpret_cast<ICE::Vector3 *>(&v_eye));

                    switch (key[n].nSpaceEye) {
                    case 2:
                        bAdd(&v_eye, &v_eye, reinterpret_cast<const bVector3 *>(&mCarToWorld.v3));
                        break;
                    case 0:
                        eMulVector(&v_eye, &mCarToWorld, &v_eye);
                        break;
                    case 3:
                        eMulVector(&v_eye, reinterpret_cast<const bMatrix4 *>(scene_origin), &v_eye);
                        break;
                    }

                    bVector3 v_look;
                    key[n].GetLook(n, reinterpret_cast<ICE::Vector3 *>(&v_look));

                    switch (key[n].nSpaceLook) {
                    case 2:
                        bAdd(&v_look, &v_look, reinterpret_cast<const bVector3 *>(&mCarToWorld.v3));
                        break;
                    case 0:
                        eMulVector(&v_look, &mCarToWorld, &v_look);
                        break;
                    case 3:
                        eMulVector(&v_look, reinterpret_cast<const bMatrix4 *>(scene_origin), &v_look);
                        break;
                    }

                    bVector3 vCamDir;
                    bSub(&vCamDir, &v_eye, &v_look);
                    bNormalize(&vCamDir, &vCamDir);

                    bVector3 *pCarDir = reinterpret_cast<bVector3 *>(&mCarToWorld.v0);
                    float dot = bDot(&vCamDir, pCarDir);

                    if (bestDot < dot) {
                        bestTrack = k;
                        bestDot = dot;
                    }
                }
            }
        }
    }

    return bestTrack;
}

void ICEManager::GetSlope(ICE::Vector3 *eye, ICE::Vector3 *look, float *fov, float *dutch, ICEData *data, int key, ICETrack *track) {
    ICE::Vector3 v_eye_slope;
    ICE::Vector3 v_look_slope;
    float f_dutch_slope = 0.0f;
    float f_lens_slope = 0.0f;

    if (data->nType != 0) {
        ICEData *p_neighbour = GetNeighbour(data, key, track);
        bool shared_slope = false;
        if (p_neighbour != 0 && p_neighbour->nType != 0 &&
            ICE::KeysShared(data, key, p_neighbour, key ^ 1)) {
            ICE::Vector3 v0;
            ICE::Vector3 v1;
            ICE::Vector3 v_eye0;
            ICE::Vector3 v_eye1;
            ICE::Vector3 v_look0;
            ICE::Vector3 v_look1;

            shared_slope = true;

            p_neighbour->GetEye(0, &v0);
            p_neighbour->GetEye(1, &v1);
            bSub(reinterpret_cast<bVector3 *>(&v_eye0), reinterpret_cast<const bVector3 *>(&v1), reinterpret_cast<const bVector3 *>(&v0));

            p_neighbour->GetLook(0, &v0);
            p_neighbour->GetLook(1, &v1);
            bSub(reinterpret_cast<bVector3 *>(&v_look0), reinterpret_cast<const bVector3 *>(&v1), reinterpret_cast<const bVector3 *>(&v0));

            float f_dutch0 = p_neighbour->fDutch[0];
            float f_lens0 = p_neighbour->fLens[0];
            float f_dutch1 = p_neighbour->fDutch[1];
            float f_lens1 = p_neighbour->fLens[1];

            data->GetEye(0, &v0);
            data->GetEye(1, &v1);
            bSub(reinterpret_cast<bVector3 *>(&v_eye1), reinterpret_cast<const bVector3 *>(&v1), reinterpret_cast<const bVector3 *>(&v0));

            data->GetLook(0, &v0);
            data->GetLook(1, &v1);
            float f_dutch_data0 = data->fDutch[0];
            float f_lens_data1 = data->fLens[1];
            bSub(reinterpret_cast<bVector3 *>(&v_look1), reinterpret_cast<const bVector3 *>(&v1), reinterpret_cast<const bVector3 *>(&v0));
            float f_lens_data0 = data->fLens[0];
            float f_dutch_data1 = data->fDutch[1];

            float f_camera_size = GetIntervalSize(data, track);
            float f_neighbour_size = GetIntervalSize(p_neighbour, track);

            float f_neighbour_blend = f_camera_size / (f_camera_size + f_neighbour_size);
            float f_camera_blend = 1.0f - f_neighbour_blend;

            if (f_neighbour_size > 0.000001f) {
                f_neighbour_blend *= f_camera_size / f_neighbour_size;
            }

            float tangent_length = data->fTangentLength[key];
            bScale(reinterpret_cast<bVector3 *>(&v_eye_slope), reinterpret_cast<const bVector3 *>(&v_eye0), f_neighbour_blend * tangent_length);
            bScaleAdd(reinterpret_cast<bVector3 *>(&v_eye_slope), reinterpret_cast<const bVector3 *>(&v_eye_slope), reinterpret_cast<const bVector3 *>(&v_eye1), f_camera_blend * tangent_length);

            bScale(reinterpret_cast<bVector3 *>(&v_look_slope), reinterpret_cast<const bVector3 *>(&v_look0), f_neighbour_blend * tangent_length);
            bScaleAdd(reinterpret_cast<bVector3 *>(&v_look_slope), reinterpret_cast<const bVector3 *>(&v_look_slope), reinterpret_cast<const bVector3 *>(&v_look1), f_camera_blend * tangent_length);

            f_lens_slope = (f_dutch1 - f_lens1) * f_neighbour_blend * tangent_length + (f_dutch_data1 - f_lens_data1) * f_camera_blend * tangent_length;
            f_dutch_slope = (f_lens0 - f_dutch0) * f_neighbour_blend * tangent_length + (f_lens_data0 - f_dutch_data0) * f_camera_blend * tangent_length;
        }
        if (!shared_slope) {
            ICE::Vector3 v_eye0;
            ICE::Vector3 v_eye1;
            ICE::Vector3 v_look0;
            ICE::Vector3 v_look1;

            data->GetEye(0, &v_eye0);
            data->GetEye(1, &v_eye1);
            data->GetLook(0, &v_look0);
            data->GetLook(1, &v_look1);

            float tangent_length = data->fTangentLength[key];
            bSub(reinterpret_cast<bVector3 *>(&v_eye_slope), reinterpret_cast<const bVector3 *>(&v_eye1), reinterpret_cast<const bVector3 *>(&v_eye0));
            bScale(reinterpret_cast<bVector3 *>(&v_eye_slope), reinterpret_cast<const bVector3 *>(&v_eye_slope), tangent_length);
            bSub(reinterpret_cast<bVector3 *>(&v_look_slope), reinterpret_cast<const bVector3 *>(&v_look1), reinterpret_cast<const bVector3 *>(&v_look0));
            bScale(reinterpret_cast<bVector3 *>(&v_look_slope), reinterpret_cast<const bVector3 *>(&v_look_slope), tangent_length);
            f_lens_slope = tangent_length * (data->fLens[1] - data->fLens[0]);
            f_dutch_slope = tangent_length * (data->fDutch[1] - data->fDutch[0]);
        }
    }

    *eye = v_eye_slope;
    *look = v_look_slope;
    *fov = f_dutch_slope;
    *dutch = f_lens_slope;
}

static float GetGroundElevation(const ICE::Vector3 *position) {
    float ground_elevation = 0.0f;
    if (IsGameFlowInGame()) {
        UMath::Vector3 unswizzled_position;
        eUnSwizzleWorldVector(reinterpret_cast<const bVector3 &>(*position), reinterpret_cast<bVector3 &>(unswizzled_position));
        unswizzled_position.y += 4.0f;
        WCollisionMgr collisionMgr(0, 3);
        bool point_valid = collisionMgr.GetWorldHeightAtPointRigorous(unswizzled_position, ground_elevation, 0);
        if (!point_valid) {
            ground_elevation = position->z;
        }
    }
    return ground_elevation;
}

static void ICEGetPlayerCarTransform(ICE::Matrix4 *mCarToWorld) {
    bIdentity(reinterpret_cast<bMatrix4 *>(mCarToWorld));
    IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
    if (iplayer != 0) {
        IRigidBody *player_rigid_body = iplayer->GetSimable()->GetRigidBody();
        if (player_rigid_body != 0) {
            UMath::Matrix4 mat;
            player_rigid_body->GetMatrix4(mat);
            bConvertFromBond(*reinterpret_cast<bMatrix4 *>(mCarToWorld), reinterpret_cast<const bMatrix4 &>(mat));
            const UMath::Vector3 &pos = player_rigid_body->GetPosition();
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pos), reinterpret_cast<bVector3 &>(reinterpret_cast<bMatrix4 *>(mCarToWorld)->v3));
        }
    }
}

int LoaderICECameras(bChunk *pChunk) {
    unsigned int id = pChunk->GetID();
    if (id == 0x0003B211) {
        TheICEManager.LoadCameraShakes(pChunk);
        return 1;
    } else if (id >= 0x8003B200 && id <= 0x8003B203) {
        TheICEManager.LoadCameraSet(pChunk);
        return 1;
    }
    return 0;
}

int UnloaderICECameras(bChunk *pChunk) {
    unsigned int id = pChunk->GetID();
    if (id == 0x0003B211) {
        TheICEManager.UnloadCameraShakes(pChunk);
        return 1;
    } else if (id >= 0x8003B200 && id <= 0x8003B203) {
        TheICEManager.UnloadCameraSet(pChunk);
        return 1;
    }
    return 0;
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
    if (TheAnimDirectory == nullptr) {
        return 0;
    }

    return TheAnimDirectory->GetSceneCount();
}

unsigned int GetSceneHash(unsigned int slot) {
    if (TheAnimDirectory == 0 || TheAnimDirectory->GetSceneCount() <= slot) {
        return 0;
    }
    AnimSceneLoadInfo info;
    TheAnimDirectory->GetSceneLoadInfo(slot, info);
    return info.mAnimSceneHash;
}

void GetNameOfSceneHash(unsigned int hash, char *name) {
    *name = 0;
    if (TheAnimDirectory != 0) {
        for (unsigned int i = 0; i < TheAnimDirectory->GetSceneCount(); i++) {
            AnimSceneLoadInfo info;
            TheAnimDirectory->GetSceneLoadInfo(i, info);
            if (hash == info.mAnimSceneHash) {
                char *filename = TheAnimDirectory->GetFileName(info.mSceneFileStartIndex);
                int pos = 0;
                while (filename[pos] != '_') {
                    pos++;
                }
                pos++;
                int start = pos;
                while (filename[pos] != '_') {
                    pos++;
                }
                int len = pos - start - 1;
                for (int j = start; len >= 0; j++, len--) {
                    *name = filename[j];
                    name++;
                }
                *name = 0;
                break;
            }
        }
    }
}

void FireEventTag(int key) {
    INIS *nis = INIS::Get();
    if (nis != 0) {
        char tagName[64];
        bSPrintf(tagName, "key_%d", key);
        nis->FireEventTag(tagName);
    }
}

} // namespace ICE

void ICECompleteEventTags() {
    ICETrack *p_track;
    float fParameter0;
    float fParameter1;
    TheICEManager.GetCameraData(&p_track, &fParameter0, &fParameter1);
    int key = TheICEManager.GetCameraIndex((fParameter0 + fParameter1) * 0.5f, p_track) + 1;
    if (p_track != 0) {
        int keyCount = p_track->GetNumKeys();
        for (; key < keyCount; key++) {
            ICE::FireEventTag(key);
        }
    }
}
