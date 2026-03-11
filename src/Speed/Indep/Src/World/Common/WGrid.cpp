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

void WGrid::Shutdown() {
    if (fgGrid != nullptr) {
        for (int i = 0; i < static_cast<int>(fgGrid->fNumRows * fgGrid->fNumCols); i++) {
            WGridNode *node = fgGrid->fNodes[i];
            if (node != nullptr) {
                node->ShutDown();
                fgGrid->fNodes[i] = nullptr;
            }
        }
        delete fgGrid;
        fgGrid = nullptr;
    }
    fgMapGroup = nullptr;
}

void WGrid::FindNodes(const UMath::Vector3 &pt, float radius, UTL::Vector<unsigned int, 16> &nodeIndList) const {
    UMath::Vector4 pts[2];
    pts[0].x = pt.x - radius;
    pts[0].z = pt.z - radius;
    pts[0].y = 0.0f;
    pts[1].x = pt.x + radius;
    pts[1].z = pt.z + radius;
    pts[1].y = 0.0f;
    FindNodesBox(pts, nodeIndList);
}