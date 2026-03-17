#ifndef GAMEPLAY_GMARKER_H
#define GAMEPLAY_GMARKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"

// total size: 0x40
class GMarker : public GRuntimeInstance {
  public:
    GMarker(const Attrib::Key &markerKey);

    ~GMarker() override;

    GameplayObjType GetType() const override {
        return kGameplayObjType_Marker;
    }

    const UMath::Vector3 &GetPosition() const {
        return mPosition;
    }

    const UMath::Vector3 &GetDirection() const {
        return mDirection;
    }

    void CalcTransform(UMath::Matrix4 &mat) const;

  private:
    UMath::Vector3 mPosition;  // offset 0x28, size 0xC
    UMath::Vector3 mDirection; // offset 0x34, size 0xC
};

#endif
