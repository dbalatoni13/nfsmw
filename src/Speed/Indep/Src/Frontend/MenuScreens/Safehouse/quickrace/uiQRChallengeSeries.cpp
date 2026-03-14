// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRChallengeSeries.hpp"

extern GRaceParameters *theChallengeRace;

void ChallengeDatum::NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) {
    if (msg != 0x0C407210)
        return;
    if (!IsLocked()) {
        theChallengeRace = race;
    } else {
        theChallengeRace = nullptr;
    }
}
