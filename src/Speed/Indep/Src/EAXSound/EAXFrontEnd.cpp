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
            this->m_pDriveOnOffSampleHandle[Index][i] = nullptr;
            this->DriveOnFadeOut[Index][i].Initialize(1.0f, 1.0f, 100, LINEAR);
            this->IsEnding[Index][i] = false;
        }
        this->DriveONCarState[Index] = DRIVE_ON_NONE;
    }

    this->m_pSFXOBJ_FEHUD = nullptr;

    {
        for (int k = 0; k < 4; k++) {
            this->m_hydraulicsControls[k] = nullptr;
            this->m_hydraulicsBounce[k] = nullptr;
        }
    }

    this->m_pPlayRapSheet = nullptr;
}

EAXFrontEnd::~EAXFrontEnd() {
    for (int k = 0; k < 4; k++) {
        delete this->m_hydraulicsControls[k];
        this->m_hydraulicsBounce[k] = nullptr;
        delete this->m_hydraulicsControls[k];
        this->m_hydraulicsControls[k] = nullptr;
    }

    delete this->m_pPlayRapSheet;
    this->m_pPlayRapSheet = nullptr;
}

void EAXFrontEnd::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    this->m_pSFXOBJ_FEHUD = psfx;
}

void EAXFrontEnd::Initialize() {}

void EAXFrontEnd::Stop(eMenuSoundTriggers etrigger) {
    if (IsSoundEnabled != 0 && this->m_pPlayRapSheet && this->m_pPlayRapSheet->GetId() == etrigger) {
        delete this->m_pPlayRapSheet;
    }
}

int EAXFrontEnd::Play(eMenuSoundTriggers etrigger) {
    int nvol;
    int testID;

    if (IsSoundEnabled == 0) {
        return -1;
    }

    nvol = 0;
    if (!this->m_pSFXOBJ_FEHUD) {
        return nvol;
    }

    int adjusted;
    int pitch;

    nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL);
    g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);

    testID = static_cast<int>(etrigger);
    adjusted = testID + 23;
    if (static_cast<unsigned int>(testID - 80) < 16 && adjusted != 0x6E && adjusted != 0x6F) {
        pitch = 0x1000;

        delete this->m_pPlayRapSheet;
        this->m_pPlayRapSheet = new PlayFrontEndSample_RS(testID, nvol, pitch, 0);
    } else {
        int getref;

        this->m_pPlayFrontEndSampleHandle = new PlayFrontEndSample(testID, nvol, 0x1000, 0);
        getref = this->m_pPlayFrontEndSampleHandle->GetRefCount();
        delete this->m_pPlayFrontEndSampleHandle;
        this->m_pPlayFrontEndSampleHandle = nullptr;
    }

    nvol = 0;

    return nvol;
}

int EAXFrontEnd::Play(void *peventst) {
    if (IsSoundEnabled != 0 && this->m_pSFXOBJ_FEHUD) {
        if (!this->m_pSFXOBJ_FEHUD->GetOutputBlockPtr()) {
            return 0;
        }

        if (peventst) {
            PlayFrontEndSampleSt *pst = static_cast<PlayFrontEndSampleSt *>(peventst);
            int Vol = pst->volume;
            int nvol = Vol * this->m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL) >> 15;

            g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
            this->m_pPlayFrontEndSampleHandle = new PlayFrontEndSample(pst->id, nvol, pst->pitch, pst->azimuth);
            if (this->m_pPlayFrontEndSampleHandle) {
                delete this->m_pPlayFrontEndSampleHandle;
            }
            this->m_pPlayFrontEndSampleHandle = nullptr;
        }
    }

    return -1;
}

void EAXFrontEnd::Update(void *peventst) {
    this->UpdateDriveOn();
}

void *EAXFrontEnd::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXFrontEnd::UpdateDriveOn() {}

void EAXFrontEnd::SetFEDrivingCarState(bVector3 *, bVector3 *, Camera *, int) {}

void EAXFrontEnd::DestroyAllDriveOnSnds() {}

EAXCommon::EAXCommon() {
    this->mMsgMiscSound =
        Hermes::Handler::Create<MMiscSound, EAXCommon, EAXCommon>(this, &EAXCommon::MsgPlayMiscSound, "Snd", 0);
    this->m_pSFXOBJ_FEHUD = nullptr;
    this->m_pRadar = nullptr;
}

EAXCommon::~EAXCommon() {
    if (this->mMsgMiscSound) {
        Hermes::Handler::Destroy(this->mMsgMiscSound);
    }
}

void EAXCommon::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype != SFXOBJ_FEHUD) {
        return;
    }
    this->m_pSFXOBJ_FEHUD = psfx;
}

void EAXCommon::MsgPlayMiscSound(const MMiscSound &msg) {
    if (msg.GetSoundID() == 0) {
        if (this->m_pRadar) {
            delete this->m_pRadar;
        }

        g_pEAXSound->SetCsisName("FE Radar");
        this->m_pRadar = new FX_Radar(0, this->m_pSFXOBJ_FEHUD->GetDMixOutput(3, DMX_VOL), 0, 0, 0);
    }
}

void EAXCommon::Initialize() {}

void EAXCommon::Stop(eMenuSoundTriggers etrigger) {}

int EAXCommon::Play(eMenuSoundTriggers etrigger) {
    int nvol;

    if (!IsSoundEnabled) return -1;
    if (Debug_Common_FE_OFF) return -1;

    SndBase *snd = this->m_pSFXOBJ_FEHUD;
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
    this->m_pPlayCommonSampleHandle = new PlayCommonSample(static_cast<int>(etrigger), nvol, 0x1000, 0);
    if (this->m_pPlayCommonSampleHandle) {
        delete this->m_pPlayCommonSampleHandle;
    }
    this->m_pPlayCommonSampleHandle = nullptr;
    return 0;
}

int EAXCommon::Play(void *peventst) {
    if (IsSoundEnabled != 0 && this->m_pSFXOBJ_FEHUD) {
        if (!this->m_pSFXOBJ_FEHUD->GetOutputBlockPtr()) {
            return 0;
        }

        if (peventst) {
            PlayCommonSampleSt *pst = static_cast<PlayCommonSampleSt *>(peventst);
            int Vol = pst->volume;
            int nvol = Vol * this->m_pSFXOBJ_FEHUD->GetDMixOutput(1, DMX_VOL) >> 15;

            g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
            this->m_pPlayCommonSampleHandle = new PlayCommonSample(pst->id, nvol, pst->pitch, pst->azimuth);
            if (this->m_pPlayCommonSampleHandle) {
                delete this->m_pPlayCommonSampleHandle;
            }
            this->m_pPlayCommonSampleHandle = nullptr;
        }
    }

    return -1;
}

void EAXCommon::Update(void *peventst) {
    (void)peventst;

    if (this->m_pRadar) {
        int refCount = this->m_pRadar->GetRefCount();

        if (refCount < 2) {
            delete this->m_pRadar;
            this->m_pRadar = nullptr;
        }
    }

    if (this->m_pSFXOBJ_FEHUD) {
        this->m_pSFXOBJ_FEHUD->SetDMIX_Input(4, 0);
    }
}

void *EAXCommon::GetEventPointer(int neventindex) {
    return nullptr;
}
