#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

FadeScreen::FadeScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

FadeScreen::~FadeScreen() {}

void FadeScreen::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    const u32 FEHash_Activate_Controllers = 0xC7D61AC7;
    const u32 FEHash_Supress_Controllers = 0x83323AEB;

    if (Message == FEHash_Supress_Controllers) {
        FEManager::Get()->SuppressControllerError(true);
    } else if (Message == FEHash_Activate_Controllers) {
        FEManager::Get()->SuppressControllerError(false);
    }
}

bool FadeScreen::IsFadeScreenOn() {
    return FEngIsScriptSet("FadeScreen.fng", 0x027FF2DC, FEHASH_APPEAR) != false;
}
