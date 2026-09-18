#ifndef GAMEPLAY_GMARKER_H
#define GAMEPLAY_GMARKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"

struct GMarker : public GRuntimeInstance {
    void CalcTransform(UMath::Matrix4 &mat) const;

    GMarker(const unsigned int &markerKey);

    ~GMarker() override;

    GameplayObjType GetType() const override { return kGameplayObjType_Marker; }

    const UMath::Vector3 &GetPosition() const { return mPosition; }
    const UMath::Vector3 &GetDirection() const { return mDirection; }

    UMath::Vector3 mPosition;  // offset 0x28, size 0xC
    UMath::Vector3 mDirection; // offset 0x34, size 0xC
};



#endif
