#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct InGameAnyTutorialScreen {
    static void SetMovieName(const char *filename);
    static char MovieFilename[64];
};

char InGameAnyTutorialScreen::MovieFilename[64];

void InGameAnyTutorialScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}
