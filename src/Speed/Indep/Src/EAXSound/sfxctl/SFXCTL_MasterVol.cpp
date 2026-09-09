#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_MasterVol.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

int GameFlowSndState[15];

DEFINE_CREATABLE(0, SFXCTL_MasterVol, SFXCTL);

SFXCTL_MasterVol::SFXCTL_MasterVol() {
    bMemSet(GameFlowSndState, 0, sizeof(GameFlowSndState));
}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

// TODO when FE is merged
void SFXCTL_MasterVol::InitSFX() {
    // float fvol = g_pEAXSound->m_pCurAudioSettings->GetMasteredSoundEffectsVol();
    // int nvol = static_cast<int>(fvol);
}

int g_IG_Music_Scale = 0x7FFF; // size: 0x4, address: 0xFFFFFFFF, Decl: 58

// TODO SFXCTL_MasterVol::UpdateParams after FE is merged

DEFINE_CREATABLE(0x10, SFXCTL_GameState, SFXCTL);

void SFXCTL_GameState::UpdateMixerOutputs() {
    for (int i = 0; i < 14; i++) {
        extern uint32 g_ActiveSFXStates; // Decl: 230
        if (g_ActiveSFXStates & (1 << i)) {
            this->SetDMIX_Input(i, 0x7fff);
        } else {
            this->SetDMIX_Input(i, 0);
        }
    }
}
