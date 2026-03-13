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
    static char MovieFilename[64];
    char _pad[0x28];
};

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
}

char InGameAnyMovieScreen::MovieFilename[64];

void InGameAnyMovieScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}

MenuScreen *InGameAnyMovieScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) InGameAnyMovieScreen(sd);
}