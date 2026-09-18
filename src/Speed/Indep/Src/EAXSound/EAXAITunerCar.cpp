#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"

STATETYPE_IMPLEMENT(0x00030000, EAXAITunerCar, EAXCar)

EAXAITunerCar::EAXAITunerCar() {
    emAddHandler(EAXAITunerCar::ProcessEvent, 0x40000);
    this->mPhysicsChangedGear = false;
}

EAXAITunerCar::~EAXAITunerCar() {
    emRemoveHandler(EAXAITunerCar::ProcessEvent);
}

int EAXAITunerCar::SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2) {
    if (SFXMessageType == SFX_CHANGEGEAR) {
        this->mPhysicsChangedGear = true;
        return 0;
    } else {
        return EAXCar::SFXMessage(SFXMessageType, param1, param2);
    }
}

void EAXAITunerCar::UpdateCarPhysics() {
    g_EAXIsPaused();
}

void EAXAITunerCar::UpdateParams(float t) {
    if (this->m_pCar != nullptr) {
        EAXCar::UpdateParams(t);
        this->mPhysicsChangedGear = false;
    }
}

void EAXAITunerCar::ProcessEvent(emEvent *event) {}

void EAXAITunerCar::UpdatAIDriveBy(float t) {
    if (GetPhysCar() != nullptr) {
        EAX_CarState *ClosestPlayer = GetClosestPlayerCar(GetPhysCar()->GetPosition());

        if (IsCarInRadius(ClosestPlayer, GetPhysCar()->GetPosition(), 12.0f)) {
            bVector3 vVelDiff = bSub(*GetPhysCar()->GetVelocity(), *ClosestPlayer->GetVelocity());
            float fRelativeVel = bLength(vVelDiff);

            if (fRelativeVel >= 15.0f) {
                stDriveByInfo TmpDriveByPackage;

                TmpDriveByPackage.eDriveByType = DRIVE_BY_AI_CAR;
                TmpDriveByPackage.pEAXCar = this;
                TmpDriveByPackage.UniqueID = (uintptr_t) this;
                TmpDriveByPackage.ClosingVelocity = fRelativeVel;
                bCopy(&TmpDriveByPackage.vLocation, GetPhysCar()->GetPosition());

                if (GetPhysCar()->mContext == 2) {
                    MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero,
                                    GetPhysCar()->mWorldID, 0)
                        .Send(UCrc32("BlewByCop"));
                }

                CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&TmpDriveByPackage);
                if (ReturnedObj != nullptr) {
                    ReturnedObj->Attach(&TmpDriveByPackage);
                }
            }
        }
    }
}

STATETYPE_IMPLEMENT(0x00040000, EAXCopCar, EAXAITunerCar)

STATETYPE_IMPLEMENT(0x000C0000, EAXTruck, EAXAITunerCar)

void EAXTruck::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::Attach(void *pAttachment) {
    if (CSTATEMGR_CarState::FinalCopV8Engines.size() != 0) {
        static_cast<EAX_CarState *>(pAttachment)->mEngineInfo.ChangeWithDefault(
            CSTATEMGR_CarState::FinalCopV8Engines[0]);
    }
    EAXCar::Attach(pAttachment);
}
