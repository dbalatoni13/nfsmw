#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFEMOVIES_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFEMOVIES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEMovie.h"

void FEngSetMovieName(FEMovie *movie, const char *name);

inline void FEngSetMovieName(const char *pkg_name, uint32 obj_hash, const char *name) {
    FEngSetMovieName(static_cast<FEMovie *>(FEngFindObject(pkg_name, obj_hash)), name);
}

#endif
