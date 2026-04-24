#include "Speed/Indep/Src/FEng/FEMovie.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

void FEngSetMovieName(FEMovie* movie, const char* name) {
    if (movie != nullptr) {
        bStrNCpy(reinterpret_cast<char*>(movie->Handle), name, 0x100);
        movie->Flags |= 0x400000;
    }
}