#ifndef FENG_FECOLOREDIMAGE_H
#define FENG_FECOLOREDIMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"
#include "feimage.h"
// total size: 0x60
class FEColoredImage : public FEImage {
  public:
    inline FEColoredImage() {}
    inline FEColoredImage(const FEColoredImage &Object, bool bReference) : FEImage(reinterpret_cast<const FEImage &>(Object), bReference) {}
    ~FEColoredImage() override;
    FEObject *Clone(bool bReference) override;

    inline void SetVertexColor(const FEColor &color, unsigned long vertexIndex, bool bRelative);
};

#endif
