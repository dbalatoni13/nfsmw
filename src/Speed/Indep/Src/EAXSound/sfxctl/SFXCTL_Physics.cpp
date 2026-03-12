#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

SFXCTL_Physics::~SFXCTL_Physics() {
    if (mMsgRevEngine) {
        Hermes::Handler::Destroy(mMsgRevEngine);
    }
    if (mMsgRevOff) {
        Hermes::Handler::Destroy(mMsgRevOff);
    }
}

SFXCTL_AIPhysics::~SFXCTL_AIPhysics() {}

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pEAXCar->m_pPhysicsCTL = this;
}

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_AIPhysics::GetController(int Index) {
    if (Index == 0) {
        return 2;
    }
    return -1;
}

void SFXCTL_AIPhysics::Destroy() {}

void SFXCTL_AIPhysics::UpdateMixerOutputs() {}

void SFXCTL_AIPhysics::SetupSFX(CSTATE_Base *_StateBase) {
    SFXCTL_Physics::SetupSFX(_StateBase);
}

void SFXCTL_AIPhysics::InitSFX() {
    SFXCTL_Physics::InitSFX();
    AIStateManager.Initialize(static_cast<SFXCTL_Physics *>(this));
    Zero60Time = 4.5f;
}

void SFXCTL_AIPhysics::AttachController(SFXCTL *psfxctl) {
    TypeInfo *pInfo = psfxctl->GetTypeInfo();
    if (((pInfo->ObjectID >> 4) & 0xFFF) == 2) {
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
    }
}

void SFXCTL_AIPhysics::UpdateTorque(float t) {
    float target;
    if (IsAccelerating) {
        target = 100.0f;
    } else {
        target = 0.0f;
    }
    PhysicsTRQ = smooth(PhysicsTRQ, target, 50.0f);
}

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }
