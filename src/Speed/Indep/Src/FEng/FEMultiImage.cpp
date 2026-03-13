#include <new>

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FEImage::FEImage(const FEImage& Object, bool bReference)
    : FEObject(Object, bReference), ImageFlags(Object.ImageFlags) {}

FEImage::~FEImage() {}

FEObject* FEImage::Clone(bool bReference) {
    FEImage* pImage = static_cast<FEImage*>(FEngMalloc(sizeof(FEImage), 0, 0));

    if (pImage) {
        new (pImage) FEImage(*this, bReference);
    }

    return pImage;
}

FEMultiImage::FEMultiImage(const FEMultiImage& Object, bool bReference)
    : FEImage(Object, bReference) {}

FEMultiImage::~FEMultiImage() {}

FEObject* FEMultiImage::Clone(bool bReference) {
    FEMultiImage* pImage = static_cast<FEMultiImage*>(FEngMalloc(sizeof(FEMultiImage), 0, 0));

    if (pImage) {
        new (pImage) FEMultiImage(*this, bReference);
    }

    return pImage;
}

FEMovie::FEMovie(const FEMovie& Object, bool bReference)
    : FEObject(Object, bReference), CurTime(Object.CurTime) {}

FEMovie::~FEMovie() {}

FEObject* FEMovie::Clone(bool bReference) {
    FEMovie* pMovie = static_cast<FEMovie*>(FEngMalloc(sizeof(FEMovie), 0, 0));

    if (pMovie) {
        new (pMovie) FEMovie(*this, bReference);
    }

    return pMovie;
}

unsigned long FEMultiImage::GetTexture(unsigned long tex_num) {
    if (tex_num > 2) {
        return 0xFFFFFFFF;
    }

    return hTexture[tex_num];
}
