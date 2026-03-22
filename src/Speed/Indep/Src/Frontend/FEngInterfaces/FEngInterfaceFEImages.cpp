FEImage* FEngFindImage(const char* pkg_name, int name_hash) {
    FEObject* obj = FEngFindObject(pkg_name, name_hash);
    if (obj == nullptr || obj->Type != FE_Image) {
        return nullptr;
    }
    return static_cast<FEImage*>(obj);
}

unsigned int FEngGetTextureHash(FEImage* image) {
    if (image != nullptr) {
        return image->Handle;
    }
    return 0;
}

void FEngSetTextureHash(FEImage* image, unsigned int texture_hash) {
    if (image == nullptr) {
        return;
    }
    if (image->Handle == texture_hash) {
        return;
    }
    image->Handle = texture_hash;
    image->Flags |= 0x2400000;
}

void FEngSetButtonTexture(FEImage* img, unsigned int tex_hash) {
    if (img != nullptr) {
        FEngSetTextureHash(img, tex_hash);
    }
}