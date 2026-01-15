#ifndef WORLD_WWORLDMATH_H
#define WORLD_WWORLDMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace WWorldMath {

bool IntersectCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &u1, float &u2);

};

#endif
