#include <new>

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"
#include "Speed/Indep/Src/FEng/FEAnimImage.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FESimpleImage.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FEImage::~FEImage() {}

FEObject* FEImage::Clone(bool bReference) {
    return FENG_NEW FEImage(*this, bReference);
}

FEMultiImage::FEMultiImage(const FEMultiImage& Object, bool bReference)
    : FEImage(Object, bReference) {}

FEMultiImage::~FEMultiImage() {}

FEObject* FEMultiImage::Clone(bool bReference) {
    return FENG_NEW FEMultiImage(*this, bReference);
}

FEMovie::FEMovie(const FEMovie& Object, bool bReference)
    : FEObject(Object, bReference), CurTime(Object.CurTime) {}

FEMovie::~FEMovie() {}

FEObject* FEMovie::Clone(bool bReference) {
    return FENG_NEW FEMovie(*this, bReference);
}

unsigned long FEMultiImage::GetTexture(unsigned long tex_num) {
    if (tex_num > 2) {
        return 0xFFFFFFFF;
    }

    return hTexture[tex_num];
}

void FEMultiImage::SetUVs(unsigned long tex_num, FEVector2 top_left, FEVector2 bottom_right) {
    if (tex_num > 2) return;
    reinterpret_cast<FEMultiImageData*>(pData)->TopLeftUV[tex_num].x = top_left.x;
    reinterpret_cast<FEMultiImageData*>(pData)->TopLeftUV[tex_num].y = top_left.y;
    reinterpret_cast<FEMultiImageData*>(pData)->BottomRightUV[tex_num].x = bottom_right.x;
    reinterpret_cast<FEMultiImageData*>(pData)->BottomRightUV[tex_num].y = bottom_right.y;
}

void FEMultiImage::GetUVs(unsigned long tex_num, FEVector2& top_left, FEVector2& bottom_right) {
    if (tex_num > 2) return;
    top_left.x = reinterpret_cast<FEMultiImageData*>(pData)->TopLeftUV[tex_num].x;
    top_left.y = reinterpret_cast<FEMultiImageData*>(pData)->TopLeftUV[tex_num].y;
    bottom_right.x = reinterpret_cast<FEMultiImageData*>(pData)->BottomRightUV[tex_num].x;
    bottom_right.y = reinterpret_cast<FEMultiImageData*>(pData)->BottomRightUV[tex_num].y;
}

FEAnimImage::~FEAnimImage() {}

FEObject* FEAnimImage::Clone(bool bReference) {
    return FENG_NEW FEAnimImage(*this, bReference);
}

FEColoredImage::~FEColoredImage() {}

FEObject* FEColoredImage::Clone(bool bReference) {
    return FENG_NEW FEColoredImage(*this, bReference);
}

FESimpleImage::~FESimpleImage() {}

FEObject* FESimpleImage::Clone(bool bReference) {
    return FENG_NEW FESimpleImage(*this, bReference);
}
