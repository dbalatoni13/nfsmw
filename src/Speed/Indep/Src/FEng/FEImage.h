#ifndef FEIMAGE_H_
#define FEIMAGE_H_

#include "FEObject.h"

// total size: 0x54
// Decl: 33
class FEImageData : public FEObjData {
  public:
    FEVector2 UpperLeft;  // offset 0x44, size 0x8, Decl: 35
    FEVector2 LowerRight; // offset 0x4C, size 0x8, Decl: 36
};

// total size: 0x60
// Decl: 44
class FEImage : public FEObject {
  public:
    u32 ImageFlags; // offset 0x5C, size 0x4, Decl: 46

    FEImage() : FEObject(), ImageFlags(0) { // Decl: 48
        Type = FE_Image;
    }
    FEImage(const FEImage &Object, bool bReference) : FEObject(Object, bReference), ImageFlags(Object.ImageFlags) {}
    ~FEImage() override {} // Decl: 50

    FEImageData *GetImageData() { // Decl: 52
        return static_cast<FEImageData *>(GetObjData());
    }

    FEObject *Clone(bool bReference) override { // Decl: 54
        return FNEW FEImage(*this, bReference);
    }

    void SetTopLeft(const FEVector2 &topright, bool bRelative) { // Decl: 56
        SetTrackValue(FETrack_UpperLeft, topright, bRelative);
        Flags |= FF_DirtyCode;
    }
    void SetBottomRight(const FEVector2 &bottomright, bool bRelative) { // Decl: 57
        SetTrackValue(FETrack_LowerRight, bottomright, bRelative);
        Flags |= FF_DirtyCode;
    }
};

#endif
