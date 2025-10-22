#ifndef RENDER_RENDERCONN_H
#define RENDER_RENDERCONN_H

#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/SmackableRender.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Sim {

class Pkt_Smackable_Open : public Sim::Packet {
public:
    // total size: 0x18
    bHash32 mModelHash; // offset 0x4, size 0x4
    unsigned int mObjectWUID; // offset 0x8, size 0x4
    const CollisionGeometry::Bounds *mCollisionNode; // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy; // offset 0x10, size 0x4
    unsigned int mRenderNode; // offset 0x14, size 0x4
};

class Pkt_Smackable_Service : public Packet {
public:
    Pkt_Smackable_Service(bool visible, float distancetoview) {}

    // total size: 0x10
    bool mVisible; // offset 0x4, size 0x1
    float mDistanceToView; // offset 0x8, size 0x4
    unsigned int mChildVisibility; // offset 0xC, size 0x4
};

}

#endif
