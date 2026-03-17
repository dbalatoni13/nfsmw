#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

EAXFrontEnd::EAXFrontEnd() {
    int i;
    int Index;

    for (Index = 0; Index < NUM_CAR_INDEXS; Index++) {
        for (i = 0; i < NUM_DRIVE_ON_STATES; i++) {
            m_pDriveOnOffSampleHandle[Index][i] = nullptr;
            DriveOnFadeOut[Index][i].Initialize(1.0f, 1.0f, 100, LINEAR);
            IsEnding[Index][i] = false;
        }
        DriveONCarState[Index] = DRIVE_ON_NONE;
    }

    m_pSFXOBJ_FEHUD = nullptr;

    {
        for (int k = 0; k < 4; k++) {
            m_hydraulicsControls[k] = nullptr;
            m_hydraulicsBounce[k] = nullptr;
        }
    }

    m_pPlayRapSheet = nullptr;
}

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

EAXCommon::EAXCommon() {
    mMsgMiscSound =
        Hermes::Handler::Create<MMiscSound, EAXCommon, EAXCommon>(this, &EAXCommon::MsgPlayMiscSound, "Snd", 0);
    m_pSFXOBJ_FEHUD = nullptr;
    m_pRadar = nullptr;
}

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
