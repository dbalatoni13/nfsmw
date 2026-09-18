#ifndef FENGINTERFACEFEMOVIES_H
#define FENGINTERFACEFEMOVIES_H

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEMovie.h"

void FEngSetMovieName(FEMovie *movie, const char *name);

inline void FEngSetMovieName(const char *pkg_name, uint32 obj_hash, const char *name) {
    FEngSetMovieName(static_cast<FEMovie *>(FEngFindObject(pkg_name, obj_hash)), name);
}

#endif
