#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <math.h>

inline int FLOAT2INT(float f) {
    return static_cast< int >(f);
}

WGrid::WGrid(const UMath::Vector4 &min, unsigned int rows, unsigned int cols, float edgeSize) {
    fMin = min;
    fEdgeSize = edgeSize;
    fInvEdgeSize = 1.0f / edgeSize;
    fNumCols = cols;
    fNumRows = rows;
    fNodes = static_cast<WGridNode **>(bMalloc(cols * 4 * rows, 0));
    for (int i = 0; i < static_cast<int>(rows * cols); i++) {
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

void WGrid::FindNodes(const UMath::Vector4 *seg, UTL::Vector<unsigned int, 16> &nodeIndList) const {
    static int iMaxNumNodes = 100;
    int iNumNodes;
    float fDirX = seg[1].x - seg[0].x;
    float fDirY = seg[1].z - seg[0].z;

    UMath::Vector2 points[2];
    points[0].x = seg[0].x;
    points[0].y = seg[0].z;
    points[1].x = seg[1].x;
    points[1].y = seg[1].z;

    int iStartPosX = static_cast< int >(FLOAT2INT(points[0].x - fMin.x) * fInvEdgeSize);
    int iStartPosY = static_cast< int >(FLOAT2INT(points[0].y - fMin.z) * fInvEdgeSize);

    bool bStartPosOut = false;
    if (iStartPosX < 0 || iStartPosX >= static_cast< int >(fNumCols) ||
        iStartPosY < 0 || iStartPosY >= static_cast< int >(fNumRows)) {
        bStartPosOut = true;
    }

    int iEndPosX = static_cast< int >(FLOAT2INT(seg[1].x - fMin.x) * fInvEdgeSize);
    int iEndPosY = static_cast< int >(FLOAT2INT(seg[1].z - fMin.z) * fInvEdgeSize);

    bool bEndPosOut = false;
    if (iEndPosX < 0 || iEndPosX >= static_cast< int >(fNumCols) ||
        iEndPosY < 0 || iEndPosY >= static_cast< int >(fNumRows)) {
        bEndPosOut = true;
    }

    if (UMath::Abs(fDirX) < 0.05f) {
        fDirX = 0.05f;
    }
    if (UMath::Abs(fDirY) < 0.05f) {
        fDirY = 0.05f;
    }

    if (!bStartPosOut) {
        if (bEndPosOut) {
            float fBarrierPosX;
            float fBarrierPosY;
            float fBarrierDistX;
            float fBarrierDistY;

            if (fDirX <= 0.0f) {
                fBarrierPosX = fMin.x + 0.1f;
            } else {
                fBarrierPosX = static_cast< float >(fNumCols) * fEdgeSize + fMin.x - 0.1f;
            }
            if (fDirY <= 0.0f) {
                fBarrierPosY = fMin.z + 0.1f;
            } else {
                fBarrierPosY = static_cast< float >(fNumRows) * fEdgeSize + fMin.z - 0.1f;
            }

            fBarrierDistX = (fBarrierPosX - points[0].x) / fDirX;
            fBarrierDistY = (fBarrierPosY - points[0].y) / fDirY;

            if (fBarrierDistY <= fBarrierDistX) {
                points[1].x = fBarrierDistY * fDirX + points[0].x;
            } else {
                points[1].y = fBarrierDistX * fDirY + points[0].y;
            }

            iEndPosX = static_cast< int >(FLOAT2INT(points[1].x - fMin.x) * fInvEdgeSize);
            iEndPosY = static_cast< int >(FLOAT2INT(points[1].y - fMin.z) * fInvEdgeSize);
        }
    } else {
        if (bEndPosOut) {
            return;
        }

        float fBarrierPosX;
        float fBarrierPosY;
        float fRevDirX = -fDirX;
        float fRevDirY = -fDirY;
        float fBarrierDistX;
        float fBarrierDistY;

        if (fRevDirX <= 0.0f) {
            fBarrierPosX = fMin.x + 0.1f;
        } else {
            fBarrierPosX = static_cast< float >(fNumCols) * fEdgeSize + fMin.x - 0.1f;
        }
        if (fRevDirY <= 0.0f) {
            fBarrierPosY = fMin.z + 0.1f;
        } else {
            fBarrierPosY = static_cast< float >(fNumRows) * fEdgeSize + fMin.z - 0.1f;
        }

        fBarrierDistX = (fBarrierPosX - points[1].x) / fRevDirX;
        fBarrierDistY = (fBarrierPosY - points[1].y) / fRevDirY;

        if (fBarrierDistY <= fBarrierDistX) {
            points[0].x = fBarrierDistY * fRevDirX + points[1].x;
        } else {
            points[0].y = fBarrierDistX * fRevDirY + points[1].y;
        }

        iStartPosX = static_cast< int >(FLOAT2INT(points[0].x - fMin.x) * fInvEdgeSize);
        iStartPosY = static_cast< int >(FLOAT2INT(points[0].y - fMin.z) * fInvEdgeSize);
    }

    nodeIndList.push_back(GetNodeInd(iStartPosY, iStartPosX));
    iNumNodes = 1;

    float fLength = UMath::Sqrt(fDirX * fDirX + fDirY * fDirY);

    if (fLength <= fEdgeSize) {
        if (iStartPosX == iEndPosX) {
            if (iStartPosY == iEndPosY) {
                return;
            }
        } else if (iStartPosY != iEndPosY) {
            goto dda_traverse;
        }

        nodeIndList.push_back(GetNodeInd(iEndPosY, iEndPosX));
        return;
    }

dda_traverse:
    {
        float fVx = fDirX / fLength;
        float fVy = fDirY / fLength;

        float fInvVx;
        float fInvVy;
        float fRx;
        float fRy;
        float fCurX = points[0].x;
        float fCurY = points[0].y;
        int iCurPosX = iStartPosX;
        int iCurPosY = iStartPosY;
        bool bEast = fVx < 0.0f;
        bool bNorth = fVy < 0.0f;
        float fWallX;
        float fWallY;

        if (UMath::Abs(fVx) >= 0.0001f) {
            fInvVx = 1.0f / fVx;
        } else {
            fInvVx = 10000.0f;
        }

        if (UMath::Abs(fVy) >= 0.0001f) {
            fInvVy = 1.0f / fVy;
        } else {
            fInvVy = 10000.0f;
        }

        if (bEast) {
            fWallX = floorf(fCurX * fInvEdgeSize) * fEdgeSize;
        } else {
            fWallX = ceilf(fCurX * fInvEdgeSize) * fEdgeSize;
        }

        if (bNorth) {
            fWallY = floorf(fCurY * fInvEdgeSize) * fEdgeSize;
        } else {
            fWallY = ceilf(fCurY * fInvEdgeSize) * fEdgeSize;
        }

        if (iCurPosX == iEndPosX) {
            goto walk_y;
        }
        if (iCurPosY == iEndPosY) {
            goto walk_x;
        }

    dda_step:
        fRx = (fWallX - fCurX) * fInvVx;
        fRy = (fWallY - fCurY) * fInvVy;

        if (fRy <= fRx) {
            fCurX = fRy * fVx + fCurX;
            fCurY = fWallY;
            if (bNorth) {
                iCurPosY--;
                fWallY -= fEdgeSize;
            } else {
                iCurPosY++;
                fWallY += fEdgeSize;
            }
        } else {
            fCurY = fRx * fVy + fCurY;
            fCurX = fWallX;
            if (bEast) {
                iCurPosX--;
                fWallX -= fEdgeSize;
            } else {
                iCurPosX++;
                fWallX += fEdgeSize;
            }
        }

        nodeIndList.push_back(GetNodeInd(iCurPosY, iCurPosX));
        iNumNodes++;

        if (iNumNodes > iMaxNumNodes) {
            nodeIndList.clear();
            WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
            return;
        }

        if (iCurPosX != iEndPosX) {
            if (iCurPosY != iEndPosY) {
                goto dda_step;
            }
            goto walk_x;
        }

    walk_y:
        if (iCurPosY == iEndPosY) {
            return;
        }

        if (bNorth) {
            do {
                iCurPosY--;
                if (iCurPosY < iEndPosY) {
                    return;
                }
                nodeIndList.push_back(GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
            } while (iNumNodes <= iMaxNumNodes);
        } else {
            do {
                iCurPosY++;
                if (iEndPosY < iCurPosY) {
                    return;
                }
                nodeIndList.push_back(GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
            } while (iNumNodes <= iMaxNumNodes);
        }

        nodeIndList.clear();
        WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
        return;

    walk_x:
        if (iCurPosX == iEndPosX) {
            goto walk_y;
        }

        if (bEast) {
            do {
                iCurPosX--;
                if (iCurPosX < iEndPosX) {
                    return;
                }
                nodeIndList.push_back(GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
            } while (iNumNodes <= iMaxNumNodes);
        } else {
            do {
                iCurPosX++;
                if (iEndPosX < iCurPosX) {
                    return;
                }
                nodeIndList.push_back(GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
            } while (iNumNodes <= iMaxNumNodes);
        }

        nodeIndList.clear();
        WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
    }
}