#include "FEngInterfaceFEMovies.hpp"

#include "Speed/Indep/bWare/Inc/Strings.hpp"

void FEngSetMovieName(FEMovie *movie, const char *name) {
    if (movie == NULL) {
        return;
    }
    bStrNCpy((char *)movie->Handle, name, 0x100);
    movie->Flags |= 0x400000;
}
