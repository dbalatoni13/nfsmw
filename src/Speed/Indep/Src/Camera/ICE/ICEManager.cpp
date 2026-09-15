#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"

// move this
ICEScene *FindAnimScene() {
    INIS *nis = INIS::Get();

    if (nis != nullptr)
        return nis->GetScene();

    return nullptr;
}

float GetGroundElevation(const UMath::Vector3 *position) {
    float ground_elevation = 0.0f; // r1+0x20
    if (IsGameFlowInGame()) {
        UMath::Vector3 unswizzled_position = UMath::Vector3();

        eUnSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(position), reinterpret_cast<bVector3 &>(unswizzled_position));

        unswizzled_position.y += 4.0f;

        bool point_valid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(unswizzled_position, ground_elevation, nullptr);

        if (!point_valid) {
            ground_elevation = position->z;
        }
    }
    return ground_elevation;
}

float ICEManager::GetAnimElevationFixup(ICE::Vector3 *position) {
    float elevation = GetGroundElevation(reinterpret_cast<UMath::Vector3 *>(position));

    if (elevation > 0.0f) {
        return elevation - this->fAnimElevation;
    }

    return 0.0f;
}

// float ICEManager::GetAnimElevationFixup(ICE::Vector3 *position) {
//     float elevation = GetGroundElevation(reinterpret_cast<UMath::Vector3 *>(position));

//     if (elevation > 0.0f) {
//         return elevation - this->fAnimElevation;
//     }

//     return 0.0f;
// }

void ICEManager::FixAnimElevation(ICE::Vector3 *position) {

    if (ICEScene *scene = FindAnimScene()) {
        if (scene->IsCameraFixingElevation() != 0) {
            UMath::Vector3 world_position;

            world_position.x = 0.0f;
            world_position.y = 0.0f;
            world_position.z = 0.0f;
            // world_position.pad = 0.0f; // must be 0x14 but pad doesnt contains in UMath::Vector3

            ICE::MulMatrix(reinterpret_cast<UMath::Vector3 *>(&world_position),
                           reinterpret_cast<const UMath::Matrix4 *>(&scene->GetSceneTransformMatrix()),
                           reinterpret_cast<const UMath::Vector3 *>(position));

            if (IsEditorOff()) {
                const UMath::Vector3 *scene_origin = reinterpret_cast<const UMath::Vector3 *>(&scene->GetSceneTransformMatrix().v3);
                this->fAnimElevation = GetGroundElevation(scene_origin);
            }
            position->z = position->z + GetAnimElevationFixup(reinterpret_cast<ICE::Vector3 *>(&world_position));
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

    this->fParameterLengthBackup = 0.0f;
    this->fAnimElevation = 0.0f;
    this->fParameterStart = 0.0f;
    this->fParameterLength = 0.0f;

    this->nContext = 3;
    this->nPlayGenericGroupHash = bStringHash("");
    this->nPlayGenericTrackName[0] = '\0';
    this->pPlaybackTrack = nullptr;

    ICEReplay::ClearRecentlyUsed();
    this->bUseRealTime = false;
    this->nMarkerIndex = -1;
    this->bSmoothExit = false;
}

bool ICEManager::RefreshCameraSplines() {
    // Local variables
    bool b_refresh = false;
    return b_refresh;
}

// Range: 0x8007D69C -> 0x8007D76C
static void ICEGetPlayerCarTransform(ICE::Matrix4 *mCarToWorld /* r30 */) {

    ICE::Identity(mCarToWorld);

    IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);

    if (iplayer != nullptr) {
        IRigidBody *player_rigid_body = iplayer->GetSimable()->GetRigidBody();
        if (player_rigid_body != nullptr) {

            UMath::Matrix4 mat;

            player_rigid_body->GetMatrix4(mat);

            eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(mat), *reinterpret_cast<bMatrix4 *>(mCarToWorld));
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(player_rigid_body->GetPosition()), reinterpret_cast<bVector3 &>(mCarToWorld->v3));
        }
    }
}

int ICEManager::ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, const ICE::Matrix4 *scene_origin) {
    struct UMath::Matrix4 mCarToWorld;

    int bestTrack = 0;

    mCarToWorld.v0.x = 0.0f;
    mCarToWorld.v1.x = 0.0f;
    mCarToWorld.v2.x = 0.0f;
    mCarToWorld.v3.x = 0.0f;

    mCarToWorld.v0.y = 0.0f;
    mCarToWorld.v1.y = 0.0f;
    mCarToWorld.v2.y = 0.0f;
    mCarToWorld.v3.y = 0.0f;

    mCarToWorld.v0.z = 0.0f;
    mCarToWorld.v1.z = 0.0f;
    mCarToWorld.v2.z = 0.0f;
    mCarToWorld.v3.z = 0.0f;

    mCarToWorld.v0.w = 0.0f;
    mCarToWorld.v1.w = 0.0f;
    mCarToWorld.v2.w = 0.0f;
    mCarToWorld.v3.w = 0.0f;

    ICEGetPlayerCarTransform(reinterpret_cast<ICE::Matrix4 *>(&mCarToWorld));

    for (int i = 0; i < nNisCameras; ++i) {
        ICEGroup *group = nullptr;
        if (scene_hash == this->pNisCameras[i].GetHandle()) {
            group = &this->pNisCameras[i];
        }

        int numTracks = group->GetNumTracks();
        if (numTracks < 2) {
            return bestTrack;
        }
        float bestDot = -1.0f;

        for (int k = 0; k < numTracks; k++) {

            ICETrack *track = group->GetTrack(k);

            if (track->GetNumKeys() > 0) {

                struct ICEData *key = track->GetKey(0);
                int n = key->bSmooth ? 1 : 0;

                struct UMath::Vector3 v_eye;
                key->GetEye(n, reinterpret_cast<ICE::Vector3 *>(&v_eye));

                switch (key->nSpaceEye) {
                    case 0:
                        ICE::MulVector(reinterpret_cast<ICE::Vector3 *>(&v_eye), reinterpret_cast<const ICE::Matrix4 *>(&mCarToWorld),
                                       reinterpret_cast<const ICE::Vector3 *>(&v_eye));
                        break;

                    case 2:
                        ICE::Add(&v_eye, &v_eye, reinterpret_cast<const UMath::Vector3 *>(&mCarToWorld.v3));
                        break;

                    case 3:
                        ICE::MulVector(reinterpret_cast<ICE::Vector3 *>(&v_eye), scene_origin, reinterpret_cast<const ICE::Vector3 *>(&v_eye));
                        break;
                }
                struct UMath::Vector3 v_look;
                key->GetLook(n, reinterpret_cast<ICE::Vector3 *>(&v_look));
                switch (key->nSpaceLook) {
                    case 0:

                        ICE::MulVector(reinterpret_cast<ICE::Vector3 *>(&v_look), reinterpret_cast<const ICE::Matrix4 *>(&mCarToWorld),

                                       reinterpret_cast<const ICE::Vector3 *>(&v_look));
                        break;

                    case 2:
                        ICE::Add(&v_look, &v_look, reinterpret_cast<const UMath::Vector3 *>(&mCarToWorld.v3));
                        break;

                    case 3:
                        ICE::MulVector(reinterpret_cast<ICE::Vector3 *>(&v_look), scene_origin, reinterpret_cast<const ICE::Vector3 *>(&v_look));
                        break;
                }

                UMath::Vector3 vCamDir;

                ICE::Sub(&vCamDir, &v_eye, &v_look);
                // vCamDir.pad = 0.0f;
                vCamDir.z = 0.0f;
                ICE::Normalize(&vCamDir, &vCamDir);

                float dot = ICE::Dot(&vCamDir, reinterpret_cast<const UMath::Vector3 *>(&mCarToWorld.v0));

                if (dot > bestDot) {
                    bestDot = dot;
                    bestTrack = k;
                }
            }
        }
    }

    return bestTrack;
}

void ICEManager::SetGenericCameraToPlay(char const *group_name, char const *track_name)

{
    this->nPlayGenericGroupHash = bStringHash(group_name);
    bStrNCpy(this->nPlayGenericTrackName, track_name, 0xd);
    this->nPlayGenericTrackName[0xd] = '\0';
    return;
}

ICEGroup *ICEManager::GetNisCameraGroup(uint32 scene_hash) {
    for (int i = 0; i < nNisCameras; ++i) {
        if (scene_hash == pNisCameras[i].GetHandle())
            return &pNisCameras[i];
    }

    return nullptr;
}
int ICEManager::GetNumSceneCameraTrack(uint32 scene_hash) {
    ICEGroup *group = this->GetNisCameraGroup(scene_hash);

    if (group != nullptr)
        return group->GetNumTracks();

    return 0;
}

int ICEManager::GetCameraIndex(float f_param, struct ICETrack *track) {
    if (track)
        return track->GetKeyNumber(f_param);
    return 0;
}

void ICEManager::Update() {
    return;
}

// TODO move this?
struct ICEManager TheICEManager; // size: 0x80
