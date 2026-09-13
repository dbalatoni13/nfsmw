#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELanguageSelect.hpp"

LanguageSelectScreen::LanguageSelectScreen(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    SetInitialOption(0);
    RefreshHeader();
    StartedTimer = RealTimer;
}

LanguageSelectScreen::~LanguageSelectScreen() {}

void LanguageSelectScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);
}
