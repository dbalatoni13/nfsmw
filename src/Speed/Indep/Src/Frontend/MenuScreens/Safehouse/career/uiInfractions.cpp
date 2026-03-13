#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct PostPursuitInfractionsScreen : MenuScreen {
    PostPursuitInfractionsScreen(ScreenConstructorData *sd);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {}
    char _pad[0x1C];
};

MenuScreen *PostPursuitInfractionsScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) PostPursuitInfractionsScreen(sd);
}