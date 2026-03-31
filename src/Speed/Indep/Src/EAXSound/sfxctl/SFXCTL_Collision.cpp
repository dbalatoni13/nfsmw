#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"

SndBase::TypeInfo *SFXCTL_3DColPos::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_3DColPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_3DColPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DColPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DColPos();
    }
    return new (SFXCTL_3DColPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DColPos();
}

SndBase::TypeInfo *SFXCTL_3DScrapePos::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_3DScrapePos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_3DScrapePos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DScrapePos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DScrapePos();
    }
    return new (SFXCTL_3DScrapePos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DScrapePos();
}
