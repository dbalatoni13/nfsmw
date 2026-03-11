#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"

void EAXFrontEnd::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    m_pSFXOBJ_FEHUD = psfx;
}

void EAXFrontEnd::Initialize() {}

void EAXFrontEnd::Update(void *peventst) {
    UpdateDriveOn();
}

void *EAXFrontEnd::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXFrontEnd::UpdateDriveOn() {}

void EAXFrontEnd::SetFEDrivingCarState(bVector3 *, bVector3 *, Camera *, int) {}

void EAXFrontEnd::DestroyAllDriveOnSnds() {}

void EAXCommon::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    m_pSFXOBJ_FEHUD = psfx;
}

void EAXCommon::Initialize() {}

void EAXCommon::Stop(eMenuSoundTriggers etrigger) {}

void *EAXCommon::GetEventPointer(int neventindex) {
    return nullptr;
}
