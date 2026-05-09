#ifndef GAMEPLAY_GMARKER_H
#define GAMEPLAY_GMARKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"

class GMarker : /* 0x00 */ GRuntimeInstance { // 0x50
  public:
    GMarker(const Attrib::Key &markerKey);
    /* vtable[1] */ ~GMarker() override;
    /* vtable[2] */ GameplayObjType GetType() override {};
    UMath::Vector3 &GetPosition() {
        return mPosition;
    };
    UMath::Vector3 &GetDirection() {
        return mDirection;
    };
    void CalcTransform(struct UMath::Matrix4 &mat) const;

  private:
    /* 0x30 */ UMath::Vector3 mPosition;
    /* 0x40 */ UMath::Vector3 mDirection;
};

#endif
