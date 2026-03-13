#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"

extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);

Infractions::Infractions(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IInfractions(pOutter)
{
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
