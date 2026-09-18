#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

struct DebugGraph {
    DebugGraph(char *title, float dimX, float dimY, float maxUnit, int colour, float xpos, float ypos, bool defaultDataSet)
        : mMaxUnit(maxUnit * kFloatScaleUp + kFloatScaleDown) {}
    float mMaxUnit;
};

static DebugGraph g_graph("t", 1, 2, 3, 4, 5, 6, true);
