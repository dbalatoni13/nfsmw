#ifndef FENG_FECOLOREDIMAGE_H
#define FENG_FECOLOREDIMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "feimage.h"

// total size: 0x60
struct FEColoredImage : public FEImage {
    inline FEColoredImage() {}
    inline FEColoredImage(const FEColoredImage& Object, bool bReference)
        : FEImage(Object, bReference) {}
    ~FEColoredImage() override;
    FEObject* Clone(bool bReference) override;

    inline void SetVertexColor(const FEColor& color, unsigned long vertexIndex, bool bRelative);
};

#endif
