#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "ICEReplay.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"

float GetGroundElevation(Vector3 *position) {
    float ground_elevation = 0.0f; // r1+0x20
    if (IsGameFlowInGame()) {
        // Range: 0x8007D39C -> 0x8007D418
        struct UMath::Vector3 unswizzled_position; // r1+0x8
        bool point_valid;

        // Range: 0x8007D39C -> 0x8007D39C
        // cast?
        // inline Vector3::Vector3() {}

        // Range: 0x8007D39C -> 0x8007D39C
        eUnSwizzleWorldVector(*position, reinterpret_cast<bVector3 &>(unswizzled_position));

        point_valid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(unswizzled_position, ground_elevation, nullptr);

        // // Range: 0x8007D39C -> 0x8007D39C
        // inline WCollisionMgr::WCollisionMgr(unsigned int surfaceExclMask, unsigned int primitiveExclMask) {}

        // // Range: 0x8007D39C -> 0x8007D39C
        // inline WCollisionMgr::~WCollisionMgr() {}
    }
}

float ICEManager::GetAnimElevationFixup(ICE::Vector3 *v) {
    float f = 0.0f;
    if (this->fAnimElevation != 0.0f) {
        float elevation = this->fAnimElevation;
    }
    return f;
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

// TODO move this?
struct ICEManager TheICEManager; // size: 0x80
