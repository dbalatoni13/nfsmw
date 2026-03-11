#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

WGrid::WGrid(const UMath::Vector4 &min, unsigned int rows, unsigned int cols, float edgeSize)  {
    fMin = min;
    fEdgeSize = edgeSize;
    fInvEdgeSize = 1.0f / edgeSize;
    fNumRows = rows;
    fNumCols = cols;
    fNodes = static_cast<WGridNode **>(bMalloc(rows * cols * 4, 0));
    for (int i = 0; i < rows * cols; i++) {
        fNodes[i] = 0;
    }
}

WGrid::~WGrid() {
    bFree(fNodes);
}