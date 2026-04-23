#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELanguageSelect.hpp"

extern Timer RealTimer;

LanguageSelectScreen::LanguageSelectScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd) //
{
    StartedTimer.ResetLow();
    if (bFadeInIconsImmediately) {
        Options.fCurFadeTime = 0.0f;
        Options.bDelayUpdate = false;
        Options.bFadingIn = true;
        Options.bFadingOut = false;
    }
    Options.SetInitialPos(0);
    RefreshHeader();
    StartedTimer = RealTimer;
}

LanguageSelectScreen::~LanguageSelectScreen() {}

void LanguageSelectScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);
}