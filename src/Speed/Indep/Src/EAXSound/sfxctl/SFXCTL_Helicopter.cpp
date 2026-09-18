#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Helicopter.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

DEFINE_CREATABLE(0xb0010, SFXCTL_3DHeliPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0xb0000, SFXCTL_Helicopter, SFXCTL);

SFXCTL_Helicopter::SFXCTL_Helicopter() {
    this->m_pHeliState = nullptr;
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

void SFXCTL_Helicopter::UpdateParams(float t) {
    if (this->m_pHeliState != nullptr && this->m_pHeliState->IsSimUpdating() == true) {
        bCopy(&this->vHeliPos, this->m_pHeliState->GetPosition());
        bCopy(&this->vHeliFwd, this->m_pHeliState->GetForwardVector());
        bCopy(&this->vHeliVel, this->m_pHeliState->GetVelocity());

        EAX_CarState *pcar = GetClosestPlayerCar(&this->vHeliPos);
        if (pcar != nullptr) {
            bVector3 pPos;
            bVector3 p2h;
            bVector3 np2h;

            this->m_fdist = bDistBetween(&this->vHeliPos, pcar->GetPosition());

            this->m_fspeed = this->m_pHeliState->GetForwardSpeed() * 0.002f;
            if (this->m_fspeed > 1.0f) {
                this->m_fspeed = 1.0f;
            } else if (this->m_fspeed < 0.25f) {
                this->m_fspeed = 0.5f;
            }
            this->m_fspeed = this->m_fspeed * 1023.0f;

            bCopy(&pPos, pcar->GetPosition());
            bSub(&p2h, &pPos, &this->vHeliPos);
            bNormalize(&np2h, &p2h);

            this->m_Rotation = (bACos(bDot(&np2h, &this->vHeliFwd)) >> 6) & 0x3FF;
        }
    }
}
