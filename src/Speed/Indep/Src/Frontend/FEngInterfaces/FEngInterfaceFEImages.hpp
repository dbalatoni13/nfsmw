#ifndef FENGINTERFACEFEIMAGES_H
#define FENGINTERFACEFEIMAGES_H

#include "Speed/Indep/Src/FEng/FEImage.h"

FEImage *FEngFindImage(const char *pkg_name, int name_hash);

inline FEImage *FEngFindImage(const char *pkg_name, const char *object_name) {
    return FEngFindImage(pkg_name, FEHashUpper(object_name));
}

uint32 FEngGetTextureHash(FEImage *image);

void FEngSetTextureHash(FEImage *image, uint32 texture_hash);

inline void FEngSetTextureHash(const char *pkg_name, uint32 obj_hash, uint32 texture_hash) {
    FEngSetTextureHash(FEngFindImage(pkg_name, obj_hash), texture_hash);
}

void FEngSetButtonTexture(FEImage *img, uint32 tex_hash);

#endif
