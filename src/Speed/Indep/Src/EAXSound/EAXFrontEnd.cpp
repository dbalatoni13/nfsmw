#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Config.h"

int Debug_Common_FE_OFF = 0; // Decl: 38

static const int CommonUISFXVolume = 16383; // Decl: 41
static const int HARDCODED_FEVOL = 6553;    // Decl: 42

bool DEBUG_PRINT_FE_SND = false; // Decl: 44

// STRIPPED
char *GetFESoundName(eMenuSoundTriggers id) {
    return nullptr;
}

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

    for (int k = 0; k < NUM_ELEMENTS(this->m_hydraulicsControls); k++) {
        this->m_hydraulicsControls[k] = nullptr;
        this->m_hydraulicsBounce[k] = nullptr;
    }

    this->m_pPlayRapSheet = nullptr;
}

EAXFrontEnd::~EAXFrontEnd() {
    for (int k = 0; k < NUM_ELEMENTS(this->m_hydraulicsControls); k++) {
        delete this->m_hydraulicsControls[k];
        this->m_hydraulicsBounce[k] = nullptr;
        delete this->m_hydraulicsControls[k];
        this->m_hydraulicsControls[k] = nullptr;
    }

    delete this->m_pPlayRapSheet;
    this->m_pPlayRapSheet = nullptr;
}

void EAXFrontEnd::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype == SFXOBJ_FEHUD) {
        this->m_pSFXOBJ_FEHUD = psfx;
    }
}

void EAXFrontEnd::Initialize() {}

void EAXFrontEnd::Stop(eMenuSoundTriggers etrigger) {
    if (IsSoundEnabled != 0 && this->m_pPlayRapSheet != nullptr && this->m_pPlayRapSheet->GetId() == etrigger) {
        delete this->m_pPlayRapSheet;
    }
}

int EAXFrontEnd::Play(eMenuSoundTriggers etrigger) {
    if (IsSoundEnabled == 0) {
        return -1;
    }

    if (this->m_pSFXOBJ_FEHUD == nullptr) {
        return 0;
    }

    int nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL);
    g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);

    int testID = static_cast<int>(etrigger + UISND_COMMON_MAX_NUM);
    if (etrigger >= 80 && etrigger <= 95 && testID != UISND_RAPSHEET_MOVE_BAR_UP && testID != UISND_RAPSHEET_MOVE_BAR_DOWN) {

        delete this->m_pPlayRapSheet;
        this->m_pPlayRapSheet = new Csis::PlayFrontEndSample_RS(etrigger, nvol, 4096, 0);
    } else {
        this->m_pPlayFrontEndSampleHandle = new Csis::PlayFrontEndSample(etrigger, nvol, 4096, 0);
        int getref = this->m_pPlayFrontEndSampleHandle->GetRefCount();
        delete this->m_pPlayFrontEndSampleHandle;
        this->m_pPlayFrontEndSampleHandle = nullptr;
    }

    return 0;
}

int EAXFrontEnd::Play(void *peventst) {
    if (IsSoundEnabled == 0) {
        return -1;
    }
    if (this->m_pSFXOBJ_FEHUD == nullptr) {
        return -1;
    }
    if (this->m_pSFXOBJ_FEHUD->GetOutputPtr(0) == nullptr) {
        return 0;
    }

    if (peventst != nullptr) {
        PlayFrontEndSampleSt *pst = static_cast<PlayFrontEndSampleSt *>(peventst);
        int Vol = pst->volume;
        int nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(1, DMX_VOL);
        Vol = (Vol * nvol) >> 15;

        g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
        this->m_pPlayFrontEndSampleHandle = new Csis::PlayFrontEndSample(pst->id, Vol, pst->pitch, pst->azimuth);
        delete this->m_pPlayFrontEndSampleHandle;
        this->m_pPlayFrontEndSampleHandle = nullptr;
    }
    return -1;
}

void EAXFrontEnd::Update(void *peventst) {
    this->UpdateDriveOn();
}

// STRIPPED
void EAXFrontEnd::StopHydraulics(int iWheel) {}

// STRIPPED
void EAXFrontEnd::PlayBounce(int iWheel) {}

void *EAXFrontEnd::GetEventPointer(int neventindex) {
    return nullptr;
}

void EAXFrontEnd::UpdateDriveOn() {}

void EAXFrontEnd::SetFEDrivingCarState(bVector3 *car_position, bVector3 *car_velocity, Camera *camera, int view_id) {}

// STRIPPED
int GetDriveOnIndexOffsetForCarType(CarType eCarType) {}

// STRIPPED
void EAXFrontEnd::PlayDriveOnSnd(eMenuSoundTriggers etrigger, int view_id, CarType eCarType) {}

// STRIPPED
void EAXFrontEnd::EndDriveOnSnd(eMenuSoundTriggers etrigger, int view_id) {}

// STRIPPED
void EAXFrontEnd::DestroyDriveOn(DRIVE_ON_STATE SndIndex, DRIVE_ON_CAR_INDEX CarIndex) {}

void EAXFrontEnd::DestroyAllDriveOnSnds() {}

EAXCommon::EAXCommon() {
    this->mMsgMiscSound = Hermes::Handler::Create<MMiscSound, EAXCommon, EAXCommon>(this, &EAXCommon::MsgPlayMiscSound, UCrc32("Snd"), 0);
    this->m_pSFXOBJ_FEHUD = nullptr;
    this->m_pRadar = nullptr;
}

EAXCommon::~EAXCommon() {
    if (this->mMsgMiscSound != nullptr) {
        Hermes::Handler::Destroy(this->mMsgMiscSound);
    }
}

void EAXCommon::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (sfxtype == SFXOBJ_FEHUD) {
        this->m_pSFXOBJ_FEHUD = psfx;
    }
}

void EAXCommon::MsgPlayMiscSound(const MMiscSound &message) {
    if (message.GetSoundID() == 0) {
        delete this->m_pRadar;

        g_pEAXSound->SetCsisName("FE Radar");
        this->m_pRadar = new Csis::FX_Radar(0, this->m_pSFXOBJ_FEHUD->GetDMixOutput(3, DMX_VOL), 0, 0, 0);
    }
}

void EAXCommon::Initialize() {}

void EAXCommon::Stop(eMenuSoundTriggers etrigger) {}

// UNSOLVED
int EAXCommon::Play(eMenuSoundTriggers etrigger) {
    if (IsSoundEnabled == 0) {
        return -1;
    }
    if (Debug_Common_FE_OFF) {
        return -1;
    }

    if (this->m_pSFXOBJ_FEHUD == nullptr) {
        return -1;
    }
    if (this->m_pSFXOBJ_FEHUD->GetOutputPtr(0) == nullptr) {
        return 0;
    }

    int nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(1, DMX_VOL);
    extern float g_SliderValue;
    if (-1.0f < g_SliderValue) {
        int CurSliderVol = static_cast<int>(g_SliderValue * 32767.0f);
        CurSliderVol = bClamp(CurSliderVol, 0, 0x7FFF);

        g_SliderValue = -1.0f;
        nvol = (nvol * CurSliderVol) >> 15;
    }

    if (etrigger == UISND_ENTER_TRIGGER) {
        this->m_pSFXOBJ_FEHUD->SetDMIX_Input(4, 0x7FFF);
    }

    g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
    this->m_pPlayCommonSampleHandle = new Csis::PlayCommonSample(static_cast<int>(etrigger), nvol, 4096, 0);
    delete this->m_pPlayCommonSampleHandle;
    this->m_pPlayCommonSampleHandle = nullptr;

    return 0;
}

int EAXCommon::Play(void *peventst) {
    if (IsSoundEnabled == 0) {
        return -1;
    }
    if (this->m_pSFXOBJ_FEHUD == nullptr) {
        return -1;
    }

    if (this->m_pSFXOBJ_FEHUD->GetOutputBlockPtr() == nullptr) {
        return 0;
    }

    if (peventst != nullptr) {
        PlayCommonSampleSt *pst = static_cast<PlayCommonSampleSt *>(peventst);
        int Vol = pst->volume;
        int nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(1, DMX_VOL);
        Vol = (Vol * nvol) >> 15;

        g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
        this->m_pPlayCommonSampleHandle = new Csis::PlayCommonSample(pst->id, Vol, pst->pitch, pst->azimuth);
        delete this->m_pPlayCommonSampleHandle;
        this->m_pPlayCommonSampleHandle = nullptr;
    }

    return -1;
}

float g_fTableTurnSpeed = 1.0f; // Decl: 377

Slope TablePitch(0.0f, 1.0f, 3500.0f, 4500.0f); // Decl: 379

void EAXCommon::Update(void *peventst) {
    if (this->m_pRadar != nullptr && this->m_pRadar->GetRefCount() < 2) {
        delete this->m_pRadar;
        this->m_pRadar = nullptr;
    }

    if (this->m_pSFXOBJ_FEHUD != nullptr) {
        this->m_pSFXOBJ_FEHUD->SetDMIX_Input(4, 0);
    }
}

void *EAXCommon::GetEventPointer(int neventindex) {
    return nullptr;
}

// STRIPPED
void EAXCommon::Reset() {}
