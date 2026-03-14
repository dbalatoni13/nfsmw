static bool gInGameMoviePlaying;

#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct MenuScreen;
struct ScreenConstructorData;

struct InGameAnyMovieScreen : MenuScreen {
    InGameAnyMovieScreen(ScreenConstructorData *sd);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {}
    static bool IsPlaying();
    static void SetMovieName(const char *filename);
    static const char *GetFEngPackageName();
    static char MovieFilename[64];
    char _pad[0x28];
};

extern bool eIsWidescreen();

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
}

char InGameAnyMovieScreen::MovieFilename[64];

void InGameAnyMovieScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}

const char *InGameAnyMovieScreen::GetFEngPackageName() {
    bool ps2_widescreen = eIsWidescreen();
    if (!ps2_widescreen) {
        return "InGameAnyMovie.fng";
    }
    return "WS_InGameAnyMovie.fng";
}

MenuScreen *InGameAnyMovieScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) InGameAnyMovieScreen(sd);
}