#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"

static const int SND_AI_createFX = 1; // Decl: 34

int DEBUG_ONLINE_ENGINES = 0; // Decl: 40

int AI_CARS_BANKS[4] = {0, 0, 0, 0}; // Decl: 43
int g_AI_PITCH_OFFSET = 0;           // Decl: 44

STATETYPE_IMPLEMENT(0x30000, EAXAITunerCar, EAXCar);

EAXAITunerCar::EAXAITunerCar() {
    emAddHandler(ProcessEvent, 0x00040000);
    this->mPhysicsChangedGear = false;
}

EAXAITunerCar::~EAXAITunerCar() {
    emRemoveHandler(ProcessEvent);
}

// STRIPPED
bool EAXAITunerCar::IsEngineFinishedLoading() {
    return false;
}

int EAXAITunerCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) {
    if (SFXMessageType == SFX_CHANGEGEAR) {
        this->mPhysicsChangedGear = true;
        return 0;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void EAXAITunerCar::UpdateCarPhysics() {
    g_EAXIsPaused();
}

int SND_AI_FollowAICar = 0; // Decl: 103

void EAXAITunerCar::UpdateParams(float t) {
    if (this->m_pCar != nullptr) {
        EAXCar::UpdateParams(t);
        this->mPhysicsChangedGear = false;
    }
}

void EAXAITunerCar::ProcessEvent(emEvent *event) {}

static const float AI_WOOSH_DIST = 12.0f;           // Decl: 204
static const float AI_WOOSH_MIN_SPEED_TRIG = 15.0f; // Decl: 205

void EAXAITunerCar::UpdatAIDriveBy(float t) {
    if (this->GetPhysCar() == nullptr) {
        return;
    }

    EAX_CarState *ClosestPlayer = GetClosestPlayerCar(this->GetPhysCar()->GetPosition());
    if (!IsCarInRadius(ClosestPlayer, this->GetPhysCar()->GetPosition(), AI_WOOSH_DIST)) {
        return;
    }

    bVector3 vVelDiff = bSub(*this->GetPhysCar()->GetVelocity(), *ClosestPlayer->GetVelocity());
    float fRelativeVel = bLength(vVelDiff);
    if (fRelativeVel < AI_WOOSH_MIN_SPEED_TRIG) {
        return;
    }

    stDriveByInfo TmpDriveByPackage;
    TmpDriveByPackage.eDriveByType = DRIVE_BY_AI_CAR;
    TmpDriveByPackage.ClosingVelocity = fRelativeVel;
    TmpDriveByPackage.pEAXCar = this;
    TmpDriveByPackage.UniqueID = reinterpret_cast<uintptr_t>(this);
    TmpDriveByPackage.vLocation = *this->GetPhysCar()->GetPosition();

    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_COP) {
        MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, this->GetPhysCar()->mWorldID, 0)
            .Send(UCrc32("BlewByCop"));
    }

    CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&TmpDriveByPackage);
    if (ReturnedObj != nullptr) {
        ReturnedObj->Attach(&TmpDriveByPackage);
    }
}

STATETYPE_IMPLEMENT(0x40000, EAXCopCar, EAXAITunerCar);

STATETYPE_IMPLEMENT(0xC0000, EAXTruck, EAXAITunerCar);

void EAXTruck::UpdateParams(float t) {
    this->UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::UpdateParams(float t) {
    this->UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::Attach(void *pAttachment) {
    EAX_CarState *pCar = static_cast<EAX_CarState *>(pAttachment);
    if (CSTATEMGR_CarState::FinalCopV8Engines.size() != 0) {
        pCar->GetEngineInfo()->ChangeWithDefault(CSTATEMGR_CarState::FinalCopV8Engines[0]);
    }
    EAXCar::Attach(pAttachment);
}
