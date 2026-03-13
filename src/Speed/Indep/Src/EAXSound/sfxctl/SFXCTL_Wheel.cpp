#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"

SFXCTL_Wheel::SFXCTL_Wheel()
    : LeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , RightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevLeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevRightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) {}

SFXCTL_Wheel::~SFXCTL_Wheel() {}

SndBase *SFXCTL_Wheel::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Wheel();
}

SndBase::TypeInfo *SFXCTL_Wheel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Wheel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Wheel::InitSFX() {
    SFXCTL::InitSFX();
}

void SFXCTL_Wheel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateTireParams();
}

bVector3 *SFXCTL_Wheel::GetWheelPos(int wheelID, int numtires) {
    if (numtires == 2) {
        if (wheelID == 0) {
            return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
        }
        return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x34);
    }
    if (numtires == 1) {
        return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
    }
    return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
}

void SFXCTL_Wheel::GenerateWheelPosition() {}

void SFXCTL_Wheel::GenerateTerrainTypes() {}

void SFXCTL_Wheel::UpdateTireParams() {
    GenerateWheelPosition();
    GenerateTerrainTypes();
}
