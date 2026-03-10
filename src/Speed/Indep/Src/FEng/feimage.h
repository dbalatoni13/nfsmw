#ifndef _FEIMAGE
#define _FEIMAGE

#include "FEObject.h"

struct FEImageData;

// total size: 0x60
struct FEImage : public FEObject {
    unsigned long ImageFlags; // offset 0x5C, size 0x4

    inline FEImage();
    inline FEImage(const FEImage& Object, bool bReference);
    ~FEImage() override;

    inline FEImageData* GetImageData();

    FEObject* Clone(bool bReference) override;

    inline void SetTopLeft(const FEVector2& topright, bool bRelative);
    inline void SetBottomRight(const FEVector2& bottomright, bool bRelative);
};

#endif
