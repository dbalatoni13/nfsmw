#include "./CARSFX_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/UG/SndDataParams.hpp"
#include <snd/sndo.h>

int DISPLAY_VOLUMES_MIX = 0;                   // Decl: 30
int SPEW_ENGINE_INFO = 0;                      // Decl: 31
static const int DEBUG_ENGINE_CREATE_INFO = 0; // Decl: 32
static const int ENG_LPF_CUTOFF = 0;           // Decl: 33
static const int ENG_LPF_RND = 11000;          // Decl: 34
static const int ENG_LPF_TRIG = 0;             // Decl: 35

static const float TRANNY_SCALER = 15.0f; // Decl: 37
int PLAYBACK_ENGINE_INFO = 0;             // Decl: 38
int PLAYBACK_USING_ENGINE_PITCH = 0;      // Decl: 39
static const int GinsuAccelVol = 32767;   // TODO use Decl: 40

static const int AEMSVol = 32767; // TODO use Decl: 47

#define REC_TIME 0 // Decl: 50
#define REC_VOL 1  // Decl: 51
#define REC_RPM 2  // Decl: 52
#define REC_TRQ 3  // Decl: 53

unsigned int DataToPlayBack[1][4] = {{0, 1, 1, 1}}; // Decl: 56

int PRINT_AI_ENGINE_INFO;                 // Decl: 71
static const int SPEW_AI_ENGINE_INFO = 0; // Decl: 72

CARSFX_EngineBase::CARSFX_EngineBase() {
    this->PitchMultipli = 0.0f;
    this->m_pcsisCarCtrl = nullptr;
    this->m_pTranny = nullptr;
    this->m_pEngineCtl = nullptr;
    this->m_p3DCarPosCtl = nullptr;
    this->SPU_or_EE = 0;
}

CARSFX_EngineBase::~CARSFX_EngineBase() {
    if (this->m_pcsisCarCtrl != nullptr) {
        delete this->m_pcsisCarCtrl;
    }

    if (this->m_pTranny != nullptr) {
        delete this->m_pTranny;
    }
}

void CARSFX_EngineBase::Detach() {
    if (this->m_pcsisCarCtrl != nullptr) {
        delete this->m_pcsisCarCtrl;
        this->m_pcsisCarCtrl = nullptr;
    }
    if (this->m_pTranny != nullptr) {
        delete this->m_pTranny;
        this->m_pTranny = nullptr;
    }
}

void CARSFX_EngineBase::UpdateParams(float t) {
    SndBase::UpdateParams(t);
}

void CARSFX_EngineBase::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_EngineBase::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_EngineBase::Destroy() {
    SndBase::Destroy();
}

void CARSFX_EngineBase::ProcessUpdate() {
    SetEngineParams();
}

int ntestrefcount = 0; // Decl: 181

void CARSFX_EngineBase::InitializeEngine() {}

void CARSFX_EngineBase::SetEngineParams() {}

DEFINE_CREATABLE(0x20000, CARSFX_AEMSEngine, SndBase);

CARSFX_AEMSEngine::CARSFX_AEMSEngine() : CARSFX_EngineBase() {
    static int AI_ENGINE_PITCH_OFFSET = 0;

    AI_ENGINE_PITCH_OFFSET = (AI_ENGINE_PITCH_OFFSET + 500) % 3000;
    this->m_iAIPitchOffset = AI_ENGINE_PITCH_OFFSET - 1500;
}

CARSFX_AEMSEngine::~CARSFX_AEMSEngine() {}

void CARSFX_AEMSEngine::UpdateParams(float t) {
    CARSFX_EngineBase::UpdateParams(t);
}

void CARSFX_AEMSEngine::SetupSFX(CSTATE_Base *_StateBase) {
    CARSFX_EngineBase::SetupSFX(_StateBase);
}

// UNSOLVED, probably the inlines
void CARSFX_AEMSEngine::InitSFX() {
    CARSFX_EngineBase::InitSFX();
    g_pEAXSound->SetCsisName(this);
    this->m_pcsisCarCtrl =
        new Csis::CAR(m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, this->SPU_or_EE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    ntestrefcount = this->m_pcsisCarCtrl != nullptr ? this->m_pcsisCarCtrl->GetRefCount() : 0;
}

int CARSFX_AEMSEngine::GetController(int Index) {
    switch (Index) {
        case 0:
            return 4;
        case 1:
            return 7;
        default:
            return -1;
    }
}

void CARSFX_AEMSEngine::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 4:
            this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
            break;
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
    }
}

// TODO remove fake vars
void CARSFX_AEMSEngine::SetEngineParams() {
    float FAKE = (this->m_pEngineCtl->GetEngRPM() - 1000.0f);
    float FAKE2 = (this->m_pEAXCar->GetAttributes().MaxRPM() - this->m_pEAXCar->GetAttributes().MinRPM()) / 9000.0f;
    float ScaledRPM = FAKE * FAKE2 + this->m_pEAXCar->GetAttributes().MinRPM();

    if (0) {
        int testrefcount = this->m_pcsisCarCtrl->GetRefCount();
    }

    if (this->m_pcsisCarCtrl != nullptr) {
        int nDMixOut;
        int TmpVol;
        int CurPitch = this->m_iAIPitchOffset + 16000;

        nDMixOut = this->GetDMixOutput(4, DMX_PITCH);
        this->PitchMultipli = nDMixOut / 4096.0f;
        CurPitch = static_cast<int>(static_cast<float>(CurPitch) * this->PitchMultipli - 16000.0f);

        nDMixOut = this->GetDMixOutput(1, DMX_VOL);
        this->SetDMIX_Input(1, nDMixOut);

        TmpVol = (this->m_pEngineCtl->m_iEngineVol * nDMixOut) >> 15;
        TmpVol = (TmpVol * 0x7FFF) >> 15;

        this->m_pcsisCarCtrl->SetVOL_ENG(TmpVol - 0x7FFF);
        this->m_pcsisCarCtrl->SetVOL_EXH(TmpVol - 0x7FFF);
        this->m_pcsisCarCtrl->SetRPM(static_cast<int>(ScaledRPM));
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_PITCH_OFFSET(CurPitch);
        this->m_pcsisCarCtrl->SetTORQUE(static_cast<int>(this->m_pEngineCtl->GetEngTorque() * 10.24f));
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_ROTATION(this->m_pEngineCtl->m_Rotation);
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_AZIMUTH(this->GetDMixOutput(0, DMX_AZIM));
        this->m_pcsisCarCtrl->CommitMemberData();
    }

    CARSFX_EngineBase::SetEngineParams();
}

int START_RECORD_GINSU_ENGINE_INFO = 0; // Decl: 433
int END_RECORD_GINSU_ENGINE_INFO = 0;   // Decl: 434
static const int GINSU_LATENCY_MS = 60; // Decl: 435

CARSFX_GinsuEngine::CARSFX_GinsuEngine() : m_pHybridEngCtl(nullptr) {
    this->m_pEngineCtl = nullptr;
    this->GinsuInitialized = false;
    this->InitCnt = 0;
    this->m_pShiftingCtl = nullptr;
}

CARSFX_GinsuEngine::~CARSFX_GinsuEngine() {
    int Index = 0;

    while ((this->m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE && Index < 1) || (this->m_pEAXCar->m_EngineType == eGINSU_ENG_DUAL && Index < 2)) {
        stGinsuData *pData = &m_GinsuData[Index];

        if (pData->mSynth != nullptr) {
            pData->mSynth->StopSynthesis();
            delete pData->mSynth;
        }

        pData->mSynth = nullptr;

        if (pData->mSynthData != nullptr) {
            delete pData->mSynthData;
        }

        pData->mSynthData = nullptr;

        if (pData->mSynthBlock != nullptr) {
            gAudioMemoryManager.FreeMemory(pData->mSynthBlock);
        }

        pData->mSynthBlock = nullptr;
        Index++;
    }
}

int CARSFX_GinsuEngine::GetController(int Index) {
    switch (Index) {
        case 0:
            return 4;
        case 1:
            return 5;
        case 2:
            return 6;
        case 3:
            return 7;
        case 4:
            return 2;
        default:
            return -1;
    }
}

void CARSFX_GinsuEngine::AttachController(SFXCTL *psfxctl) {
    int nindex = psfxctl->GetObjectIndex();
    switch (nindex) {
        case 4:
            this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
            break;
        case 5:
            this->m_pHybridEngCtl = static_cast<SFXCTL_HybridMotor *>(psfxctl);
            break;
        case 6:
            this->m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
            break;
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
        case 2:
            this->m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
    }
}

void CARSFX_GinsuEngine::SetupSFX(CSTATE_Base *_StateBase) {
    CARSFX_EngineBase::SetupSFX(_StateBase);
}

void CARSFX_GinsuEngine::InitSFX() {
    CARSFX_EngineBase::InitSFX();
    this->InitializeEngine();
    this->Enable();
}

void CARSFX_GinsuEngine::Detach() {
    int Index = 0;

    while ((m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE && Index < 1) || (m_pEAXCar->m_EngineType == eGINSU_ENG_DUAL && Index < 2)) {
        stGinsuData *pData = &m_GinsuData[Index];

        if (pData->mSynth != nullptr) {
            SNDSYS_entercritical();
            SNDvol(m_GinsuData[Index].mSNDhandle, 0);
            SNDSYS_leavecritical();
        }
        Index++;
    }
    CARSFX_EngineBase::Detach();
}

char *GetGinsuData(const char *filename);

void CARSFX_GinsuEngine::StartupGinsu() {
    int Index = 0;
    while ((this->m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE && Index <= 0) || (this->m_pEAXCar->m_EngineType == eGINSU_ENG_DUAL && Index <= 1)) {
        {
            const char *filename = nullptr;
            if (Index == 0) {
                filename = this->m_pEAXCar->mEngineInfo.Filename_GinsuAccel().GetString();
            } else if (Index == 1) {
                filename = this->m_pEAXCar->mEngineInfo.Filename_GinsuDecel().GetString();
            }

            char *filedata = GetGinsuData(filename);
            if (filedata == nullptr) {
                return;
            }

            stGinsuData *pData = &m_GinsuData[Index];
            if (pData->mSynthData == nullptr) {
                pData->mSynthData = new ("AUD: Ginsu synth data") GinsuSynthData();
            }

            int blocksize = GinsuSynthesis::GetMemblockSize();
            if (pData->mSynthBlock == nullptr) {
                pData->mSynthBlock = gAudioMemoryManager.AllocateMemory(blocksize, "AUD: Ginsu synth workspace", false);
            }
            if (pData->mSynth == nullptr) {
                pData->mSynth = new ("AUD: Ginsu synth object") GinsuSynthesis(pData->mSynthBlock, blocksize);
            }

            bool bindOK = pData->mSynthData->BindToData(filedata);
            if (bindOK) {
                pData->mSynth->SetSynthData(*pData->mSynthData);
                pData->mMaxFrequency = pData->mSynthData->GetMaxFrequency();
                pData->mMinFrequency = pData->mSynthData->GetMinFrequency();
                if (pData->mSNDhandle < 0) {
                    pData->mSNDhandle = pData->mSynth->StartSynthesis(pData->mMinFrequency);
                }
            }
        }
        Index++;
    }
    this->GinsuInitialized = true;
}

stSndDataLoadParams g_SndAssetList[MAX_SIZE_SNDASSETLIST]; // size: 0x1380, Decl: 953
char *GetGinsuData(const char *filename) {
    int index = gAEMSMgr.IsAssetInList(Attrib::StringKey(filename));
    if (index == -1) {
        return nullptr;
    }
    if (g_SndAssetList[index].mBankSlot != nullptr) {
        return g_SndAssetList[index].mBankSlot->MAINmemLocation;
    }
    return static_cast<char *>(g_SndAssetList[index].pmem);
}

void CARSFX_GinsuEngine::InitializeEngine() {
    this->StartupGinsu();
    int UseEE = this->SPU_or_EE;

    this->m_pcsisCarCtrl =
        new Csis::CAR(this->m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, UseEE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    this->m_refCount = static_cast<unsigned short>(this->m_pcsisCarCtrl->GetRefCount());
    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_PLAYER && this->m_pEAXCar->GetAttributes().Tranny()) {
        this->m_pTranny = new Csis::CAR_TRANNY(m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        int ref = this->m_pTranny->GetRefCount();
    }
}

void CARSFX_GinsuEngine::SetEngineParams() {
    if (!this->IsEnabled()) {
        return;
    }
    this->m_GinsuRPM = this->m_pHybridEngCtl->m_GinsuScaledRPM;
    this->PitchMultipli = static_cast<float>(this->GetDMixOutput(4, DMX_PITCH)) / 4096.0f;
    this->SetGinsuParams();

    if (this->m_pcsisCarCtrl != nullptr) {
        int nDMixOut = this->GetDMixOutput(1, DMX_VOL);
        int TmpVol = (this->m_pHybridEngCtl->m_EngVolAEMS * nDMixOut) >> 15;
        TmpVol = (TmpVol * 0x7FFF) >> 15;

        int nFXDryVol;
        int nFXWetVol;
        this->m_pcsisCarCtrl->SetVOL_ENG(TmpVol - 0x7FFF);
        this->m_pcsisCarCtrl->SetVOL_EXH(TmpVol - 0x7FFF);

        int TmpRpmVol = this->m_pHybridEngCtl->m_EngVolRedLine * nDMixOut >> 15;
        this->m_pcsisCarCtrl->SetMAX_RPM(TmpRpmVol);

        this->m_pcsisCarCtrl->SetRPM(static_cast<int>(this->m_GinsuRPM));
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_PITCH_OFFSET(static_cast<int>(this->PitchMultipli * 16383.0f) - 0x3FFF);
        this->m_pcsisCarCtrl->SetTORQUE(static_cast<int>(this->m_pEngineCtl->GetEngTorque() * 10.24f));
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_ROTATION(this->m_pEngineCtl->m_Rotation);
        this->m_pcsisCarCtrl->SetCOMMON_PARAMETERS_AZIMUTH(this->GetDMixOutput(0, DMX_AZIM));
        this->m_pcsisCarCtrl->CommitMemberData();
    }

    if (this->m_pTranny == nullptr) {
        return;
    }

    this->m_pTranny->SetMagnitude(static_cast<int>(this->GetPhysCar()->GetForwardSpeed() * TRANNY_SCALER));
    this->m_pTranny->SetVOL(this->GetDMixOutput(3, DMX_VOL));
    this->m_pTranny->SetAZIMUTH(this->GetDMixOutput(0, DMX_AZIM));
    this->m_pTranny->SetPITCH_OFFSET(this->GetDMixOutput(4, DMX_PITCH));
    this->m_pTranny->SetREVERB_AND_FILTERS_LoPass(this->GetDMixOutput(5, DMX_FREQ));
    this->m_pTranny->SetREVERB_AND_FILTERS_Wet(this->m_pTunnelCtl->m_AEMSWetVol);
    this->m_pTranny->SetREVERB_AND_FILTERS_Dry(this->m_pTunnelCtl->m_AEMSDryVol);

    if (this->m_pShiftingCtl != nullptr && this->m_pShiftingCtl->IsActive()) {
        this->m_pTranny->SetSingle_Shot(1);
    } else {
        this->m_pTranny->SetSingle_Shot(0);
    }

    this->m_pTranny->CommitMemberData();
}

DEFINE_CREATABLE(0x20010, CARSFX_SingleGinsuEng, SndBase);

CARSFX_SingleGinsuEng::CARSFX_SingleGinsuEng() {}

CARSFX_SingleGinsuEng::~CARSFX_SingleGinsuEng() {}

void CARSFX_SingleGinsuEng::SetGinsuParams() {
    if (!this->GinsuInitialized) {
        return;
    }

    int nDMixOut = this->GetDMixOutput(2, DMX_VOL);
    int TmpVol = (this->m_pHybridEngCtl->m_EngVolAccelGinsu * nDMixOut) >> 15;
    float freq = this->m_GinsuRPM;
    float fPitchBelowMinFreq = 1.0f;

    if (this->m_GinsuRPM < this->m_GinsuData[0].mMinFrequency) {
        freq = this->m_GinsuData[0].mMinFrequency;
        fPitchBelowMinFreq = this->m_GinsuRPM / freq;
    }

    int nFXDryVol = this->m_pTunnelCtl->m_GinsuDryVol >> 8;
    nFXDryVol = nFXDryVol >= 0 ? (nFXDryVol > 0x7F ? 0x7F : nFXDryVol) : 0;

    this->m_GinsuAccelVol = TmpVol >> 8;
    this->m_GinsuAccelVol = this->m_GinsuAccelVol >= 0 ? (this->m_GinsuAccelVol > 0x7F ? 0x7F : this->m_GinsuAccelVol) : 0;

    int nFXWetVol = (this->m_pTunnelCtl->m_GinsuWetVol * nDMixOut) >> 23;
    nFXWetVol = nFXWetVol >= 0 ? (nFXWetVol > 0x7F ? 0x7F : nFXWetVol) : 0;

    SNDSYS_entercritical();
    SNDvol(this->m_GinsuData[0].mSNDhandle, this->m_GinsuAccelVol);

    SND3dpos(this->m_GinsuData[0].mSNDhandle, this->GetDMixOutput(0, DMX_AZIM), 0);
    SNDpitchmult(this->m_GinsuData[0].mSNDhandle, static_cast<int>(fPitchBelowMinFreq * this->PitchMultipli * 4096.0f));
    SNDCTRL_lowpass(this->m_GinsuData[0].mSNDhandle, static_cast<int>(this->m_pHybridEngCtl->m_GinsuLPFVal));
    SNDCTRL_drylevel(this->m_GinsuData[0].mSNDhandle, nFXDryVol);
    SNDfxlevel(this->m_GinsuData[0].mSNDhandle, 0, nFXWetVol);
    SNDSYS_leavecritical();

    this->m_GinsuData[0].mSynth->UpdateFrequency(freq, 60.0f);
    this->m_GinsuRPM = this->m_GinsuData[0].mSynth->GetCurrentPitch() * fPitchBelowMinFreq * 120.0f;
}

DEFINE_CREATABLE(0x20020, CARSFX_DualGinsuEng, SndBase);

CARSFX_DualGinsuEng::CARSFX_DualGinsuEng() {}

CARSFX_DualGinsuEng::~CARSFX_DualGinsuEng() {}

void CARSFX_DualGinsuEng::SetGinsuParams() {
    if (!this->GinsuInitialized) {
        return;
    } else {
        int nDMixOut = this->GetDMixOutput(2, DMX_VOL);
        int TmpVol = (this->m_pHybridEngCtl->m_EngVolAccelGinsu * nDMixOut) >> 15;
        int FAKE = static_cast<int>(this->m_pHybridEngCtl->m_GinsuLPFVal);
        int LowPassFilter = bMin(this->GetDMixOutput(5, DMX_FREQ), FAKE);

        float freq = this->m_GinsuRPM;
        float fPitchBelowMinFreq = 1.0f;

        if (this->m_GinsuRPM < this->m_GinsuData[0].mMinFrequency) {
            freq = this->m_GinsuData[0].mMinFrequency;
            fPitchBelowMinFreq = this->m_GinsuRPM / freq;
        }

        int nFXDryVol = this->m_pTunnelCtl->m_GinsuDryVol >> 8;
        nFXDryVol = nFXDryVol >= 0 ? (nFXDryVol > 0x7F ? 0x7F : nFXDryVol) : 0;

        this->m_GinsuAccelVol = TmpVol >> 8;
        this->m_GinsuAccelVol = this->m_GinsuAccelVol >= 0 ? (this->m_GinsuAccelVol > 0x7F ? 0x7F : this->m_GinsuAccelVol) : 0;

        int nFXWetVol = this->m_pTunnelCtl->m_GinsuWetVol * nDMixOut >> 23;
        nFXWetVol = nFXWetVol >= 0 ? (nFXWetVol > 0x7F ? 0x7F : nFXWetVol) : 0;

        SNDSYS_entercritical();
        SNDvol(this->m_GinsuData[0].mSNDhandle, this->m_GinsuAccelVol);
        SND3dpos(this->m_GinsuData[0].mSNDhandle, this->GetDMixOutput(0, DMX_AZIM), 0);
        SNDpitchmult(this->m_GinsuData[0].mSNDhandle, static_cast<int>(fPitchBelowMinFreq * this->PitchMultipli * 4096.0f));
        SNDCTRL_lowpass(this->m_GinsuData[0].mSNDhandle, LowPassFilter);
        SNDCTRL_drylevel(this->m_GinsuData[0].mSNDhandle, nFXDryVol);
        SNDfxlevel(this->m_GinsuData[0].mSNDhandle, 0, nFXWetVol);
        this->m_GinsuData[0].mSynth->UpdateFrequency(freq, 60.0f);
        this->m_GinsuRPM = this->m_GinsuData[0].mSynth->GetCurrentPitch() * fPitchBelowMinFreq * 120.0f;

        nFXDryVol = this->m_pTunnelCtl->m_GinsuDryVol >> 8;
        TmpVol = this->m_pHybridEngCtl->m_EngVolDecelGinsu * nDMixOut >> 15;
        nFXDryVol = nFXDryVol >= 0 ? (nFXDryVol > 0x7F ? 0x7F : nFXDryVol) : 0;

        this->m_GinsuDecelVol = TmpVol >> 8;
        this->m_GinsuDecelVol = this->m_GinsuDecelVol >= 0 ? (this->m_GinsuDecelVol > 0x7F ? 0x7F : this->m_GinsuDecelVol) : 0;

        nFXWetVol = nDMixOut * this->m_pTunnelCtl->m_GinsuWetVol >> 23;
        nFXWetVol = nFXWetVol >= 0 ? (nFXWetVol > 0x7F ? 0x7F : nFXWetVol) : 0;

        SNDvol(this->m_GinsuData[1].mSNDhandle, this->m_GinsuDecelVol);
        SND3dpos(this->m_GinsuData[1].mSNDhandle, this->GetDMixOutput(0, DMX_AZIM), 0);
        SNDpitchmult(this->m_GinsuData[1].mSNDhandle, static_cast<int>(fPitchBelowMinFreq * this->PitchMultipli * 4096.0f));
        SNDCTRL_lowpass(this->m_GinsuData[1].mSNDhandle, LowPassFilter);
        SNDCTRL_drylevel(this->m_GinsuData[1].mSNDhandle, nFXDryVol);
        SNDfxlevel(this->m_GinsuData[1].mSNDhandle, 0, nFXWetVol);
        this->m_GinsuData[1].mSynth->UpdateFrequency(freq, 60.0f);
        SNDSYS_leavecritical();
    }
}
