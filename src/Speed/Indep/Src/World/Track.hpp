#ifndef WORLD_TRACK_H
#define WORLD_TRACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x8
struct TopologyCoordinate : public bTNode<TopologyCoordinate> {
    TopologyCoordinate() {}

    ~TopologyCoordinate() {}

    void SetInterestBBox(const bVector3 *position, float radius, const bVector3 *velocity) {}

    bool IsLoaded() { return false; }
};

void EstablishRemoteCaffeineConnection();

#endif
