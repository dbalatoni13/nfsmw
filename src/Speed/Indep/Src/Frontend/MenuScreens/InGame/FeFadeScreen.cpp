#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

FadeScreen::FadeScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

FadeScreen::~FadeScreen() {}

void FadeScreen::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    if (Message == 0x83323AEB) {
        FEManager::Get()->SuppressControllerError(true);
    } else if (Message == 0xC7D61AC7) {
        FEManager::Get()->SuppressControllerError(false);
    }
}

bool FadeScreen::IsFadeScreenOn() {
    return FEngIsScriptSet("FadeScreen.fng", 0x027FF2DC, 0x5079C8F8) != false;
}
