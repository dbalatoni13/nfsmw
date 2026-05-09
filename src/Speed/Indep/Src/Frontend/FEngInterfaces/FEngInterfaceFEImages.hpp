#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFEIMAGES_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFEIMAGES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/FEng/feimage.h"

FEImage *FEngFindImage(const char *pkg_name, int name_hash);

uint32 FEngGetTextureHash(FEImage *image);

void FEngSetTextureHash(FEImage *image, uint32 texture_hash);

inline void FEngSetTextureHash(const char *pkg_name, uint32 obj_hash, uint32 texture_hash) {
    FEngSetTextureHash(FEngFindImage(pkg_name, obj_hash), texture_hash);
}

void FEngSetButtonTexture(FEImage *img, uint32 tex_hash);

#endif
