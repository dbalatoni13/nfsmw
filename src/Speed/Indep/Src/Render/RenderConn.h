#ifndef RENDER_RENDERCONN_H
#define RENDER_RENDERCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/SmackableRender.hpp"

namespace RenderConn {

class Pkt_Smackable_Open : public Sim::Packet {
  public:
    // total size: 0x18
    bHash32 mModelHash;                              // offset 0x4, size 0x4
    unsigned int mObjectWUID;                        // offset 0x8, size 0x4
    const CollisionGeometry::Bounds *mCollisionNode; // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy;                // offset 0x10, size 0x4
    unsigned int mRenderNode;                        // offset 0x14, size 0x4
};

class Pkt_Smackable_Service : public Sim::Packet {
  public:
    Pkt_Smackable_Service(bool visible, float distancetoview) {
        this->mVisible = visible;
        this->mDistanceToView = distancetoview;
        this->mChildVisibility = 0xFFFFFF;
    }

    // total size: 0x10
    bool mVisible;                 // offset 0x4, size 0x1
    float mDistanceToView;         // offset 0x8, size 0x4
    unsigned int mChildVisibility; // offset 0xC, size 0x4
};

void UpdateServices(float dT);
void UpdateLoading();

} // namespace RenderConn

#endif
