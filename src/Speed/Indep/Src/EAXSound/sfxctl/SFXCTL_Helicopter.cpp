#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"

SFXCTL_3DHeliPos::~SFXCTL_3DHeliPos() {}

SndBase *SFXCTL_3DHeliPos::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_3DHeliPos();
}

SFXCTL_Helicopter::SFXCTL_Helicopter()
    : m_pHeliState(nullptr) //
    , m_p3DHeliPosCtl(nullptr) {}

SFXCTL_Helicopter::~SFXCTL_Helicopter() {}

SndBase::TypeInfo *SFXCTL_3DHeliPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DHeliPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_Helicopter::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Helicopter::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Helicopter::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Helicopter();
}

int SFXCTL_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 1;
    }
    return -1;
}

void SFXCTL_Helicopter::AttachController(SFXCTL *controller) {
    SndBase::AttachController(controller);
    if (!m_p3DHeliPosCtl) {
        m_p3DHeliPosCtl = static_cast< SFXCTL_3DHeliPos * >(controller);
    }
}

void SFXCTL_Helicopter::Detach() { m_pHeliState = nullptr; }

void SFXCTL_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pHeliState = nullptr;
}

void SFXCTL_Helicopter::InitSFX() {}

void SFXCTL_Helicopter::UpdateParams(float t) { (void)t; }
