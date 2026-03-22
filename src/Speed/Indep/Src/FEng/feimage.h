#ifndef _FEIMAGE
#define _FEIMAGE

#include "FEObject.h"

struct FEImageData;

// total size: 0x60
struct FEImage : public FEObject {
    unsigned long ImageFlags; // offset 0x5C, size 0x4

    inline FEImage() : FEObject(), ImageFlags(0) {}
    inline FEImage(const FEImage& Object, bool bReference)
        : FEObject(Object, bReference), ImageFlags(Object.ImageFlags) {}
    ~FEImage() override;

    inline FEImageData* GetImageData();

    FEObject* Clone(bool bReference) override;

    inline void SetTopLeft(const FEVector2& topleft, bool bRelative);
    inline void SetBottomRight(const FEVector2& bottomright, bool bRelative);
};

inline void FEImage::SetTopLeft(const FEVector2& topleft, bool bRelative) {
    SetTrackValue(FETrack_UpperLeft, topleft, bRelative);
    Flags |= 0x400000;
}

inline void FEImage::SetBottomRight(const FEVector2& bottomright, bool bRelative) {
    SetTrackValue(FETrack_LowerRight, bottomright, bRelative);
    Flags |= 0x400000;
}

#endif
