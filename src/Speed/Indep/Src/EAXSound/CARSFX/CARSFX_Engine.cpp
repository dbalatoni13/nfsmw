#include "./CARSFX_Engine.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

extern "C" int SNDvol(int sndHandle, int vol);
extern stSndDataLoadParams g_SndAssetList[];
char *GetGinsuData(const char *filename);
int ntestrefcount;
static int AI_ENGINE_PITCH_OFFSET;

void CARSFX_EngineBase::UpdateParams(float) {}

void CARSFX_EngineBase::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_EngineBase::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_EngineBase::Destroy() {}

void CARSFX_EngineBase::Detach() {
    if (m_pcsisCarCtrl) {
        delete m_pcsisCarCtrl;
        m_pcsisCarCtrl = nullptr;
    }
    if (m_pTranny) {
        delete m_pTranny;
        m_pTranny = nullptr;
    }
}

void CARSFX_EngineBase::ProcessUpdate() {
    SetEngineParams();
}

void CARSFX_EngineBase::InitializeEngine() {}

void CARSFX_EngineBase::SetEngineParams() {}

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
    int nindex;

    nindex = psfxctl->GetObjectIndex();
    switch (nindex) {
    case 4:
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 5:
        m_pHybridEngCtl = static_cast<SFXCTL_HybridMotor *>(psfxctl);
        break;
    case 6:
        m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    case 2:
        m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    }
}

void CARSFX_GinsuEngine::SetupSFX(CSTATE_Base *_StateBase) {
    CARSFX_EngineBase::SetupSFX(_StateBase);
}

void CARSFX_GinsuEngine::InitSFX() {
    CARSFX_EngineBase::InitSFX();
    InitializeEngine();
    Enable();
}

void CARSFX_GinsuEngine::Detach() {
    int Index;

    for (Index = 0;
         (m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE && Index < 1) ||
             (m_pEAXCar->m_EngineType == eGINSU_ENG_DUAL && Index < 2);
         Index++) {
        stGinsuData *pData = &m_GinsuData[Index];

        if (pData->mSynth) {
            SNDSYS_entercritical();
            SNDvol(m_GinsuData[Index].mSNDhandle, 0);
            SNDSYS_leavecritical();
        }
    }
    CARSFX_EngineBase::Detach();
}

void CARSFX_GinsuEngine::InitializeEngine() {
    int UseEE;

    StartupGinsu();
    UseEE = SPU_or_EE;
    m_pcsisCarCtrl = new CAR(m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, UseEE, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0);
    m_refCount = static_cast<unsigned short>(m_pcsisCarCtrl->GetRefCount());
    if (GetPhysCar()->GetContext() == Sound::kRaceContext_QuickRace && m_pEAXCar->GetAttributes().Tranny()) {
        int ref;

        m_pTranny = new CAR_TRANNY(m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        ref = m_pTranny->GetRefCount();
    }
}

void CARSFX_GinsuEngine::StartupGinsu() {
    int Index;

    Index = 0;
    while ((m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE && Index <= 0) ||
           (m_pEAXCar->m_EngineType == eGINSU_ENG_DUAL && Index <= 1)) {
        {
            const char *filename;
            char *filedata;
            stGinsuData *pData;
            int blocksize;
            bool bindOK;

            filename = nullptr;
            if (Index == 0) {
                filename = m_pEAXCar->mEngineInfo.Filename_GinsuAccel().GetString();
                if (!filename) {
                    filename = "";
                }
            } else if (Index == 1) {
                filename = m_pEAXCar->mEngineInfo.Filename_GinsuDecel().GetString();
                if (!filename) {
                    filename = "";
                }
            }
            filedata = GetGinsuData(filename);
            if (!filedata) {
                return;
            }
            pData = &m_GinsuData[Index];
            if (!pData->mSynthData) {
                pData->mSynthData = new ("AUD: Ginsu synth data") GinsuSynthData();
            }
            blocksize = GinsuSynthesis::GetMemblockSize();
            if (!pData->mSynthBlock) {
                pData->mSynthBlock = gAudioMemoryManager.AllocateMemory(blocksize, "AUD: Ginsu synth workspace", false);
            }
            if (!pData->mSynth) {
                pData->mSynth = new ("AUD: Ginsu synth object") GinsuSynthesis(pData->mSynthBlock, blocksize);
            }
            bindOK = pData->mSynthData->BindToData(filedata);
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
    GinsuInitialized = true;
}

char *GetGinsuData(const char *filename) {
    int index;

    index = gAEMSMgr.IsAssetInList(Attrib::StringKey(filename));
    if (index == -1) {
        return nullptr;
    }
    if (g_SndAssetList[index].mBankSlot) {
        return g_SndAssetList[index].mBankSlot->MAINmemLocation;
    }
    return static_cast<char *>(g_SndAssetList[index].pmem);
}

void CARSFX_GinsuEngine::SetEngineParams() {
    int nDMixOut;
    int TmpVol;
    int nFXDryVol;
    int nFXWetVol;
    int TmpRpmVol;

    if (!IsEnabled()) {
        return;
    }

    m_GinsuRPM = m_pHybridEngCtl->m_GinsuScaledRPM;
    nDMixOut = GetDMixOutput(4, DMX_PITCH);
    PitchMultipli = static_cast<float>(nDMixOut) * 0.000244140625f;
    SetGinsuParams();

    if (m_pcsisCarCtrl) {
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        TmpVol = (m_pHybridEngCtl->m_EngVolAEMS * nDMixOut >> 15) * 0x7FFF >> 15;
        m_pcsisCarCtrl->SetVOL_ENG(TmpVol - 0x7FFF);
        m_pcsisCarCtrl->SetVOL_EXH(TmpVol - 0x7FFF);
        TmpRpmVol = m_pHybridEngCtl->m_EngVolRedLine * nDMixOut >> 15;
        m_pcsisCarCtrl->SetMAX_RPM(TmpRpmVol);
        m_pcsisCarCtrl->SetRPM(static_cast<int>(m_GinsuRPM));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_PITCH_OFFSET(static_cast<int>(PitchMultipli * 16383.0f) - 0x3FFF);
        m_pcsisCarCtrl->SetTORQUE(static_cast<int>(m_pEngineCtl->GetEngTorque() * 10.24f));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_ROTATION(m_pEngineCtl->m_Rotation);
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_AZIMUTH(GetDMixOutput(0, DMX_AZIM));
        m_pcsisCarCtrl->CommitMemberData();
    }

    if (!m_pTranny) {
        return;
    }

    m_pTranny->SetMagnitude(static_cast<int>(GetPhysCar()->GetForwardSpeed() * 15.0f));
    m_pTranny->SetVOL(GetDMixOutput(3, DMX_VOL));
    m_pTranny->SetAZIMUTH(GetDMixOutput(0, DMX_AZIM));
    m_pTranny->SetPITCH_OFFSET(GetDMixOutput(4, DMX_PITCH));
    m_pTranny->SetREVERB_AND_FILTERS_LoPass(GetDMixOutput(5, DMX_FREQ));
    nFXWetVol = m_pTunnelCtl->m_AEMSWetVol;
    m_pTranny->SetREVERB_AND_FILTERS_Wet(nFXWetVol);
    nFXDryVol = m_pTunnelCtl->m_AEMSDryVol;
    m_pTranny->SetREVERB_AND_FILTERS_Dry(nFXDryVol);
    if (m_pShiftingCtl && m_pShiftingCtl->IsActive()) {
        m_pTranny->SetSingle_Shot(1);
    } else {
        m_pTranny->SetSingle_Shot(0);
    }
    m_pTranny->CommitMemberData();
}

SndBase::TypeInfo CARSFX_AEMSEngine::s_TypeInfo = { 0x00020000, "CARSFX_AEMSEngine", &SndBase::s_TypeInfo, CARSFX_AEMSEngine::CreateObject };

SndBase::TypeInfo *CARSFX_AEMSEngine::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_AEMSEngine::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_AEMSEngine::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_AEMSEngine();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_AEMSEngine();
}

CARSFX_AEMSEngine::CARSFX_AEMSEngine()
    : CARSFX_EngineBase() {
    AI_ENGINE_PITCH_OFFSET = (AI_ENGINE_PITCH_OFFSET + 500) % 3000;
    m_iAIPitchOffset = AI_ENGINE_PITCH_OFFSET - 1500;
}

CARSFX_AEMSEngine::~CARSFX_AEMSEngine() {}

void CARSFX_AEMSEngine::UpdateParams(float t) {
    CARSFX_EngineBase::UpdateParams(t);
}

void CARSFX_AEMSEngine::SetupSFX(CSTATE_Base *_StateBase) {
    CARSFX_EngineBase::SetupSFX(_StateBase);
}

void CARSFX_AEMSEngine::InitSFX() {
    int refCount;

    CARSFX_EngineBase::InitSFX();
    g_pEAXSound->SetCsisName(this);
    m_pcsisCarCtrl = new CAR(m_pEAXCar->GetAttributes().CarID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SPU_or_EE, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0);
    refCount = 0;
    if (m_pcsisCarCtrl) {
        refCount = m_pcsisCarCtrl->GetRefCount();
    }
    ntestrefcount = refCount;
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
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void CARSFX_AEMSEngine::SetEngineParams() {
    float ScaledRPM;
    int nDMixOut;
    int TmpVol;
    int CurPitch;

    ScaledRPM = (m_pEngineCtl->GetEngRPM() - 1000.0f) *
                    ((m_pEAXCar->GetAttributes().MaxRPM() - m_pEAXCar->GetAttributes().MinRPM()) * 0.00011111111f) +
                m_pEAXCar->GetAttributes().MinRPM();
    if (m_pcsisCarCtrl) {
        CurPitch = m_iAIPitchOffset + 16000;
        nDMixOut = GetDMixOutput(4, DMX_PITCH);
        PitchMultipli = static_cast<float>(nDMixOut) * 0.000244140625f;
        CurPitch = static_cast<int>(static_cast<float>(CurPitch) * PitchMultipli - 16000.0f);
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        SetDMIX_Input(1, nDMixOut);
        TmpVol = (m_pEngineCtl->m_iEngineVol * nDMixOut >> 15) * 0x7FFF >> 15;
        m_pcsisCarCtrl->SetVOL_ENG(TmpVol - 0x7FFF);
        m_pcsisCarCtrl->SetVOL_EXH(TmpVol - 0x7FFF);
        m_pcsisCarCtrl->SetRPM(static_cast<int>(ScaledRPM));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_PITCH_OFFSET(CurPitch);
        m_pcsisCarCtrl->SetTORQUE(static_cast<int>(m_pEngineCtl->GetEngTorque() * 10.24f));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_ROTATION(m_pEngineCtl->m_Rotation);
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_AZIMUTH(GetDMixOutput(0, DMX_AZIM));
        m_pcsisCarCtrl->CommitMemberData();
    }
    CARSFX_EngineBase::SetEngineParams();
}

void CARSFX_AEMSEngine::SetupLoadData() {
    const char *auxBankName;
    eBANK_SLOT_TYPE type;

    auxBankName = m_pEAXCar->GetEngineAttributes().BankName_auxRAM(0).GetString();
    if (!auxBankName) {
        auxBankName = "";
    }
    if (auxBankName != "") {
        SPU_or_EE = 0;
        type = eBANK_SLOT_NONE;
        if (GetPhysCar()->GetContext() == Sound::CONTEXT_AIRACER || GetPhysCar()->GetContext() == Sound::CONTEXT_COP) {
            type = eBANK_SLOT_AI_AEMS_ENGINE;
        }
        SPU_or_EE = 1;
        LoadAsset(m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM, type, true);
    }
}
