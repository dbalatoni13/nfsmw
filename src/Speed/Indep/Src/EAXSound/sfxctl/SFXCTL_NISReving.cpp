#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"

void SFXCTL_Physics::MsgRevOff(const MAIEngineRev &message) {
    eCurNisRevingState = NIS_OFF;
    pRevData = nullptr;
}

void NIS_RevManager::StartNISReving() {}
