#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SndBase::TypeInfo *SFXCTL_3DHeliPos::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_3DHeliPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_3DHeliPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DHeliPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DHeliPos();
    }
    return new (SFXCTL_3DHeliPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DHeliPos();
}

SndBase::TypeInfo *SFXCTL_Helicopter::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Helicopter::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Helicopter::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Helicopter::GetStaticTypeInfo()->typeName, false) SFXCTL_Helicopter();
    }
    return new (SFXCTL_Helicopter::GetStaticTypeInfo()->typeName, true) SFXCTL_Helicopter();
}

SFXCTL_Helicopter::SFXCTL_Helicopter()
    : m_pHeliState(nullptr) {
    this->vHeliPos = bVector3(0.0f, 0.0f, 0.0f);
    this->vHeliFwd = bVector3(0.0f, 0.0f, 0.0f);
}

SFXCTL_Helicopter::~SFXCTL_Helicopter() {}

int SFXCTL_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 1;
    }
    return -1;
}

void SFXCTL_Helicopter::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 1) {
        this->m_p3DHeliPosCtl = static_cast<SFXCTL_3DHeliPos *>(psfxctl);
    }
}

void SFXCTL_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXCTL_Helicopter::InitSFX() {
    this->m_pHeliState = static_cast<EAX_HeliState *>(this->m_pStateBase->m_pAttachment);
    this->m_p3DHeliPosCtl->AssignPositionVector(&this->vHeliPos);
    this->m_p3DHeliPosCtl->AssignVelocityVector(&this->vHeliVel);
    this->m_p3DHeliPosCtl->AssignDirectionVector(&this->vHeliFwd);
}

void SFXCTL_Helicopter::Detach() { this->m_pHeliState = nullptr; }

void SFXCTL_Helicopter::UpdateParams(float t) {
    float dot;

    (void)t;

    if (this->m_pHeliState && this->m_pHeliState->IsSimUpdating()) {
        EAX_CarState *pcar;

        this->vHeliPos = *this->m_pHeliState->GetPosition();
        this->vHeliFwd = *this->m_pHeliState->GetForwardVector();
        this->vHeliVel = *this->m_pHeliState->GetVelocity();

        pcar = GetClosestPlayerCar(&this->vHeliPos);
        if (!pcar) {
            return;
        }

        this->m_fdist = bDistBetween(&this->vHeliPos, pcar->GetPosition());
        this->m_fspeed = this->m_pHeliState->GetForwardSpeed() * 0.002f;
        if (this->m_fspeed > 1.0f) {
            this->m_fspeed = 1.0f;
        } else if (this->m_fspeed < 0.25f) {
            this->m_fspeed = 0.5f;
        }

        this->m_fspeed *= 1023.0f;

        bVector3 pPos(*pcar->GetPosition());
        bVector3 p2h;
        bSub(&p2h, &pPos, &this->vHeliPos);
        bVector3 np2h;
        bNormalize(&np2h, &p2h);
        dot = bDot(&np2h, &this->vHeliFwd);
        this->m_Rotation = bACos(dot) >> 6;
    }
}
