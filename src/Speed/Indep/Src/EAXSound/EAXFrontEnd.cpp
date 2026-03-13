#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

EAXFrontEnd::EAXFrontEnd()
    : m_pSFXOBJ_FEHUD(nullptr) {}

EAXFrontEnd::~EAXFrontEnd() {
    DestroyAllDriveOnSnds();
}

void EAXFrontEnd::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    m_pSFXOBJ_FEHUD = psfx;
}

void EAXFrontEnd::Initialize() {}

int EAXFrontEnd::Play(eMenuSoundTriggers etrigger) {
    return Play(GetEventPointer(static_cast< int >(etrigger)));
}

void EAXFrontEnd::Stop(eMenuSoundTriggers etrigger) {
    (void)etrigger;
}

int EAXFrontEnd::Play(void *peventst) {
    (void)peventst;
    return 0;
}

void EAXFrontEnd::Update(void *peventst) {
    UpdateDriveOn();
}

void *EAXFrontEnd::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXFrontEnd::UpdateDriveOn() {}

void EAXFrontEnd::SetFEDrivingCarState(bVector3 *, bVector3 *, Camera *, int) {}

void EAXFrontEnd::DestroyAllDriveOnSnds() {}

EAXCommon::EAXCommon()
    : m_pSFXOBJ_FEHUD(nullptr) //
    , mMsgMiscSound(nullptr) {}

EAXCommon::~EAXCommon() {
    if (mMsgMiscSound != nullptr) {
        Hermes::Handler::Destroy(mMsgMiscSound);
    }
}

void EAXCommon::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    m_pSFXOBJ_FEHUD = psfx;
}

void EAXCommon::Initialize() {}

int EAXCommon::Play(eMenuSoundTriggers etrigger) {
    return Play(GetEventPointer(static_cast< int >(etrigger)));
}

void EAXCommon::Stop(eMenuSoundTriggers etrigger) {}

int EAXCommon::Play(void *peventst) {
    (void)peventst;
    return 0;
}

void EAXCommon::Update(void *peventst) {
    (void)peventst;
}

void *EAXCommon::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXCommon::MsgPlayMiscSound(const MMiscSound &msg) {
    Play(static_cast< eMenuSoundTriggers >(msg.GetSoundID()));
}
