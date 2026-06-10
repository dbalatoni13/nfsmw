#ifndef FECOLOREDIMAGE_H_
#define FECOLOREDIMAGE_H_

#include "FEImage.h"

// File: speed/indep/src/feng/FEColoredImage.h
// total size: 0x94
// Decl: speed/indep/src/feng/FEColoredImage.h:27
class FEColoredImageData : public FEImageData {
  public:
    FEColor VertexColors[4]; // offset 0x54, size 0x40, Decl: speed/indep/src/feng/FEColoredImage.h:29
};

// total size: 0x60
// Decl: speed/indep/src/feng/FEColoredImage.h:36
class FEColoredImage : public FEImage {
  public:
    FEColoredImage() {} // Decl: speed/indep/src/feng/FEColoredImage.h:39
    FEColoredImage(const FEColoredImage &Object, bool bReference) : FEImage(reinterpret_cast<const FEImage &>(Object), bReference) {}
    ~FEColoredImage() override {}

    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FEColoredImage.h:43

    void SetVertexColor(const FEColor &color, unsigned long vertexIndex, bool bRelative) {}
};

#endif
