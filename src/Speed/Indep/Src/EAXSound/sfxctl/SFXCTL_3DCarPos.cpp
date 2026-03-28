#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

SFXCTL_3DCarPos::~SFXCTL_3DCarPos() {}

SFXCTL_3DCarPos::SFXCTL_3DCarPos() {}

SndBase *SFXCTL_3DCarPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DCarPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DCarPos();
    }
    return new (SFXCTL_3DCarPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DCarPos();
}

void SFXCTL_3DCarPos::InitSFX() {
    SFXCTL_3DObjPos::InitSFX();
}

SndBase::TypeInfo *SFXCTL_3DCarPos::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_3DCarPos::GetTypeName() const { return s_TypeInfo.typeName; }
