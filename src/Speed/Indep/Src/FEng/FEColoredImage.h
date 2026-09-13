#ifndef FECOLOREDIMAGE_H_
#define FECOLOREDIMAGE_H_

#include "FEImage.h"
#include "Speed/Indep/Src/FEng/FEObject.h"

// total size: 0x94
// Decl: 27
class FEColoredImageData : public FEImageData {
  public:
    FEColor VertexColors[4]; // offset 0x54, size 0x40, Decl: 29
};

// total size: 0x60
// Decl: 36
class FEColoredImage : public FEImage {
  public:
    FEColoredImage() { // Decl: 39
        Type = FE_ColoredImage;
    }
    FEColoredImage(const FEColoredImage &Object, bool bReference) : FEImage(reinterpret_cast<const FEImage &>(Object), bReference) {}
    ~FEColoredImage() override {}

    FEObject *Clone(bool bReference) override { // Decl: 43
        return FNEW FEColoredImage(*this, bReference);
    }

    void SetVertexColor(const FEColor &color, unsigned long vertexIndex, bool bRelative) {}
};

#endif
