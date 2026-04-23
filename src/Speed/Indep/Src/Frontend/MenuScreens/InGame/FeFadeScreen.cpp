#include "Speed/Indep/Src/Frontend/FEManager.hpp"

bool FEngIsScriptSet(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash);

FadeScreen::FadeScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

FadeScreen::~FadeScreen() {}

void FadeScreen::NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) {
    if (Message == 0x83323AEB) {
        FEManager::Get()->SuppressControllerError(true);
    } else if (Message == 0xC7D61AC7) {
        FEManager::Get()->SuppressControllerError(false);
    }
}

bool FadeScreen::IsFadeScreenOn() {
    return FEngIsScriptSet("FadeScreen.fng", 0x027FF2DC, 0x5079C8F8) != false;
}
