#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"

SFXCTL_3DColPos::~SFXCTL_3DColPos() {}

SFXCTL_3DScrapePos::~SFXCTL_3DScrapePos() {}

SndBase *SFXCTL_3DColPos::CreateObject(unsigned int allocator) {
    if (allocator != 0) {
        return new (SFXCTL_3DColPos::s_TypeInfo.typeName, true) SFXCTL_3DColPos();
    } else {
        return new (SFXCTL_3DColPos::s_TypeInfo.typeName, false) SFXCTL_3DColPos();
    }
}

SndBase *SFXCTL_3DScrapePos::CreateObject(unsigned int allocator) {
    if (allocator != 0) {
        return new (SFXCTL_3DScrapePos::s_TypeInfo.typeName, true) SFXCTL_3DScrapePos();
    } else {
        return new (SFXCTL_3DScrapePos::s_TypeInfo.typeName, false) SFXCTL_3DScrapePos();
    }
}

SndBase::TypeInfo *SFXCTL_3DColPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DColPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_3DScrapePos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DScrapePos::GetTypeName() const { return s_TypeInfo.typeName; }
