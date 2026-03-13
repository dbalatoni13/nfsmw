#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct MenuScreen;
struct ScreenConstructorData;

struct InGameAnyTutorialScreen : MenuScreen {
    InGameAnyTutorialScreen(ScreenConstructorData *sd);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {}
    static void SetMovieName(const char *filename);
    static char MovieFilename[64];
    char _pad[0x24];
};

char InGameAnyTutorialScreen::MovieFilename[64];

void InGameAnyTutorialScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}

MenuScreen *InGameAnyTutorialScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) InGameAnyTutorialScreen(sd);
}
