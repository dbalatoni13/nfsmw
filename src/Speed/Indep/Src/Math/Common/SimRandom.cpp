#include "Speed/Indep/Src/Math/SimRandom.h"

SimRandom::SimRandom() {
    this->randSeed = 0x075BCD15;
}

SimRandom::~SimRandom() {}

void SimRandom::Reset() {
    this->randtemp = 0;
    this->numRandCalls = 0;

    this->fastRandom = 0xF874AF01;
    int seedIterations = this->randSeed % 50;

    for (int i = 0; i < seedIterations; i++) {
        this->SimRandom_Generate();
    }
}

int SimRandom::SimRandom_Generate() {
    this->numRandCalls++;
    this->randtemp = (this->fastRandom * this->randSeed);
    this->fastRandom = this->randtemp & 0xFFFF;

    return (this->randtemp >> 8) & 0xFFFF;
}
