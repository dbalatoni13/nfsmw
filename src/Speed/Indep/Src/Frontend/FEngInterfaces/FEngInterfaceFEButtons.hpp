#ifndef FENGINTERFACEFEBUTTONS_H
#define FENGINTERFACEFEBUTTONS_H

#include "Speed/Indep/Src/FEng/FEObject.h"

void FEngSetCurrentButton(const char *pkg_name, uint32 hash);

FEObject *FEngGetCurrentButton(const char *pkg_name);

void FEngSetButtonState(const char *pkg_name, uint32 button_hash, bool enabled);

#endif
