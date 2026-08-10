#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"

DEFINE_CREATABLE(0x20100, SFXObj_Reverb, SndBase);

// size: 0x30, address: 0x80418214, Decl: 45
eREVERBFX ReverbZoneCrossMap[12] = {
    RVRB_CARSHOW_SML,       RVRB_CARSHOW_BASS, RVRB_CARSHOW_BASS_2, RVRB_GARAGE,   RVRB_GARAGE_SML, RVRB_SIMPLE_TUNNEL,
    RVRB_SIMPLE_TUNNEL_SML, RVRB_HILLS_CLOSE,  RVRB_HILLS,          RVRB_CITYOPEN, RVRB_CITYDENSE,  RVRB_ALLEY,
};

// size: 0x30, address: 0x80418244, Decl: 61
char *csfxedit[12] = {
    "City_Dense.fx",        "Alley.fx",       "City_Open.fx", "Garage.fx",    "Garage_sml.fx", "Simple_Tunnel.fx",
    "Simple_Tunnel_Sml.fx", "Hills_Close.fx", "Hills.fx",     "City_Open.fx", "City_Dense.fx", "Alley.fx",
};

ParameterAccessor ReverbAccessor("Reverb"); // size: 0x1C, address: 0x8045E564, Decl: 25

Snd::GlobalFxProcessor *SFXObj_Reverb::m_pFXEditModule[2]; // Decl: 79

char *SFXObj_Reverb::m_pFXEditPatch[12]; // Decl: 88

void *SFXObj_Reverb::m_EchoBuffer = nullptr;   // Decl: 90
void *SFXObj_Reverb::m_UnusedBuffer = nullptr; // Decl: 91
bool SFXObj_Reverb::bUnavailable = true;       // Decl: 92

SFXObj_Reverb::ReverbStructure SFXObj_Reverb::m_EchoAllocs[4]; // Decl: 94

SFXObj_Reverb::SFXObj_Reverb() : CARSFX() {
    for (int n = 0; n < NUM_ELEMENTS(this->m_pFXEditPatch); n++) {
        this->m_pFXEditPatch[n] = nullptr;
    }
}

SFXObj_Reverb::~SFXObj_Reverb() {
    Destroy();
}

int SFXObj_Reverb::GetController(int Index) {
    return Index != 0 ? -1 : 6;
}

void SFXObj_Reverb::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 6) {
        m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
    }
}

void SFXObj_Reverb::SetupLoadData() {
    for (int n = 0; n < NUM_ELEMENTS(csfxedit); n++) {
        LoadAsset(Attrib::StringKey(csfxedit[n]), SNDPATH_FXEDIT, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
    }
}

void SFXObj_Reverb::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    if (this->m_pStateBase->m_InstNum == 0) {
        this->m_pFXEditModule[0] = nullptr;
        this->m_pFXEditModule[1] = nullptr;
    }
}

// TODO
void SFXObj_Reverb::InitSFX() {
    SndBase::InitSFX();

    for (int n = 0; n < 12; n++) {
        int index = gAEMSMgr.IsAssetLoaded(Attrib::StringKey(csfxedit[n]));

        if (index < 0) {
            this->Disable();
            return;
        }

        m_pFXEditPatch[n] = static_cast<char *>(g_SndAssetList[index].pmem);
    }

    this->Enable();
    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (this->m_pFXEditModule[0] != nullptr) {
            // this->m_pFXEditModule[0]->Release();
            this->m_pFXEditModule[0] = nullptr;
        }
        // Snd::GlobalFxProcessor::CreateInstance(Snd::DEVICE_MAIN, 0, m_pFXEditModule);
        // m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[3]);
    } else {
        if (this->m_pFXEditModule[0] != nullptr) {
            // m_pFXEditModule[0]->Release();
            this->m_pFXEditModule[0] = nullptr;
        }
        // Snd::GlobalFxProcessor::CreateInstance(Snd::DEVICE_MAIN, 0, m_pFXEditModule);
        // this->m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[8]);
        m_pTunnelCtl->SetCurrentReverbType(RVRB_HILLS, 0);
    }
}

void SFXObj_Reverb::UpdateParams(float t) {
    if (IsEnabled() && m_pTunnelCtl->m_IsLeadCar && g_pEAXSound->GetSndGameMode() != SND_FRONTEND && g_pEAXSound->GetSndGameMode() != SND_CARSHOW) {
        int ndmixverb = this->GetDMixOutput(0, DMX_VOL);
        this->m_pTunnelCtl->m_AEMSWetVol = ndmixverb * m_pTunnelCtl->m_AEMSWetVol >> 15;
        this->m_pTunnelCtl->m_GinsuWetVol = ndmixverb * m_pTunnelCtl->m_GinsuWetVol >> 15;
    }
}

// TODO
void SFXObj_Reverb::ProcessUpdate() {
    if (IsEnabled() && m_pTunnelCtl->m_IsLeadCar && g_pEAXSound->GetSndGameMode() != SND_FRONTEND && g_pEAXSound->GetSndGameMode() != SND_CARSHOW &&
        m_pTunnelCtl->bIsReadyForSwitch && (m_pFXEditModule[0] != nullptr)) {
        // SNDSYS_service();
        // m_pFXEditModule[0]->Reset();
        // m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[m_pTunnelCtl->m_ReverbType]);
    }
}

// TODO
void SFXObj_Reverb::Destroy() {
    if (this->m_pFXEditModule[0] != nullptr) {
        // this->m_pFXEditModule[0]->Release();
        this->m_pFXEditModule[0] = nullptr;
    }

    if (this->m_pFXEditModule[1] != nullptr) {
        // m_pFXEditModule[1]->Release();
        this->m_pFXEditModule[1] = nullptr;
    }

    for (int n = 0; n < NUM_ELEMENTS(this->m_pFXEditPatch); n++) {
        this->m_pFXEditPatch[n] = nullptr;
    }

    if (this->m_EchoBuffer != nullptr) {
        gAudioMemoryManager.FreeMemory(this->m_EchoBuffer);
        this->m_EchoBuffer = nullptr;
    }
}

static const int DEBUG_REVERB_BUFFER = 0; // size: 0x4, Decl: 367

// STRIPPED
void *SFXObj_Reverb::AllocReverbBuffer(char *name, int size) {}

// STRIPPED
bool SFXObj_Reverb::FreeReverbBuffer(void *ptr) {}
