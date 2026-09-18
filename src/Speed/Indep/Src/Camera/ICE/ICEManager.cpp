#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

#define BCHUNK_ICE_CAMERA_SHAKE_GROUP (0x0003B211 + DATA_BCHUNK)

struct ICEAnchor;

float GetGroundElevation(const ICE::Vector3 *p);
ICEAnchor *GetICEAnchor();

bool ICEReplay::WasRecentlyUsed(ICETrack *pTrack) {

    for (int i = 0; i < 3; i++) {

        if (reinterpret_cast<int>(pTrack) == ICEReplay::RecentlyUsedTracks[i]) {

            return true;
        }
    }
    return false;
}

void ICEReplay::ClearRecentlyUsed() {

    ICEReplay::nRecentlyUsedIndex = 0;

    for (int i = 0; i < 3; i++) {

        ICEReplay::RecentlyUsedTracks[i] = 0;
    }
    return;
}

ICEAnchor *GetICEAnchor() {
    eView *view = eGetView(1, false);
    if (view != nullptr) {
        CameraMover *m = view->GetCameraMover();
        if (m != nullptr && m->GetType() == CM_ICE) {
            return static_cast<ICEMover *>(m)->GetICEAnchor();
        }
        return nullptr;
    }
    return nullptr;
}

float GetGroundElevation(const ICE::Vector3 *p) {
    float ground_elevation = 0.0f;
    if (IsGameFlowInGame()) {

        ICE::Vector3 unswizzled_position;
        eUnSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(p), *reinterpret_cast<bVector3 *>(&unswizzled_position));
        unswizzled_position.y += 4.0f;
        bool point_valid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(
            *reinterpret_cast<UMath::Vector3 *>(&unswizzled_position), ground_elevation, nullptr);
        if (!point_valid) {

            ground_elevation = p->z;
        }
    }

    return ground_elevation;
}

float ICEManager::GetAnimElevationFixup(ICE::Vector3 *p) {

    float elevation = GetGroundElevation(p);

    if (elevation > 0.0f) {

        return elevation - fAnimElevation;
    }

    return 0.0f;
}

void ICEManager::FixAnimElevation(ICE::Vector3 *p) {

    {
        ICEScene *scene = ICE::FindAnimScene();
        if (scene) {

            if (scene->IsCameraFixingElevation()) {

                ICE::Vector3 world_position;
                ICE::MulMatrix(&world_position, reinterpret_cast<const ICE::Matrix4 *>(&scene->GetSceneTransformMatrix()), p);
                if (IsEditorOff()) {

                    const ICE::Vector3 *scene_origin = reinterpret_cast<const ICE::Vector3 *>(&scene->GetSceneTransformMatrix().v3);
                    fAnimElevation = GetGroundElevation(scene_origin);
                }
                p->z += GetAnimElevationFixup(&world_position);
            }
        }
    }
}

ICEManager::ICEManager() {

    this->nState = 0;
    this->nTrack = 0;
    this->nHandle = 0;
    this->nOption = 0;
    this->nSetting = 0;
    this->nSceneHash = 0;
    this->nExitConfirmOption = 0;
    this->nDeleteConfirmOption = 0;
    this->nCopyMode = 0;
    this->nNisCameras = 0;
    this->nFmvCameras = 0;
    this->nReplayCameras = 0;
    this->nGenericCameras = 0;
    this->pNisCameras = nullptr;
    this->pFmvCameras = nullptr;
    this->pReplayCameras = nullptr;
    this->pGenericCameras = nullptr;
    this->pShakeGroup = nullptr;

    this->fAnimElevation = 0.0f;
    this->fParameterStart = 0.0f;
    this->fParameterLength = 0.0f;
    this->fParameterLengthBackup = 0.0f;

    this->nContext = 3;
    this->nPlayGenericGroupHash = bStringHash("");
    this->nPlayGenericTrackName[0] = '\0';
    this->pPlaybackTrack = nullptr;

    ICEReplay::ClearRecentlyUsed();
    this->bSmoothExit = false;
    this->nMarkerIndex = -1;
    this->bUseRealTime = false;
}

float ICEManager::GetTimerSeconds() {
    if (bUseRealTime) {
        return RealTimer.GetSeconds();
    }

    return WorldTimer.GetSeconds();
}

bool ICEManager::RefreshCameraSplines() {
    return false;
}

void ICEGetPlayerCarTransform(ICE::Matrix4 *m) {

    ICE::Identity(m);
    IPlayer *iplayer = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::First(PLAYER_LOCAL);
    if (iplayer) {

        IRigidBody *player_rigid_body = iplayer->GetSimable()->GetRigidBody();
        if (player_rigid_body) {

            UMath::Matrix4 mat;
            player_rigid_body->GetMatrix4(mat);
            eSwizzleWorldMatrix(*reinterpret_cast<const bMatrix4 *>(&mat), *reinterpret_cast<bMatrix4 *>(m));
            eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&player_rigid_body->GetPosition()),
                                *reinterpret_cast<bVector3 *>(&m->v3));
        }
    }
}

int ICEManager::ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, const ICE::Matrix4 *matrix) {

    ICE::Matrix4 mCarToWorld;

    ICEGetPlayerCarTransform(&mCarToWorld);

    int bestTrack = 0;

    for (int i = 0; i < nNisCameras; i++) {

        if (scene_hash != pNisCameras[i].GetHandle()) {

            continue;
        }

        ICEGroup *group = &pNisCameras[i];

        int numTracks = group->GetNumTracks();

        if (numTracks <= 1) {

            break;
        }

        float bestDot = -1.0f;

        for (int k = 0; k < numTracks; k++) {

            ICETrack *track = group->GetTrack(k);

            if (track->GetNumKeys() <= 0) {

                continue;
            }

            ICEData *key = track->GetKey(0);

            int n = 0;

            if (key->bSmooth != 0) {

                n = 1;
            }

            ICE::Vector3 v_eye;
            key[n].GetEye(n, &v_eye);

            switch (key[n].nSpaceEye) {
            case 0:
                ICE::MulVector(&v_eye, &mCarToWorld, &v_eye);
                break;
            case 2:
                ICE::Add(&v_eye, &v_eye, reinterpret_cast<const ICE::Vector3 *>(&mCarToWorld.v3));
                break;
            case 3:
                ICE::MulVector(&v_eye, matrix, &v_eye);
                break;
            default:
                break;
            }

            ICE::Vector3 v_look;
            key[n].GetLook(n, &v_look);

            switch (key[n].nSpaceLook) {
            case 0:
                ICE::MulVector(&v_look, &mCarToWorld, &v_look);
                break;
            case 2:
                ICE::Add(&v_look, &v_look, reinterpret_cast<const ICE::Vector3 *>(&mCarToWorld.v3));
                break;
            case 3:
                ICE::MulVector(&v_look, matrix, &v_look);
                break;
            default:
                break;
            }

            ICE::Vector3 vCamDir;
            ICE::Sub(&vCamDir, &v_eye, &v_look);

            vCamDir.z = 0.0f;

            ICE::Normalize(&vCamDir, &vCamDir);

            ICE::Vector3 *pCarDir = reinterpret_cast<ICE::Vector3 *>(&mCarToWorld.v0);

            float dot = ICE::Dot(&vCamDir, pCarDir);

            if (dot > bestDot) {

                bestDot = dot;
                bestTrack = k;
            }
        }
    }

    return bestTrack;
}

void ICEManager::SetGenericCameraToPlay(const char *group_name, const char *track_name) {

    nPlayGenericGroupHash = bStringHash(group_name);
    bStrNCpy(nPlayGenericTrackName, track_name, 13);

    nPlayGenericTrackName[13] = 0;
}

ICEGroup *ICEManager::GetNisCameraGroup(unsigned int scene_hash) {

    for (int i = 0; i < nNisCameras; i++) {

        if (scene_hash == pNisCameras[i].GetHandle()) {

            return &pNisCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetFmvCameraGroup(unsigned int scene_hash) {

    for (int i = 0; i < nFmvCameras; i++) {

        if (scene_hash == pFmvCameras[i].GetHandle()) {

            return &pFmvCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetReplayCameraGroup(unsigned int category_hash) {

    for (int i = 0; i < nReplayCameras; i++) {

        if (category_hash == pReplayCameras[i].GetHandle()) {

            return &pReplayCameras[i];
        }
    }

    return 0;
}

ICEGroup *ICEManager::GetGenericCameraGroup(unsigned int name_hash) {

    for (int i = 0; i < nGenericCameras; i++) {

        if (name_hash == pGenericCameras[i].GetHandle()) {

            return &pGenericCameras[i];
        }
    }

    return 0;
}

ICEShakeTrack *ICEManager::GetShakeTrack(unsigned int handle) {

    ICEShakeGroup *group;

    if (handle == 0) {

        return 0;
    }

    group = pShakeGroup;

    if (group != 0) {

        int num_tracks = group->GetNumTracks();

        for (int i = 0; i < num_tracks; i++) {

            ICEShakeTrack *track = group->GetTrack(i);

            if (track != 0) {

                if (bStringHash(track->GetName()) == handle) {

                    return track;
                }
            }
        }
    }

    return 0;
}

void ICEManager::Init() {
    pNisCameras = new ("NisCameras", 0) ICEGroup[256];
    pFmvCameras = new ("FmvCameras", 0) ICEGroup[10];
    pReplayCameras = new ("ReplayCameras", 0) ICEGroup[50];
    pGenericCameras = new ("GenericCameras", 0) ICEGroup[50];
    pShakeGroup = new ("ShakeGroup", 0) ICEShakeGroup;
}

bool bUseOldDutch = true;

static const char *GenericCategoryNames[2] = {
    "Cinematics",
    "Debug",
};

void ICEManager::Resolve() {

    {
        int num_scenes = ICE::GetSceneCount();

        for (unsigned int scene = 0; scene < num_scenes; scene++) {

            unsigned int scene_hash = ICE::GetSceneHash(scene);

            char scene_name[16];
            ICE::GetNameOfSceneHash(scene_hash, scene_name);

            if (ICE::StrNICmp(scene_name, "FMV", 3) == 0) {
                continue;
            }
            if (ICE::StrNICmp(scene_name, "replay", 6) == 0) {
                continue;
            }
            if (ICE::StrNICmp(scene_name, "clip", 4) == 0) {
                continue;
            }
            if (GetNisCameraGroup(scene_hash) != 0) {
                continue;
            }
            if (nNisCameras > 255) {
                continue;
            }
            pNisCameras[nNisCameras].SetContext(ICE_CONTEXT_NIS);

            pNisCameras[nNisCameras].SetHandle(scene_hash);

            nNisCameras++;
        }

        for (unsigned int fmv = 0; fmv < num_scenes; fmv++) {

            unsigned int scene_hash = ICE::GetSceneHash(fmv);

            char scene_name[16];
            ICE::GetNameOfSceneHash(scene_hash, scene_name);

            if (ICE::StrNICmp(scene_name, "FMV", 3) != 0) {
                continue;
            }
            if (GetFmvCameraGroup(scene_hash) != 0) {
                continue;
            }
            if (nFmvCameras > 9) {
                continue;
            }
            pFmvCameras[nFmvCameras].SetContext(ICE_CONTEXT_FMV);

            pFmvCameras[nFmvCameras].SetHandle(scene_hash);

            nFmvCameras++;
        }
    }

    {
        int num_categories = ICE::GetReplayCategoryNumElements();

        for (int category = 0; category < num_categories; category++) {

            unsigned int category_hash = ICE::GetReplayCategoryHash(category);

            if (GetReplayCameraGroup(category_hash) != 0) {
                continue;
            }
            if (nReplayCameras > 49) {
                continue;
            }
            pReplayCameras[nReplayCameras].SetContext(ICE_CONTEXT_REPLAY);

            pReplayCameras[nReplayCameras].SetHandle(category_hash);

            nReplayCameras++;
        }

        for (int name = 0; name < 2; name++) {

            unsigned int name_hash = bStringHash(GenericCategoryNames[name]);

            if (GetGenericCameraGroup(name_hash) != 0) {
                continue;
            }
            if (nGenericCameras > 49) {
                continue;
            }
            pGenericCameras[nGenericCameras].SetContext(ICE_CONTEXT_GENERIC);

            pGenericCameras[nGenericCameras].SetHandle(name_hash);

            nGenericCameras++;
        }
    }
}

int ICEManager::GetCameraIndex(float f_parameter, ICETrack *track) {

    int i;

    if (track) {

        i = track->GetKeyNumber(f_parameter);
    } else {

        i = 0;
    }

    return i;
}

float ICEManager::GetParameter() {

    float f_param = 0.0f;

    ICEScene *scene = ICE::FindAnimScene();

    if (scene) {

        if (fParameterLength > 0.0f) {

            f_param = (scene->GetTimeElapsed() - fParameterStart) / fParameterLength;
        }
    }

    return f_param;
}

float ICEManager::GetParameter(int n, ICETrack *pTrack) {

    if (pTrack == 0) {

        return 0.0f;
    }

    return pTrack->GetKeyParameter(n);
}

float ICEManager::GetIntervalSize(ICEData *pKey, ICETrack *pTrack) {

    int n = 0;

    if (pTrack != 0) {

        n = pTrack->GetKeyIndex(pKey);
    }

    return GetParameter(n + 1, pTrack) - GetParameter(n, pTrack);
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

void ICEManager::ChooseReplayCamera() {

    float f_param = 1.0f;

    if (pPlaybackTrack) {

        f_param = (GetTimerSeconds() - pPlaybackTrack->GetStart()) / pPlaybackTrack->GetLength();
    }

    if (f_param >= 1.0f) {

        ICETrack *track = ICEReplay::ChooseGoodCamera(GetICEAnchor(), pReplayCameras, nReplayCameras);

        if (track) {

            pPlaybackTrack = track;

            pPlaybackTrack->SetStart(GetTimerSeconds());
        }
    }
}

int ICEManager::ChooseCameraPlaybackTrack() {

    pPlaybackTrack = 0;

    bUseOldDutch = false;

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

            bUseOldDutch = true;
        }

    } else {

        pPlaybackTrack = ChooseGenericCamera();

        if (pPlaybackTrack == 0) {

            pPlaybackTrack = ICEReplay::ChooseGoodCamera(GetICEAnchor(), pReplayCameras, nReplayCameras);

            if (pPlaybackTrack != 0) {

                pPlaybackTrack->SetStart(GetTimerSeconds());
            }
        }
    }

    return pPlaybackTrack != 0;
}

ICEData *ICEManager::GetCameraData(uint32 scene_hash, int camTrack) {

    ICEGroup *group = GetNisCameraGroup(scene_hash);

    if (group != 0) {

        char name[12];

        bSPrintf(name, "Track %d", camTrack);
        pPlaybackTrack = group->GetTrack(name);

        if (pPlaybackTrack != 0) {

            return pPlaybackTrack->GetCameraData(0, 0, 0);
        }
    }

    return 0;
}

ICEData *ICEManager::GetCameraData(ICETrack **ppTrack, float *pParameter0, float *pParameter1) {

    if (ppTrack != 0) {

        *ppTrack = pPlaybackTrack;
    }

    if (pPlaybackTrack != 0) {

        return pPlaybackTrack->GetCameraData(pParameter0, pParameter1, 0);
    }

    return 0;
}

ICEData *ICEManager::GetNeighbour(ICEData *pKey, int dir, ICETrack *pTrack) {

    if (pTrack != 0) {

        int n = pTrack->GetKeyIndex(pKey);

        return pTrack->GetKey((dir != 0) ? n + 1 : n - 1);
    }

    return 0;
}

void ICEManager::GetSlope(ICE::Vector3 *pEye, ICE::Vector3 *pLook, float *pDutch, float *pLens, ICEData *pKey, int n,
                          ICETrack *pTrack) {

    ICE::Vector3 v_eye_slope(0.0f, 0.0f, 0.0f);
    ICE::Vector3 v_look_slope(0.0f, 0.0f, 0.0f);
    float f_lens_slope = 0.0f;
    float f_dutch_slope = 0.0f;

    if (pKey->nType != 0) {

        bool shared_slope = 0;
        ICEData *p_neighbour = GetNeighbour(pKey, n, pTrack);

        if (p_neighbour != 0 && p_neighbour->nType != 0) {

            if (ICE::KeysShared(pKey, n, p_neighbour, n ^ 1)) {

                shared_slope = 1;

                ICE::Vector3 v0;
                ICE::Vector3 v1;
                ICE::Vector3 v_eye0;
                ICE::Vector3 v_eye1;
                ICE::Vector3 v_look0;
                ICE::Vector3 v_look1;

                p_neighbour->GetEye(0, &v0);
                p_neighbour->GetEye(1, &v1);
                ICE::Sub(&v_eye0, &v1, &v0);

                p_neighbour->GetLook(0, &v0);
                p_neighbour->GetLook(1, &v1);
                ICE::Sub(&v_look0, &v1, &v0);

                float f_dutch0 = p_neighbour->fDutch[1] - p_neighbour->fDutch[0];
                float f_lens0 = p_neighbour->fLens[1] - p_neighbour->fLens[0];

                pKey->GetEye(0, &v0);
                pKey->GetEye(1, &v1);
                ICE::Sub(&v_eye1, &v1, &v0);

                pKey->GetLook(0, &v0);
                pKey->GetLook(1, &v1);
                ICE::Sub(&v_look1, &v1, &v0);

                float f_dutch1 = pKey->fDutch[1] - pKey->fDutch[0];
                float f_lens1 = pKey->fLens[1] - pKey->fLens[0];

                float f_camera_size = GetIntervalSize(pKey, pTrack);
                float f_neighbour_size = GetIntervalSize(p_neighbour, pTrack);
                float f_neighbour_blend = f_camera_size / (f_camera_size + f_neighbour_size);
                float f_camera_blend = 1.0f - f_neighbour_blend;

                if (f_neighbour_size > 0.000001f) {

                    f_neighbour_blend *= f_camera_size / f_neighbour_size;
                }

                float tangent_length = pKey->fTangentLength[n];
                f_neighbour_blend *= tangent_length;
                f_camera_blend *= tangent_length;

                ICE::Scale(&v_eye_slope, &v_eye0, f_neighbour_blend);
                ICE::ScaleAdd(&v_eye_slope, &v_eye_slope, &v_eye1, f_camera_blend);

                ICE::Scale(&v_look_slope, &v_look0, f_neighbour_blend);
                ICE::ScaleAdd(&v_look_slope, &v_look_slope, &v_look1, f_camera_blend);

                f_dutch_slope = f_dutch0 * f_neighbour_blend + f_dutch1 * f_camera_blend;
                f_lens_slope = f_lens0 * f_neighbour_blend + f_lens1 * f_camera_blend;
            }
        }

        if (shared_slope == 0) {

            ICE::Vector3 v_eye0;
            ICE::Vector3 v_eye1;
            ICE::Vector3 v_look0;
            ICE::Vector3 v_look1;

            pKey->GetEye(0, &v_eye0);
            pKey->GetEye(1, &v_eye1);
            pKey->GetLook(0, &v_look0);
            pKey->GetLook(1, &v_look1);

            ICE::Sub(&v_eye_slope, &v_eye1, &v_eye0);
            ICE::Sub(&v_look_slope, &v_look1, &v_look0);

            float tangent_length = pKey->fTangentLength[n];
            ICE::Scale(&v_eye_slope, &v_eye_slope, tangent_length);
            ICE::Scale(&v_look_slope, &v_look_slope, tangent_length);

            f_dutch_slope = tangent_length * (pKey->fDutch[1] - pKey->fDutch[0]);
            f_lens_slope = tangent_length * (pKey->fLens[1] - pKey->fLens[0]);
        }
    }

    *pEye = v_eye_slope;
    *pLook = v_look_slope;
    *pDutch = f_dutch_slope;
    *pLens = f_lens_slope;
}

void ICEManager::Update() {
}

int ICEManager::GetNumSceneCameraTrack(uint32 scene_hash) {

    int n = 0;
    ICEGroup *group = GetNisCameraGroup(scene_hash);

    if (group) {

        n = group->NumTracks;
    }

    return n;
}

ICEGroup *ICEManager::AddCameraGroup(ICEContext context, unsigned int handle) {

    ICEGroup *group = GetCameraGroup(context, handle);

    if (group == 0) {

        switch (context) {
        case ICE_CONTEXT_NIS:
            if (nNisCameras <= 255) {

                group = &pNisCameras[nNisCameras++];
                group->SetContext(context);
                group->SetHandle(handle);
            }
            break;

        case ICE_CONTEXT_FMV:
            if (nFmvCameras <= 9) {

                group = &pFmvCameras[nFmvCameras++];
                group->SetContext(context);
                group->SetHandle(handle);
            }
            break;

        case ICE_CONTEXT_REPLAY:
            if (nReplayCameras <= 49) {

                group = &pReplayCameras[nReplayCameras++];
                group->SetContext(context);
                group->SetHandle(handle);
            }
            break;

        case ICE_CONTEXT_GENERIC:
            if (nGenericCameras <= 49) {

                group = &pGenericCameras[nGenericCameras++];
                group->SetContext(context);
                group->SetHandle(handle);
            }
            break;

        default:
            break;
        }
    }

    return group;
}

ICEGroup *ICEManager::GetCameraGroup(ICEContext ctx, unsigned int handle) {
    int count = 0;
    ICEGroup *groups = 0;
    switch (ctx) {
    case ICE_CONTEXT_NIS:
        groups = pNisCameras;
        count = nNisCameras;
        break;
    case ICE_CONTEXT_FMV:
        groups = pFmvCameras;
        count = nFmvCameras;
        break;
    case ICE_CONTEXT_REPLAY:
        groups = pReplayCameras;
        count = nReplayCameras;
        break;
    case ICE_CONTEXT_GENERIC:
        groups = pGenericCameras;
        count = nGenericCameras;
        break;
    default:
        break;
    }
    for (int i = 0; i < count; i++) {
        ICEGroup *group = &groups[i];
        if (handle == group->Handle) {
            return group;
        }
    }
    return 0;
}

void ICEManager::LoadCameraSet(bChunk *chunk) {

    ICEContext context = static_cast<ICEContext>(4);
    bool warned_overflow = false;

    switch (chunk->GetID()) {
    case BCHUNK_ICE_NIS_CAMERAS:
        context = ICE_CONTEXT_NIS;
        break;
    case BCHUNK_ICE_FMV_CAMERAS:
        context = ICE_CONTEXT_FMV;
        break;
    case BCHUNK_ICE_MKR_CAMERAS:
        context = ICE_CONTEXT_REPLAY;
        break;
    case BCHUNK_ICE_REPLAY_CAMERAS:
        context = ICE_CONTEXT_GENERIC;
        break;
    default:
        break;
    }

    for (bChunk *child = chunk->GetFirstChunk(); child != chunk->GetLastChunk(); child = child->GetNext()) {

        unsigned int *p_handle = reinterpret_cast<unsigned int *>(child->GetData());

        bPlatEndianSwap(&p_handle[0]);
        bPlatEndianSwap(&p_handle[1]);

        ICEGroup *group = AddCameraGroup(context, p_handle[0]);

        if (group != 0) {

            int num_tracks = p_handle[1];
            ICETrack *track = reinterpret_cast<ICETrack *>(&p_handle[2]);

            for (int i = 0; i < num_tracks; i++) {

                track->PlatEndianSwap();

                group->AddTrack(track);

                track = reinterpret_cast<ICETrack *>(reinterpret_cast<char *>(track) + track->MemoryImageSize());
            }
        }
    }
}

void ICEManager::UnloadCameraSet(bChunk *set_chunk) {
    ICEContext context = static_cast<ICEContext>(4);
    unsigned int id = set_chunk->GetID();
    switch (id) {
    case BCHUNK_ICE_NIS_CAMERAS:
        context = ICE_CONTEXT_NIS;
        break;
    case BCHUNK_ICE_FMV_CAMERAS:
        context = ICE_CONTEXT_FMV;
        break;
    case BCHUNK_ICE_MKR_CAMERAS:
        context = ICE_CONTEXT_REPLAY;
        break;
    case BCHUNK_ICE_REPLAY_CAMERAS:
        context = ICE_CONTEXT_GENERIC;
        break;
    default:
        break;
    }

    for (bChunk *chunk = set_chunk->GetFirstChunk(); chunk != set_chunk->GetLastChunk(); chunk = chunk->GetNext()) {

        unsigned int *p_handle = reinterpret_cast<unsigned int *>(chunk->GetData());

        ICEGroup *group = GetCameraGroup(context, *p_handle);

        if (group != 0) {

            group->FlushTracks();
        }
    }

    int num_groups = 0;
    ICEGroup *groups = 0;

    switch (context) {
    case ICE_CONTEXT_NIS:
        groups = pNisCameras;
        num_groups = nNisCameras;
        break;
    case ICE_CONTEXT_FMV:
        groups = pFmvCameras;
        num_groups = nFmvCameras;
        break;
    case ICE_CONTEXT_REPLAY:
        groups = pReplayCameras;
        num_groups = nReplayCameras;
        break;
    case ICE_CONTEXT_GENERIC:
        groups = pGenericCameras;
        num_groups = nGenericCameras;
        break;
    default:
        break;
    }

    for (int i = 0; i < num_groups; i++) {

        groups[i].FlushAllocatedTracks();
    }
}

void ICEManager::LoadCameraShakes(bChunk *set_chunk) {
    unsigned int id = set_chunk->GetID();
    bool warned_overflow;
    bChunk *chunk;
    {
        unsigned int *p_handle = reinterpret_cast<unsigned int *>(set_chunk->GetData());
        bPlatEndianSwap(p_handle);
        ICEShakeGroup *group = pShakeGroup;
        if (group) {
            warned_overflow = false;
            int num_tracks = *p_handle;
            ICEShakeTrack *track = reinterpret_cast<ICEShakeTrack *>(p_handle + 1);
            for (int i = 0; i < num_tracks; i++) {
                track->PlatEndianSwap();
                group->AddTrack(track);
                track = reinterpret_cast<ICEShakeTrack *>(reinterpret_cast<char *>(track) + track->MemoryImageSize());
            }
        }
    }
}

void ICEManager::UnloadCameraShakes(bChunk *chunk) {

    unsigned int id = chunk->GetID();

    ICEShakeGroup *group = pShakeGroup;

    group->FlushTracks();

    group->FlushAllocatedTracks();
}

int LoaderICECameras(bChunk *chunk) {

    switch (chunk->GetID()) {

    case BCHUNK_ICE_NIS_CAMERAS:
    case BCHUNK_ICE_FMV_CAMERAS:
    case BCHUNK_ICE_MKR_CAMERAS:
    case BCHUNK_ICE_REPLAY_CAMERAS:
        TheICEManager.LoadCameraSet(chunk);
        return 1;

    case BCHUNK_ICE_CAMERA_SHAKE_GROUP:
        TheICEManager.LoadCameraShakes(chunk);
        return 1;

    default:
        break;
    }

    return 0;
}

int UnloaderICECameras(bChunk *chunk) {

    switch (chunk->GetID()) {

    case BCHUNK_ICE_NIS_CAMERAS:
    case BCHUNK_ICE_FMV_CAMERAS:
    case BCHUNK_ICE_MKR_CAMERAS:
    case BCHUNK_ICE_REPLAY_CAMERAS:
        TheICEManager.UnloadCameraSet(chunk);
        return 1;

    case BCHUNK_ICE_CAMERA_SHAKE_GROUP:
        TheICEManager.UnloadCameraShakes(chunk);
        return 1;

    default:
        break;
    }

    return 0;
}

void ICECompleteEventTags() {
    ICETrack *p_track;
    float fParameter0;
    float fParameter1;

    ICEData *pCameraData = TheICEManager.GetCameraData(&p_track, &fParameter0, &fParameter1);

    int key = TheICEManager.GetCameraIndex((fParameter0 + fParameter1) * 0.5f, p_track) + 1;

    if (p_track != nullptr) {
        int keyCount = p_track->GetNumKeys();

        while (key < keyCount) {

            ICE::FireEventTag(key);
            key++;
        }
    }
}

// TODO move this?
struct ICEManager TheICEManager; // size: 0x80
