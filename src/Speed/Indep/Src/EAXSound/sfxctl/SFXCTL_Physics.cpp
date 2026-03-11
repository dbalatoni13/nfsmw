#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_AIPhysics::GetController(int Index) {
    if (Index == 0) {
        return 2;
    }
    return -1;
}

void SFXCTL_AIPhysics::Destroy() {}

void SFXCTL_AIPhysics::UpdateMixerOutputs() {}

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }
