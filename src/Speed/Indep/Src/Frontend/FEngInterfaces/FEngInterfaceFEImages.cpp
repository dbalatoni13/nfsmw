#include "FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

#include <types.h>

FEImage *FEngFindImage(const char *pkg_name, int name_hash) {
    FEObject *obj = FEngFindObject(pkg_name, name_hash);
    if (obj == nullptr || obj->Type != FE_Image) {
        return nullptr;
    }
    return static_cast<FEImage *>(obj);
}

uint32 FEngGetTextureHash(FEImage *image) {
    if (image != nullptr) {
        return image->Handle;
    }
    return 0;
}

void FEngSetTextureHash(FEImage *image, uint32 texture_hash) {
    if (image == nullptr) {
        return;
    }
    if (image->Handle == texture_hash) {
        return;
    }
    image->Handle = texture_hash;
    image->Flags |= 0x2400000;
}

void FEngSetButtonTexture(FEImage *img, uint32 tex_hash) {
    if (img != nullptr) {
        FEngSetTextureHash(img, tex_hash);
    }
}
