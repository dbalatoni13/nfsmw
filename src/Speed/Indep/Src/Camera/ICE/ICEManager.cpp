#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
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

float ICEManager::GetTimerSeconds() {
    return bUseRealTime ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
}

bool ICEManager::RefreshCameraSplines() {
    // Local variables
    bool b_refresh = false;
    return b_refresh;
}

int ICEManager::ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, const ICE::Matrix4 *scene_origin) {
    // Local variables
    // struct UMath::Matrix4 mCarToWorld; // r1+0x8
    struct UMath::Matrix4 mCarToWorld; // r1+0x8
    int bestTrack;                     // r19
    // int bestTrack = (pNisCameras) | (track->Allocated << 16) | (track->Name[0] << 24);

    // /* anonymous block */ {
    //     // Range: 0x8007D784 -> 0x8007DA6C
    //     int i; // r21
    for (int i = 0; i < nNisCameras; i++) {

        //     // Range: 0x8007D804 -> 0x8007D804
        //     inline unsigned int ICEGroup::GetHandle() {}
        //     /* anonymous block */ {
        if (pNisCameras[i].GetHandle() == scene_hash) {

            //         // Range: 0x8007D81C -> 0x8007DA5C
            //         struct ICEGroup *group;
            //         int numTracks; // r25
            //         float bestDot; // f30

            struct ICEGroup *group = this->GetNisCameraGroup(scene_hash);
            //         // Range: 0x8007D81C -> 0x8007D81C
            //         inline int ICEGroup::GetNumTracks() {}
            int numTracks = group->GetNumTracks(); // r25
            float bestDot;                         // f30 // mb 0.0f or -1.0f

            //         /* anonymous block */ {
            //             // Range: 0x8007D828 -> 0x8007DA5C
            //             int k; // r26
            for (int k = 0; k < numTracks; k++) {

                //             /* anonymous block */ {
                //                 // Range: 0x8007D83C -> 0x8007DA50
                //                 struct ICETrack *track; // r3

                for (ICETrack *track = group->GetTrack(k); track != group->GetTrack(k); track = track->GetNext()) {

                    //                 // Range: 0x8007D83C -> 0x8007D83C
                    //                 inline int ICETrack::GetNumKeys() {}

                    if (track->GetNumKeys() < 2)
                        return bestTrack;
                    if (track->GetNumKeys() > 0) {
                        //                 /* anonymous block */ {
                        //                     // Range: 0x8007D854 -> 0x8007DA50
                        //                     struct ICEData *key;
                        //                     int n;                         // r29
                        //                     struct UMath::Vector3 v_eye;   // r1+0x48
                        //                     struct UMath::Vector3 v_look;  // r1+0x58
                        //                     struct UMath::Vector3 vCamDir; // r1+0x68
                        //                     struct UMath::Vector3 *pCarDir;
                        //                     float dot;

                        //                     // Range: 0x8007D854 -> 0x8007D878
                        //                     inline struct ICEData *ICETrack::GetKey(int n) {
                        //                         // Range: 0x8007D854 -> 0x8007D868
                        //                         inline int UMath::Clamp(int a, int min, int max) {
                        //                             // Range: 0x8007D854 -> 0x8007D868
                        //                             inline int bClamp(int a, int MINIMUM, int MAXIMUM) {
                        //                                 // Range: 0x8007D854 -> 0x8007D854
                        //                                 inline int bMax(int a, int b) {}

                        //                                 // Range: 0x8007D854 -> 0x8007D868
                        //                                 inline int bMin(int a, int b) {}
                        //                             }
                        //                         }
                        //                     }

                        //                     // Range: 0x8007D88C -> 0x8007D88C
                        //                     inline Vector3::Vector3() {}

                        //                     // Range: 0x8007D8E4 -> 0x8007D8E4
                        //                     inline void MulVector(struct UMath::Vector3 * dst, const struct UMath::Matrix4 *m, const struct
                        //                     UMath::Vector3 *v)
                        //                     {}

                        //                     // Range: 0x8007D8F8 -> 0x8007D8F8
                        //                     inline void UMath::Add(struct UMath::Vector3 * dst, const struct UMath::Vector3 *v1, const struct
                        //                     UMath::Vector3 *v2) {
                        //                         // Range: 0x8007D8F8 -> 0x8007D8F8
                        //                         inline struct bVector3 *bAdd(struct bVector3 * dest, const struct bVector3 *v1, const struct
                        //                         bVector3 *v2)
                        //                         {
                        //                             // Local variables
                        //                             float x1; // f0
                        //                             float y1; // f13
                        //                             float z1; // f12
                        //                             float x2; // f11
                        //                             float y2; // f10
                        //                             float z2; // f9

                        //                             // Range: 0x8007D8F8 -> 0x8007D8F8
                        //                             inline struct bVector3 *bFill(struct bVector3 * dest, float x, float y, float z) {}
                        //                         }
                        //                     }

                        //                     // Range: 0x8007D92C -> 0x8007D93C
                        //                     inline void MulVector(struct UMath::Vector3 * dst, const struct UMath::Matrix4 *m, const struct
                        //                     UMath::Vector3 *v)
                        //                     {}

                        //                     // Range: 0x8007D93C -> 0x8007D93C
                        //                     inline Vector3::Vector3() {}

                        //                     // Range: 0x8007D98C -> 0x8007D98C
                        //                     inline void MulVector(struct UMath::Vector3 * dst, const struct UMath::Matrix4 *m, const struct
                        //                     UMath::Vector3 *v)
                        //                     {}

                        //                     // Range: 0x8007D9A0 -> 0x8007D9A0
                        //                     inline void UMath::Add(struct UMath::Vector3 * dst, const struct UMath::Vector3 *v1, const struct
                        //                     UMath::Vector3 *v2) {
                        //                         // Range: 0x8007D9A0 -> 0x8007D9A0
                        //                         inline struct bVector3 *bAdd(struct bVector3 * dest, const struct bVector3 *v1, const struct
                        //                         bVector3 *v2)
                        //                         {
                        //                             // Local variables
                        //                             float x1; // f0
                        //                             float y1; // f13
                        //                             float z1; // f12
                        //                             float x2; // f11
                        //                             float y2; // f10
                        //                             float z2; // f9

                        //                             // Range: 0x8007D9A0 -> 0x8007D9A0
                        //                             inline struct bVector3 *bFill(struct bVector3 * dest, float x, float y, float z) {}
                        //                         }
                        //                     }

                        //                     // Range: 0x8007D9D4 -> 0x8007D9E4
                        //                     inline void MulVector(struct UMath::Vector3 * dst, const struct UMath::Matrix4 *m, const struct
                        //                     UMath::Vector3 *v)
                        //                     {}

                        //                     // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                     inline Vector3::Vector3() {}

                        //                     // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                     inline void UMath::Sub(struct UMath::Vector3 * dst, const struct UMath::Vector3 *v1, const struct
                        //                     UMath::Vector3 *v2) {
                        //                         // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                         inline struct bVector3 *bSub(struct bVector3 * dest, const struct bVector3 *v1, const struct
                        //                         bVector3 *v2)
                        //                         {
                        //                             // Local variables
                        //                             float x1; // f12
                        //                             float y1; // f13
                        //                             float z1;
                        //                             float x2; // f0
                        //                             float y2; // f11
                        //                             float z2;

                        //                             // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                             inline struct bVector3 *bFill(struct bVector3 * dest, float x, float y, float z) {}
                        //                         }
                        //                     }

                        //                     // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                     inline void Normalize(struct UMath::Vector3 * dst, const struct UMath::Vector3 *src) {}

                        //                     // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                     inline float UMath::Dot(const struct UMath::Vector3 *v1, const struct UMath::Vector3 *v2) {
                        //                         // Range: 0x8007D9E4 -> 0x8007D9E4
                        //                         inline float bDot(const struct bVector3 *v1, const struct bVector3 *v2) {}
                        //                     }
                        //                 }
                    }
                    //             }
                }
                //         }
            }
            //     }
        }
        // }
    }
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

void ICEManager::Update() {
    return;
}

// TODO move this?
struct ICEManager TheICEManager; // size: 0x80
