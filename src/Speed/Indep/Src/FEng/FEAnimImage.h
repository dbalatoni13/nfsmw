#ifndef FENG_FEANIMIMAGE_H
#define FENG_FEANIMIMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "feimage.h"

struct FEAnimImageData;

// total size: 0x60
struct FEAnimImage : public FEImage {
    inline FEAnimImage() {}
    inline FEAnimImage(const FEAnimImage& Object, bool bReference)
        : FEImage(Object, bReference) {}
    ~FEAnimImage() override;

    inline FEAnimImageData* GetImageData();

    FEObject* Clone(bool bReference) override;

    inline void SetTopLeft(const FEVector2& topleft, bool bRelative) {
        SetTrackValue(FETrack_UpperLeft, topleft, bRelative);
        Flags |= 0x400000;
    }

    inline void SetBottomRight(const FEVector2& bottomright, bool bRelative) {
        SetTrackValue(FETrack_LowerRight, bottomright, bRelative);
        Flags |= 0x400000;
    }
};

#endif
