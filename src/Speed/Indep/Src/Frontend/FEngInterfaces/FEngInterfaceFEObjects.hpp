#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFEOBJECTS_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFEOBJECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
void FEngSetInvisible(FEObject *obj);

inline void FEngSetInvisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

#endif
