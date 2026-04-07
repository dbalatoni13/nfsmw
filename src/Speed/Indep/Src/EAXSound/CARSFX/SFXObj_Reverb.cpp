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
    int SetCustom(void *pFxDefinition);
};
} // namespace Snd

extern const char *csfxedit[];
extern stSndDataLoadParams g_SndAssetList[];

Snd::GlobalFxProcessor *SFXObj_Reverb::m_pFXEditModule[2];
char *SFXObj_Reverb::m_pFXEditPatch[12];

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
