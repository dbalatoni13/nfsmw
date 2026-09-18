#include "FEngInterfaceFEImages.hpp"

#include "FEngInterfaceFEObjects.hpp"

FEImage *FEngFindImage(const char *pkg_name, int obj_hash) {
    FEObject *obj = FEngFindObject(pkg_name, obj_hash);
    if (obj == nullptr || obj->Type != FE_Image) {
        return nullptr;
    }
    return (FEImage *)obj;
}

unsigned int FEngGetTextureHash(FEImage *image) {
    if (image != nullptr) {
        return image->Handle;
    }
    return 0;
}

void FEngSetTextureHash(FEImage *image, unsigned int hash) {
    if (image == nullptr) {
        return;
    }
    if (image->Handle == hash) {
        return;
    }
    image->Handle = hash;
    image->Flags |= 0x2400000;
}

void FEngSetButtonTexture(FEImage *image, unsigned int hash) {
    if (image != nullptr) {
        FEngSetTextureHash(image, hash);
    }
}
