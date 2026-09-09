#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

DEFINE_CREATABLE(0xb0010, SFXCTL_3DHeliPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0xb0000, SFXCTL_Helicopter, SFXCTL);

SFXCTL_Helicopter::SFXCTL_Helicopter() : m_pHeliState(nullptr) {
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

void SFXCTL_Helicopter::Detach() {
    this->m_pHeliState = nullptr;
}

extern SndCamera *pSndCamera; // size: 0x4, Decl: 59

static const float twk_fmaxhelispeed = 500.0f; // size: 0x4, Decl: 61
void SFXCTL_Helicopter::UpdateParams(float t) {
    float dot;

    if ((this->m_pHeliState != nullptr) && (this->m_pHeliState->IsSimUpdating() == true)) {
        this->vHeliPos = *this->m_pHeliState->GetPosition();
        this->vHeliFwd = *this->m_pHeliState->GetForwardVector();
        this->vHeliVel = *this->m_pHeliState->GetVelocity();
        
        EAX_CarState *pcar = GetClosestPlayerCar(&this->vHeliPos);
        if (pcar == nullptr) {
            return;
        }

        this->m_fdist = bDistBetween(&this->vHeliPos, pcar->GetPosition());
        this->m_fspeed = this->m_pHeliState->GetForwardSpeed() / twk_fmaxhelispeed;
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
