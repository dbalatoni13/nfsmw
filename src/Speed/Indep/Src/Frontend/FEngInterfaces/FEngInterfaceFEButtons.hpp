#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFEBUTTONS_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFEBUTTONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"


void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);
FEObject *FEngGetCurrentButton(const char *pkg_name);
void FEngSetButtonState(const char *pkg_name, unsigned int button_hash, bool enabled);

inline void FEngEnableButton(const char *pkg_name, unsigned int button_hash) {
    FEngSetButtonState(pkg_name, button_hash, true);
}
unsigned char FEngGetLastButton(const char *pkg_name);
void FEngSetLastButton(const char *pkg_name, unsigned char button);

#endif
