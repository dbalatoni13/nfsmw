static bool gInGameMoviePlaying;

#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct InGameAnyMovieScreen {
    static bool IsPlaying();
    static void SetMovieName(const char *filename);
    static char MovieFilename[64];
};

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
}

char InGameAnyMovieScreen::MovieFilename[64];

void InGameAnyMovieScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}