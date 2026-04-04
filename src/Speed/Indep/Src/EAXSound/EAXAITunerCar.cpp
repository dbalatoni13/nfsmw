#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <new>
#include <types.h>

bool IsCarInRadius(EAX_CarState *pCar, const bVector3 *vPos, float fRadius);

namespace AIDriveBy {
enum eAIDRIVE_BY_TYPE {
    DRIVE_BY_UNKNOWN = 0,
    DRIVE_BY_TREE = 1,
    DRIVE_BY_LAMPPOST = 2,
    DRIVE_BY_SMOKABLE = 3,
    DRIVE_BY_TUNNEL_IN = 4,
    DRIVE_BY_TUNNEL_OUT = 5,
    DRIVE_BY_OVERPASS_IN = 6,
    DRIVE_BY_OVERPASS_OUT = 7,
    DRIVE_BY_AI_CAR = 8,
    DRIVE_BY_TRAFFIC = 9,
    DRIVE_BY_BRIDGE = 10,
    DRIVE_BY_PRE_COL = 11,
    DRIVE_BY_CAMERA_BY = 12,
    MAX_DRIVE_BY_TYPES = 13,
};

struct stAIDriveByInfo {
    eAIDRIVE_BY_TYPE eDriveByType;
    EAXCar *pEAXCar;
    float ClosingVelocity;
    bVector3 vLocation;
    unsigned int UniqueID;

    stAIDriveByInfo() {
        vLocation = bVector3(0.0f, 0.0f, 0.0f);
    }
};
} // namespace AIDriveBy

void emAddHandler(void (*function)(emEvent *), unsigned int stream_mask);
void emRemoveHandler(void (*function)(emEvent *));
bool g_EAXIsPaused(void);

CSTATE_Base::StateInfo EAXAITunerCar::s_StateInfo = {
    0x00030000,
    "EAXAITunerCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(void)>(&EAXAITunerCar::CreateState),
};

CSTATE_Base::StateInfo *EAXAITunerCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXAITunerCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXAITunerCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXAITunerCar), s_StateInfo.stateName, false)) EAXAITunerCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXAITunerCar), s_StateInfo.stateName, true)) EAXAITunerCar;
    }
}

EAXAITunerCar::EAXAITunerCar() {
    emAddHandler(ProcessEvent, 0x00040000);
    mPhysicsChangedGear = 0;
}

EAXAITunerCar::~EAXAITunerCar() {
    emRemoveHandler(ProcessEvent);
}

int EAXAITunerCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) {
    if (SFXMessageType == SFX_CHANGEGEAR) {
        mPhysicsChangedGear = 1;
        return 0;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void EAXAITunerCar::UpdateCarPhysics() {
    g_EAXIsPaused();
}

void EAXAITunerCar::UpdateParams(float t) {
    if (m_pCar != nullptr) {
        EAXCar::UpdateParams(t);
        mPhysicsChangedGear = 0;
    }
}

void EAXAITunerCar::ProcessEvent(emEvent *event) {
}

void EAXAITunerCar::UpdatAIDriveBy(float t) {
    EAX_CarState *ClosestPlayer;
    float fRelativeVel;
    bVector3 vVelDiff;

    (void)t;

    if (GetPhysCar() == nullptr) {
        return;
    }

    ClosestPlayer = GetClosestPlayerCar(GetPhysCar()->GetPosition());
    if (!IsCarInRadius(ClosestPlayer, GetPhysCar()->GetPosition(), 12.0f)) {
        return;
    }

    vVelDiff = bSub(*GetPhysCar()->GetVelocity(), *ClosestPlayer->GetVelocity());
    fRelativeVel = bLength(vVelDiff);
    if (fRelativeVel < 15.0f) {
        return;
    }

    AIDriveBy::stAIDriveByInfo TmpDriveByPackage;
    TmpDriveByPackage.eDriveByType = AIDriveBy::DRIVE_BY_AI_CAR;
    TmpDriveByPackage.ClosingVelocity = fRelativeVel;
    TmpDriveByPackage.pEAXCar = this;
    TmpDriveByPackage.UniqueID = reinterpret_cast<unsigned int>(this);
    TmpDriveByPackage.vLocation = *GetPhysCar()->GetPosition();

    if (GetPhysCar()->GetContext() == Sound::CONTEXT_COP) {
        MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero,
                               GetPhysCar()->mWorldID,
                               0);
        moment.Send(UCrc32("BlewByCop"));
    }

    CSTATE_Base *ReturnedObj;
    ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&TmpDriveByPackage);
    if (ReturnedObj != nullptr) {
        ReturnedObj->Attach(&TmpDriveByPackage);
    }
}

CSTATE_Base::StateInfo EAXCopCar::s_StateInfo = {
    0x00040000,
    "EAXCopCar",
    &EAXAITunerCar::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(void)>(&EAXCopCar::CreateState),
};

CSTATE_Base::StateInfo *EAXCopCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXCopCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXCopCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXCopCar), s_StateInfo.stateName, false)) EAXCopCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXCopCar), s_StateInfo.stateName, true)) EAXCopCar;
    }
}

CSTATE_Base::StateInfo *EAXTruck::GetStateInfo(void) const {
    return &s_StateInfo;
}

CSTATE_Base::StateInfo EAXTruck::s_StateInfo = {
    0x000C0000,
    "EAXTruck",
    &EAXAITunerCar::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(void)>(&EAXTruck::CreateState),
};

const char *EAXTruck::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTruck::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTruck), s_StateInfo.stateName, false)) EAXTruck;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTruck), s_StateInfo.stateName, true)) EAXTruck;
    }
}

void EAXTruck::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::Attach(void *pAttachment) {
    EAX_CarState *pCar = static_cast<EAX_CarState *>(pAttachment);
    if (CSTATEMGR_CarState::FinalCopV8Engines.size() != 0) {
        pCar->GetEngineInfo()->ChangeWithDefault(CSTATEMGR_CarState::FinalCopV8Engines[0]);
    }
    EAXCar::Attach(pAttachment);
}
