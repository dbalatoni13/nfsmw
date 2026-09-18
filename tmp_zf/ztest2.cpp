#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

struct TestGraph {
    TestGraph(char *title, float dimX, float dimY, float maxUnit, int colour, float xpos, float ypos, bool defaultDataSet)
        : mMaxUnit(maxUnit * kFloatScaleUp + kFloatScaleDown) {}
    float mMaxUnit;
};
