#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <new>
#include <types.h>

struct emEvent;
struct bVector3;

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
};
} // namespace AIDriveBy

void emAddHandler(void (*function)(emEvent *), unsigned int stream_mask);
void emRemoveHandler(void (*function)(emEvent *));
bool g_EAXIsPaused(void);

CSTATE_Base::StateInfo EAXAITunerCar::s_StateInfo = {
    0x00030000,
    "EAXAITunerCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXAITunerCar::CreateState),
};

CSTATE_Base::StateInfo *EAXAITunerCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXAITunerCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXAITunerCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXAITunerCar), GetStaticStateInfo()->stateName, false)) EAXAITunerCar;
    } else {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXAITunerCar), GetStaticStateInfo()->stateName, true)) EAXAITunerCar;
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
    bVector3 vVelDiff;
    float fRelativeVel;
    AIDriveBy::stAIDriveByInfo TmpDriveByPackage;
    CSTATE_Base *ReturnedObj;

    (void)t;

    if (m_pCar == nullptr) {
        return;
    }

    ClosestPlayer = GetClosestPlayerCar(static_cast<bVector3 *>(static_cast<void *>(&m_pCar->mMatrix.v3)));
    if (!IsCarInRadius(ClosestPlayer, static_cast<bVector3 *>(static_cast<void *>(&m_pCar->mMatrix.v3)), 12.0f)) {
        return;
    }

    vVelDiff = bSub(
        m_pCar->mVel0,
        ClosestPlayer->mVel0);
    fRelativeVel = bLength(vVelDiff);
    if (fRelativeVel < 15.0f) {
        return;
    }

    TmpDriveByPackage.eDriveByType = AIDriveBy::DRIVE_BY_AI_CAR;
    TmpDriveByPackage.ClosingVelocity = fRelativeVel;
    TmpDriveByPackage.vLocation = *static_cast<bVector3 *>(static_cast<void *>(&m_pCar->mMatrix.v3));
    TmpDriveByPackage.pEAXCar = this;
    TmpDriveByPackage.UniqueID = reinterpret_cast< unsigned int >(this);

    if (m_pCar->mContext == Sound::CONTEXT_COP) {
        MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero,
                               m_pCar->mWorldID,
                               0);
        moment.Send(UCrc32("BlewByCop"));
    }

    ReturnedObj = EAXSound::m_pStateMgr[eMM_DRIVEBY]->GetFreeState(&TmpDriveByPackage);
    if (ReturnedObj != nullptr) {
        ReturnedObj->Attach(&TmpDriveByPackage);
    }
}

CSTATE_Base::StateInfo EAXCopCar::s_StateInfo = {
    0x00040000,
    "EAXCopCar",
    &EAXAITunerCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXCopCar::CreateState),
};

CSTATE_Base::StateInfo *EAXCopCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXCopCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXCopCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXCopCar), GetStaticStateInfo()->stateName, false)) EAXCopCar;
    } else {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXCopCar), GetStaticStateInfo()->stateName, true)) EAXCopCar;
    }
}

CSTATE_Base::StateInfo *EAXTruck::GetStateInfo(void) const {
    return &s_StateInfo;
}

CSTATE_Base::StateInfo EAXTruck::s_StateInfo = {
    0x000C0000,
    "EAXTruck",
    &EAXAITunerCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXTruck::CreateState),
};

const char *EAXTruck::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTruck::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXTruck), GetStaticStateInfo()->stateName, false)) EAXTruck;
    } else {
        return ::new (AudioMemBase::operator new(
            sizeof(EAXTruck), GetStaticStateInfo()->stateName, true)) EAXTruck;
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
