#include <new>

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"
#include "Speed/Indep/Src/FEng/FEAnimImage.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FESimpleImage.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FEImage::FEImage(const FEImage& Object, bool bReference)
    : FEObject(Object, bReference), ImageFlags(Object.ImageFlags) {}

FEImage::~FEImage() {}

FEObject* FEImage::Clone(bool bReference) {
    FEImage* pImage = static_cast<FEImage*>(FEngMalloc(sizeof(FEImage), 0, 0));
    new (pImage) FEImage(*this, bReference);
    return pImage;
}

FEMultiImage::FEMultiImage(const FEMultiImage& Object, bool bReference)
    : FEImage(Object, bReference) {}

FEMultiImage::~FEMultiImage() {}

FEObject* FEMultiImage::Clone(bool bReference) {
    FEMultiImage* pImage = static_cast<FEMultiImage*>(FEngMalloc(sizeof(FEMultiImage), 0, 0));
    new (pImage) FEMultiImage(*this, bReference);
    return pImage;
}

FEMovie::FEMovie(const FEMovie& Object, bool bReference)
    : FEObject(Object, bReference), CurTime(Object.CurTime) {}

FEMovie::~FEMovie() {}

FEObject* FEMovie::Clone(bool bReference) {
    FEMovie* pMovie = static_cast<FEMovie*>(FEngMalloc(sizeof(FEMovie), 0, 0));
    new (pMovie) FEMovie(*this, bReference);
    return pMovie;
}

unsigned long FEMultiImage::GetTexture(unsigned long tex_num) {
    if (tex_num > 2) {
        return 0xFFFFFFFF;
    }

    return hTexture[tex_num];
}

void FEMultiImage::SetUVs(unsigned long tex_num, FEVector2 top_left, FEVector2 bottom_right) {
    if (tex_num > 2) return;
    FEMultiImageData* pImgData = static_cast<FEMultiImageData*>(static_cast<void*>(pData));
    pImgData->TopLeftUV[tex_num] = top_left;
    pImgData->BottomRightUV[tex_num] = bottom_right;
}

void FEMultiImage::GetUVs(unsigned long tex_num, FEVector2& top_left, FEVector2& bottom_right) {
    if (tex_num > 2) return;
    FEMultiImageData* pImgData = static_cast<FEMultiImageData*>(static_cast<void*>(pData));
    top_left = pImgData->TopLeftUV[tex_num];
    bottom_right = pImgData->BottomRightUV[tex_num];
}

FEAnimImage::~FEAnimImage() {}

FEObject* FEAnimImage::Clone(bool bReference) {
    FEAnimImage* pImage = static_cast<FEAnimImage*>(FEngMalloc(sizeof(FEAnimImage), 0, 0));
    new (pImage) FEAnimImage(*this, bReference);
    return pImage;
}

FEColoredImage::~FEColoredImage() {}

FEObject* FEColoredImage::Clone(bool bReference) {
    FEColoredImage* pImage = static_cast<FEColoredImage*>(FEngMalloc(sizeof(FEColoredImage), 0, 0));
    new (pImage) FEColoredImage(*this, bReference);
    return pImage;
}

FESimpleImage::~FESimpleImage() {}

FEObject* FESimpleImage::Clone(bool bReference) {
    FESimpleImage* pImage = static_cast<FESimpleImage*>(FEngMalloc(sizeof(FESimpleImage), 0, 0));
    new (pImage) FESimpleImage(*this, bReference);
    return pImage;
}
