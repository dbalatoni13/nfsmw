#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"

extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern int FEPrintf(const char *pkg_name, FEObject *obj, const char *fmt, ...);
extern int FEPrintf(FEString *text, const char *fmt, ...);

Infractions::Infractions(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200000000ULL) //
    , IInfractions(pOutter)
{
    RegisterGroup(FEHashUpper("INFRACTIONS_HEADER_GROUP"));
    mpDataGenericIcon = RegisterObject(FEHashUpper("GENERIC_ICONS"));
    mpDataTotalInfractions = RegisterString(FEHashUpper("TOTAL_INFRACTIONS_TEXT"));
    for (int i = 0; i < 4; i++) {
        char buf[32];
        bSPrintf(buf, "INFRACTION_GROUP_%d", i + 1);
        mpDataInfractionStrings[i] = RegisterGroup(FEHashUpper(buf));
    }
}

void Infractions::Update(IPlayer *player) {
    bool infractionStringShowing = false;
    for (int i = 0; i <= 3; i++) {
        if (!FEngIsScriptSet(mpDataInfractionStrings[i], 0x16a259)) {
            infractionStringShowing = true;
            break;
        }
    }
    if (!infractionStringShowing) {
        if (FEngIsScriptSet(mpDataGenericIcon, 0x5079c8f8) ||
            FEngIsScriptSet(mpDataGenericIcon, 0x3826a28)) {
            FEngSetScript(mpDataGenericIcon, 0x33113ac, true);
        }
    }
}

void Infractions::RequestInfraction(const char *infractionString) {
    for (int i = 0; i < 4; i++) {
        if (FEngIsScriptSet(mpDataInfractionStrings[i], 0x16a259)) {
            FEngSetScript(mpDataInfractionStrings[i], 0x5079c8f8, true);
            FEPrintf(GetPackageName(), mpDataInfractionStrings[i], "%s", infractionString);
            break;
        }
    }
    FEngSetScript(mpDataGenericIcon, 0x5079c8f8, true);
    FEPrintf(mpDataTotalInfractions, "%d", GInfractionManager::Get().GetNumInfractions());
}
