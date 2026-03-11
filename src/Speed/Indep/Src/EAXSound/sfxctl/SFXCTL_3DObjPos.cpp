#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

SndBase::TypeInfo *SFXCTL_3DObjPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DObjPos::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_3DObjPos::SetPlayerRef(int i) { m_PlayerRef = i; }

void SFXCTL_3DObjPos::AssignPositionVector(bVector3 *pV3ObjPos) { m_pV3ObjPos = pV3ObjPos; }

void SFXCTL_3DObjPos::AssignDirectionVector(const bVector3 *pV3ObjDir) {
    m_pV3ObjDir = const_cast<bVector3 *>(pV3ObjDir);
}

void SFXCTL_3DObjPos::AssignVelocityVector(const bVector3 *pV3ObjVel) {
    m_pV3ObjVel = const_cast<bVector3 *>(pV3ObjVel);
}

void SFXCTL_3DObjPos::Detach() {
    m_pV3ObjPos = nullptr;
    m_pV3ObjDir = nullptr;
}
