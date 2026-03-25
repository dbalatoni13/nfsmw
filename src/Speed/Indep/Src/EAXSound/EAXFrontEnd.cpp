#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/FE_AEMS.h"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float g_SliderValue;
extern int Debug_Common_FE_OFF;

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
    for (int k = 0; k < 4; k++) {
        delete m_hydraulicsControls[k];
        m_hydraulicsBounce[k] = nullptr;
        delete m_hydraulicsControls[k];
        m_hydraulicsControls[k] = nullptr;
    }

    delete m_pPlayRapSheet;
    m_pPlayRapSheet = nullptr;
}

void EAXFrontEnd::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    m_pSFXOBJ_FEHUD = psfx;
}

void EAXFrontEnd::Initialize() {}

int EAXFrontEnd::Play(eMenuSoundTriggers etrigger) {
    int nvol;
    int testID;

    if (IsSoundEnabled == 0) {
        return -1;
    }

    nvol = 0;
    if (m_pSFXOBJ_FEHUD) {
        nvol = m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL);
        g_pEAXSound->SetCsisName(m_pSFXOBJ_FEHUD);

        testID = static_cast<int>(etrigger);
        if (static_cast<unsigned int>(testID - 80) < 16 && etrigger != UISND_EA_MSGR_CHALLENGE_REQ &&
            etrigger != UISND_MAIN_MENU) {
            delete m_pPlayRapSheet;
            m_pPlayRapSheet = new PlayFrontEndSample_RS(testID, nvol, 0x1000, 0);
        } else {
            int getref;

            m_pPlayFrontEndSampleHandle = new PlayFrontEndSample(testID, nvol, 0x1000, 0);
            getref = m_pPlayFrontEndSampleHandle->GetRefCount();
            delete m_pPlayFrontEndSampleHandle;
            m_pPlayFrontEndSampleHandle = nullptr;
        }

        nvol = 0;
    }

    return nvol;
}

void EAXFrontEnd::Stop(eMenuSoundTriggers etrigger) {
    if (IsSoundEnabled != 0 && m_pPlayRapSheet && m_pPlayRapSheet->GetId() == etrigger) {
        delete m_pPlayRapSheet;
    }
}

int EAXFrontEnd::Play(void *peventst) {
    if (IsSoundEnabled != 0 && m_pSFXOBJ_FEHUD) {
        if (m_pSFXOBJ_FEHUD->GetOutputBlockPtr() == nullptr) {
            return 0;
        }

        if (peventst != nullptr) {
            PlayFrontEndSampleSt *pst = static_cast<PlayFrontEndSampleSt *>(peventst);
            int Vol = pst->volume;
            int nvol = Vol * m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL) >> 15;

            g_pEAXSound->SetCsisName(m_pSFXOBJ_FEHUD);
            m_pPlayFrontEndSampleHandle = new PlayFrontEndSample(pst->id, nvol, pst->pitch, pst->azimuth);
            if (m_pPlayFrontEndSampleHandle) {
                delete m_pPlayFrontEndSampleHandle;
            }
            m_pPlayFrontEndSampleHandle = nullptr;
        }
    }

    return -1;
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
    int nvol;

    if (!IsSoundEnabled) return -1;
    if (Debug_Common_FE_OFF) return -1;

    SndBase *snd = m_pSFXOBJ_FEHUD;
    if (!snd) return -1;
    if (!snd->GetOutputBlockPtr()) return 0;

    nvol = snd->GetDMixOutput(1, DMX_VOL);
    if (-1.0f < g_SliderValue) {
        int CurSliderVol = static_cast<int>(g_SliderValue * 32767.0f);
        if (CurSliderVol < 0) CurSliderVol = 0;
        if (CurSliderVol > 0x7FFF) CurSliderVol = 0x7FFF;

        g_SliderValue = -1.0f;
        nvol = nvol * CurSliderVol >> 15;
    }

    if (etrigger == UISND_ENTER_TRIGGER) {
        snd->SetDMIX_Input(4, 0x7FFF);
    }

    g_pEAXSound->SetCsisName(snd);
    m_pPlayCommonSampleHandle = new PlayCommonSample(static_cast<int>(etrigger), nvol, 0x1000, 0);
    if (m_pPlayCommonSampleHandle) {
        delete m_pPlayCommonSampleHandle;
    }
    m_pPlayCommonSampleHandle = nullptr;
    return 0;
}

void EAXCommon::Stop(eMenuSoundTriggers etrigger) {}

int EAXCommon::Play(void *peventst) {
    if (IsSoundEnabled != 0 && m_pSFXOBJ_FEHUD) {
        if (m_pSFXOBJ_FEHUD->GetOutputBlockPtr() == nullptr) {
            return 0;
        }

        if (peventst != nullptr) {
            PlayCommonSampleSt *pst = static_cast<PlayCommonSampleSt *>(peventst);
            int Vol = pst->volume;
            int nvol = Vol * m_pSFXOBJ_FEHUD->GetDMixOutput(1, DMX_VOL) >> 15;

            g_pEAXSound->SetCsisName(m_pSFXOBJ_FEHUD);
            m_pPlayCommonSampleHandle = new PlayCommonSample(pst->id, nvol, pst->pitch, pst->azimuth);
            if (m_pPlayCommonSampleHandle) {
                delete m_pPlayCommonSampleHandle;
            }
            m_pPlayCommonSampleHandle = nullptr;
        }
    }

    return -1;
}

void EAXCommon::Update(void *peventst) {
    (void)peventst;

    if (m_pRadar) {
        int refCount = m_pRadar->GetRefCount();

        if (refCount < 2) {
            delete m_pRadar;
            m_pRadar = nullptr;
        }
    }

    if (m_pSFXOBJ_FEHUD) {
        m_pSFXOBJ_FEHUD->SetDMIX_Input(4, 0);
    }
}

void *EAXCommon::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXCommon::MsgPlayMiscSound(const MMiscSound &msg) {
    if (msg.GetSoundID() == 0) {
        if (m_pRadar) {
            delete m_pRadar;
        }

        g_pEAXSound->SetCsisName("FE Radar");
        m_pRadar = new FX_Radar(0, m_pSFXOBJ_FEHUD->GetDMixOutput(3, DMX_VOL), 0, 0, 0);
    }
}
