#ifndef _PATH_SPOT
#define _PATH_SPOT

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class WRoadNav;

// total size: 0x10
struct path_spot {
    short segmentindex; // offset 0x0, size 0x2
    int nodeind;        // offset 0x4, size 0x4
    float param;        // offset 0x8, size 0x4
    float laneoffset;   // offset 0xC, size 0x4

    path_spot()
        : segmentindex(-1) //
        , nodeind(0) //
        , param(0.0f) //
        , laneoffset(0.0f) {}

    path_spot(short s, int i, float p, float l)
        : segmentindex(s) //
        , nodeind(i) //
        , param(p) //
        , laneoffset(l) {}

    path_spot(const WRoadNav &nav);

    void init_nav(WRoadNav &nav) const;
    void init_nav(WRoadNav &nav, const UMath::Vector3 &point) const;
};

#endif
