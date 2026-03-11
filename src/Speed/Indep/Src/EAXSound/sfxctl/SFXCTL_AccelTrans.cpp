#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"

SFXCTL_AccelTrans::~SFXCTL_AccelTrans() {}

SndBase::TypeInfo *SFXCTL_AccelTrans::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AccelTrans::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_AccelTrans::Destroy() {}

int SFXCTL_AccelTrans::GetController(int Index) {
    if (Index == 1) {
        return 2;
    }
    if (Index < 2 && Index == 0) {
        return 4;
    }
    return -1;
}
