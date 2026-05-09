#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFEBUTTONS_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFEBUTTONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"

void FEngSetCurrentButton(const char *pkg_name, uint32 hash);

FEObject *FEngGetCurrentButton(const char *pkg_name);

void FEngSetButtonState(const char *pkg_name, uint32 button_hash, bool enabled);

#endif
