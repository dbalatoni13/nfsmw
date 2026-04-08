#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

namespace Snd {
enum Device {
    DEVICE_MAIN = 0,
    DEVICE_IOP = 1,
};

class GlobalFxProcessor {
  public:
    static int CreateInstance(Device device, int bus, GlobalFxProcessor **ppGlobalFxProcessor);
    int Release();
    int Reset();
    int SetCustom(void *pFxDefinition);
};
} // namespace Snd

extern const char *csfxedit[];
extern stSndDataLoadParams g_SndAssetList[];
extern void SNDSYS_service();

SndBase::TypeInfo SFXObj_Reverb::s_TypeInfo = {
    0x00020100,
    "SFXObj_Reverb",
    &SndBase::s_TypeInfo,
    SFXObj_Reverb::CreateObject,
};
Snd::GlobalFxProcessor *SFXObj_Reverb::m_pFXEditModule[2];
char *SFXObj_Reverb::m_pFXEditPatch[12];
void *SFXObj_Reverb::m_EchoBuffer;
SFXObj_Reverb::ReverbStructure SFXObj_Reverb::m_EchoAllocs[4];

void SFXObj_Reverb::ReverbStructure::Clear() {
    Size = 0;
    Alloc = nullptr;
}

SFXObj_Reverb::TypeInfo *SFXObj_Reverb::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Reverb::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Reverb::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXObj_Reverb();
    }
    return new (s_TypeInfo.typeName, true) SFXObj_Reverb();
}

SFXObj_Reverb::SFXObj_Reverb()
    : CARSFX() {
    for (int n = 0; n < 12; n++) {
        m_pFXEditPatch[n] = nullptr;
    }
}

SFXObj_Reverb::~SFXObj_Reverb() {
    Destroy();
}

int SFXObj_Reverb::GetController(int Index) {
    return Index == 0 ? 6 : -1;
}

void SFXObj_Reverb::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 6) {
        m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
    }
}

void SFXObj_Reverb::SetupLoadData() {
    for (int n = 0; n < 12; n++) {
        LoadAsset(Attrib::StringKey(csfxedit[n]), SNDPATH_FXEDIT, EAXSND_DT_GENERIC_DATA, eBANK_SLOT_NONE, true);
    }
}

void SFXObj_Reverb::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    if (m_pStateBase->m_InstNum == 0) {
        m_pFXEditModule[0] = nullptr;
        m_pFXEditModule[1] = nullptr;
    }
}

void SFXObj_Reverb::InitSFX() {
    SndBase::InitSFX();

    for (int n = 0; n < 12; n++) {
        int index = gAEMSMgr.IsAssetLoaded(Attrib::StringKey(csfxedit[n]));

        if (index < 0) {
            Disable();
            return;
        }

        m_pFXEditPatch[n] = static_cast<char *>(g_SndAssetList[index].pmem);
    }

    Enable();
    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (m_pFXEditModule[0]) {
            m_pFXEditModule[0]->Release();
            m_pFXEditModule[0] = nullptr;
        }
        Snd::GlobalFxProcessor::CreateInstance(Snd::DEVICE_MAIN, 0, m_pFXEditModule);
        m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[3]);
    } else {
        if (m_pFXEditModule[0]) {
            m_pFXEditModule[0]->Release();
            m_pFXEditModule[0] = nullptr;
        }
        Snd::GlobalFxProcessor::CreateInstance(Snd::DEVICE_MAIN, 0, m_pFXEditModule);
        m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[8]);
        m_pTunnelCtl->SetCurrentReverbType(RVRB_HILLS, 0);
    }
}

void SFXObj_Reverb::UpdateParams(float t) {
    if (IsEnabled() && m_pTunnelCtl->m_IsLeadCar) {
        bool bUpdateReverb = false;

        if (g_pEAXSound->GetSndGameMode() != SND_FRONTEND &&
            g_pEAXSound->GetSndGameMode() != SND_CARSHOW) {
            bUpdateReverb = true;
        }

        if (bUpdateReverb) {
            int ndmixverb = GetDMixOutput(0, DMX_VOL);
            m_pTunnelCtl->m_AEMSWetVol = ndmixverb * m_pTunnelCtl->m_AEMSWetVol >> 15;
            m_pTunnelCtl->m_GinsuWetVol = ndmixverb * m_pTunnelCtl->m_GinsuWetVol >> 15;
        }
    }
}

void SFXObj_Reverb::ProcessUpdate() {
    if (IsEnabled() && m_pTunnelCtl->m_IsLeadCar) {
        bool bCanSwitch = false;

        if (g_pEAXSound->GetSndGameMode() != SND_FRONTEND &&
            g_pEAXSound->GetSndGameMode() != SND_CARSHOW) {
            bCanSwitch = true;
        }

        if (bCanSwitch && m_pTunnelCtl->bIsReadyForSwitch && m_pFXEditModule[0]) {
            SNDSYS_service();
            m_pFXEditModule[0]->Reset();
            m_pFXEditModule[0]->SetCustom(m_pFXEditPatch[m_pTunnelCtl->m_ReverbType]);
        }
    }
}

void SFXObj_Reverb::Destroy() {
    if (m_pFXEditModule[0]) {
        m_pFXEditModule[0]->Release();
        m_pFXEditModule[0] = nullptr;
    }

    if (m_pFXEditModule[1]) {
        m_pFXEditModule[1]->Release();
        m_pFXEditModule[1] = nullptr;
    }

    for (int n = 0; n < 12; n++) {
        m_pFXEditPatch[n] = nullptr;
    }

    if (m_EchoBuffer) {
        gAudioMemoryManager.FreeMemory(m_EchoBuffer);
        m_EchoBuffer = nullptr;
    }
}
