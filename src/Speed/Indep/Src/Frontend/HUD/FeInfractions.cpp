#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

Infractions::Infractions(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200000000ULL), IInfractions(pOutter) {
    RegisterGroup(FEHashUpper("INFRACTIONS_HEADER_GROUP"));
    mpDataGenericIcon = RegisterObject("GENERIC_ICONS");
    mpDataTotalInfractions = RegisterString("TOTAL_INFRACTIONS_TEXT");
    for (int i = 0; i < 4; i++) {
        char sztemp[32];
        bSPrintf(sztemp, "INFRACTION_GROUP_%d", i + 1);
        mpDataInfractionStrings[i] = RegisterGroup(FEHashUpper(sztemp));
    }
}

void Infractions::Update(IPlayer *player) {
    bool infractionStringShowing = false;
    for (int i = 0; i <= 3; i++) {
        if (!FEngIsScriptSet(mpDataInfractionStrings[i], FEHASH_HIDE)) {
            infractionStringShowing = true;
            break;
        }
    }
    if (!infractionStringShowing) {
        if (FEngIsScriptSet(mpDataGenericIcon, FEHASH_APPEAR) || FEngIsScriptSet(mpDataGenericIcon, 0x3826a28)) {
            FEngSetScript(mpDataGenericIcon, FEHASH_LEAVE, true);
        }
    }
}

void Infractions::RequestInfraction(const char *infractionString) {
    for (int i = 0; i < 4; i++) {
        if (FEngIsScriptSet(mpDataInfractionStrings[i], FEHASH_HIDE)) {
            FEngSetScript(mpDataInfractionStrings[i], FEHASH_APPEAR, true);
            FEPrintf(GetPackageName(), mpDataInfractionStrings[i], "%s", infractionString);
            break;
        }
    }
    FEngSetScript(mpDataGenericIcon, FEHASH_APPEAR, true);
    FEPrintf(mpDataTotalInfractions, "%d", GInfractionManager::Get().GetNumInfractions());
}
