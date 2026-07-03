#ifndef FEIMAGE_H_
#define FEIMAGE_H_

#include "FEObject.h"

// File: speed/indep/src/feng/feimage.h
// total size: 0x54
// Decl: speed/indep/src/feng/feimage.h:33
class FEImageData : public FEObjData {
  public:
    FEVector2 UpperLeft;  // offset 0x44, size 0x8, Decl: speed/indep/src/feng/feimage.h:35
    FEVector2 LowerRight; // offset 0x4C, size 0x8, Decl: speed/indep/src/feng/feimage.h:36
};

// total size: 0x60
// Decl: speed/indep/src/feng/feimage.h:44
class FEImage : public FEObject {
  public:
    u32 ImageFlags; // offset 0x5C, size 0x4, Decl: speed/indep/src/feng/feimage.h:46

    FEImage() : FEObject(), ImageFlags(0) { // Decl: speed/indep/src/feng/feimage.h:48
        Type = FE_Image;
    }
    FEImage(const FEImage &Object, bool bReference) : FEObject(Object, bReference), ImageFlags(Object.ImageFlags) {
        Type = FE_Image;
    }
    ~FEImage() override {} // Decl: speed/indep/src/feng/feimage.h:50

    FEImageData *GetImageData() {} // Decl: speed/indep/src/feng/feimage.h:52

    FEObject *Clone(bool bReference) override { // Decl: speed/indep/src/feng/feimage.h:54
        return FNEW FEImage(*this, bReference);
    }

    void SetTopLeft(const FEVector2 &topright, bool bRelative) { // Decl: speed/indep/src/feng/feimage.h:56
        SetTrackValue(FETrack_UpperLeft, topright, bRelative);
        Flags |= FF_DirtyCode;
    }
    void SetBottomRight(const FEVector2 &bottomright, bool bRelative) { // Decl: speed/indep/src/feng/feimage.h:57
        SetTrackValue(FETrack_LowerRight, bottomright, bRelative);
        Flags |= FF_DirtyCode;
    }
};

#endif
