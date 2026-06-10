#ifndef FEANIMIMAGE_H_
#define FEANIMIMAGE_H_

#include "FEImage.h"

// File: speed/indep/src/feng/FEAnimImage.h
// total size: 0x58
// Decl: speed/indep/src/feng/FEAnimImage.h:25
class FEAnimImageData : public FEImageData {
  public:
    i32 FrameNum; // offset 0x54, size 0x4, Decl: speed/indep/src/feng/FEAnimImage.h:27
};

// total size: 0x60
// Decl: speed/indep/src/feng/FEAnimImage.h:35
class FEAnimImage : public FEImage {
  public:
    FEAnimImage() {} // Decl: speed/indep/src/feng/FEAnimImage.h:37
    FEAnimImage(const FEAnimImage &Object, bool bReference) : FEImage(Object, bReference) {}
    ~FEAnimImage() override {}

    FEAnimImageData *GetImageData() {} // Decl: speed/indep/src/feng/FEAnimImage.h:41

    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FEAnimImage.h:43

    void SetTopLeft(const FEVector2 &topright, bool bRelative) {
        SetTrackValue(FETrack_UpperLeft, topright, bRelative);
        Flags |= 0x400000;
    }

    void SetBottomRight(const FEVector2 &bottomright, bool bRelative) {
        SetTrackValue(FETrack_LowerRight, bottomright, bRelative);
        Flags |= 0x400000;
    }
};

#endif
