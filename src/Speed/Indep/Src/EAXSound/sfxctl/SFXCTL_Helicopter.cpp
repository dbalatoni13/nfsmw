#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SFXCTL_3DHeliPos::~SFXCTL_3DHeliPos() {}

SndBase *SFXCTL_3DHeliPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DHeliPos::s_TypeInfo.typeName, false) SFXCTL_3DHeliPos();
    }
    return new (SFXCTL_3DHeliPos::s_TypeInfo.typeName, true) SFXCTL_3DHeliPos();
}

SFXCTL_Helicopter::SFXCTL_Helicopter()
    : m_pHeliState(nullptr) {
    vHeliPos = bVector3(0.0f, 0.0f, 0.0f);
    vHeliFwd = bVector3(0.0f, 0.0f, 0.0f);
}

SFXCTL_Helicopter::~SFXCTL_Helicopter() {}

SndBase::TypeInfo *SFXCTL_3DHeliPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DHeliPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_Helicopter::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Helicopter::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Helicopter::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Helicopter::s_TypeInfo.typeName, false) SFXCTL_Helicopter();
    }
    return new (SFXCTL_Helicopter::s_TypeInfo.typeName, true) SFXCTL_Helicopter();
}

int SFXCTL_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 1;
    }
    return -1;
}

void SFXCTL_Helicopter::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 1) {
        m_p3DHeliPosCtl = static_cast<SFXCTL_3DHeliPos *>(psfxctl);
    }
}

void SFXCTL_Helicopter::Detach() { m_pHeliState = nullptr; }

void SFXCTL_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXCTL_Helicopter::InitSFX() {
    m_pHeliState = static_cast<EAX_HeliState *>(m_pStateBase->m_pAttachment);
    m_p3DHeliPosCtl->AssignPositionVector(&vHeliPos);
    m_p3DHeliPosCtl->AssignVelocityVector(&vHeliVel);
    m_p3DHeliPosCtl->AssignDirectionVector(&vHeliFwd);
}

void SFXCTL_Helicopter::UpdateParams(float t) {
    float dot;

    (void)t;

    if (m_pHeliState != nullptr && m_pHeliState->IsSimUpdating()) {
        EAX_CarState *pcar;

        vHeliPos = *m_pHeliState->GetPosition();
        vHeliFwd = *m_pHeliState->GetForwardVector();
        vHeliVel = *m_pHeliState->GetVelocity();

        pcar = GetClosestPlayerCar(&vHeliPos);
        if (pcar == nullptr) {
            return;
        }

        m_fdist = bDistBetween(&vHeliPos, pcar->GetPosition());
        m_fspeed = m_pHeliState->GetForwardSpeed() * 0.002f;
        if (m_fspeed > 1.0f) {
            m_fspeed = 1.0f;
        } else if (m_fspeed < 0.25f) {
            m_fspeed = 0.5f;
        }

        m_fspeed *= 1023.0f;

        bVector3 pPos(*pcar->GetPosition());
        bVector3 p2h;
        bSub(&p2h, &pPos, &vHeliPos);
        bVector3 np2h;
        bNormalize(&np2h, &p2h);
        dot = bDot(&np2h, &vHeliFwd);
        m_Rotation = bACos(dot) >> 6;
    }
}
