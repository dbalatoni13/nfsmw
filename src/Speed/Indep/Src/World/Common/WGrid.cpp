#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

WGrid::WGrid(const UMath::Vector4 &min, unsigned int rows, unsigned int cols, float edgeSize) {
    fMin = min;
    fEdgeSize = edgeSize;
    fInvEdgeSize = 1.0f / edgeSize;
    fNumRows = rows;
    fNumCols = cols;
    fNodes = static_cast<WGridNode **>(bMalloc(cols * 4 * rows, 0));
    for (int i = 0; i < static_cast<int>(cols * rows); i++) {
        fNodes[i] = 0;
    }
}

WGrid::~WGrid() {
    bFree(fNodes);
}

void WGrid::Init(const UGroup *mapGroup) {
    fgMapGroup = mapGroup;
    if (mapGroup != nullptr) {
        const UGroup *cDatGroup = mapGroup->GroupLocate('CD', 'at');
        const UData *carpGridData = cDatGroup->DataLocate('CG', 'rd');
        const WGrid *carpGrid = static_cast<const WGrid *>(carpGridData->GetDataConst());

        if (carpGrid->fNumRows == 0 || carpGrid->fNumCols == 0) {
            fgGrid = new WGrid(UMath::Vector4Make(-10000.0f, -10000.0f, -10000.0f, 1.0f), 2, 2, 10000.0f);
        } else {
            fgGrid = new WGrid(carpGrid->fMin, carpGrid->fNumRows, carpGrid->fNumCols, carpGrid->fEdgeSize);
        }

        unsigned int numNodes = cDatGroup->DataCountType(UDataGroupTag('CG', 'cn'));
        const UData *nodeDat = cDatGroup->DataLocateFirst(UDataGroupTag('CG', 'cn'), 0xFFFFFFFF, 0xFFFFFFFF);
        if (nodeDat != cDatGroup->DataEnd()) {
            const WGridNode *n = static_cast<const WGridNode *>(nodeDat->GetDataConst());
            for (unsigned int i = 0; i < nodeDat->DataCount(); i++) {
                fgGrid->fNodes[n->GetNodeInd()] = const_cast<WGridNode *>(n);
                n = reinterpret_cast<const WGridNode *>(reinterpret_cast<const char *>(n) + n->TotalSize());
            }
        }
    } else {
        fgGrid = new WGrid(UMath::Vector4Make(-10000.0f, -10000.0f, -10000.0f, 1.0f), 2, 2, 10000.0f);
    }
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
    pts[1].y = 0.0f;
    pts[1].x = pt.x + radius;
    pts[1].z = pt.z + radius;
    pts[0].y = 0.0f;
    FindNodesBox(pts, nodeIndList);
}

void WGrid::FindNodesBox(const UMath::Vector4 *pts, UTL::Vector<unsigned int, 16> &nodeIndList) const {
    unsigned int colMin;
    unsigned int rowMin;
    unsigned int colMax;
    unsigned int rowMax;

    GetRowCol(UMath::Vector4To3(pts[0]), rowMin, colMin);
    GetRowCol(UMath::Vector4To3(pts[1]), rowMax, colMax);

    if (rowMin > rowMax) {
        unsigned int temp = rowMin;
        rowMin = rowMax;
        rowMax = temp;
    }
    if (colMin > colMax) {
        unsigned int temp = colMin;
        colMin = colMax;
        colMax = temp;
    }

    if (rowMax - rowMin > 20) {
        rowMax = rowMin;
    }
    if (colMax - colMin > 20) {
        colMax = colMin;
    }

    RangeCheckROWCOL(rowMin, colMin);
    RangeCheckROWCOL(rowMax, colMax);

    for (unsigned int col = colMin; col <= colMax; col++) {
        for (unsigned int row = rowMin; row <= rowMax; row++) {
            nodeIndList.push_back(GetNodeInd(row, col));
        }
    }
}