#ifndef MATH_SIMRANDOM_H
#define MATH_SIMRANDOM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x10
class SimRandom {
  public:
    SimRandom();
    ~SimRandom();
    void Reset();

  private:
    unsigned int fastRandom;   // offset 0x0, size 0x4
    unsigned int randtemp;     // offset 0x4, size 0x4
    unsigned int randSeed;     // offset 0x8, size 0x4
    unsigned int numRandCalls; // offset 0xC, size 0x4
};

#endif
