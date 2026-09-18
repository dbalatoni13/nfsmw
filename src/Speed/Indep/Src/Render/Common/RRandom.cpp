// RRandom.cpp - storage for the render random number generator state.

#include "Speed/Indep/Src/Render/Common/RRandom.h"

unsigned int RRandom::fastRandom = 0xF874AF01;
unsigned int RRandom::randSeed = 1;
