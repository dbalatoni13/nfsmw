#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <math.h>

WGrid *WGrid::fgGrid = nullptr;            // size: 0x4, address: 0x80438F74
const UGroup *WGrid::fgMapGroup = nullptr; // size: 0x4, address: 0x80438F78
unsigned int WGrid::fNumGrids = 0;         // size: 0x4, address: 0xFFFFFFFF

WGrid::WGrid(const UMath::Vector4 &min, unsigned int rows, unsigned int cols, float edgeSize)
    : fMin(min),                     //
      fEdgeSize(edgeSize),           //
      fInvEdgeSize(1.0f / edgeSize), //
      fNumRows(rows),                //
      fNumCols(cols)                 //
{
    fNodes = static_cast<WGridNode **>(Alloc(sizeof(WGridNode *) * rows * cols, 0, "WGrid Nodes"));
    // TODO likely a macro because of the cast?
    for (int i = 0; i < static_cast<int>(rows * cols); i++) {
        fNodes[i] = nullptr;
    }
}

WGrid::~WGrid() {
    bFree(fNodes);
}

void WGrid::Init(const UGroup *mapGroup) {
    fgMapGroup = mapGroup;
    if (mapGroup != nullptr) {
        const UGroup *cDatGroup = mapGroup->GroupLocate(MAKE_UDATA_TYPE('CD'), 'at');
        const UData *carpGridData = cDatGroup->DataLocate(MAKE_UDATA_TYPE('CG'), 'rd');
        const WGrid *carpGrid = static_cast<const WGrid *>(carpGridData->GetDataConst());

        if (carpGrid->fNumRows == 0 || carpGrid->fNumCols == 0) {
            fgGrid = new ("WGrid", 0) WGrid(UMath::Vector4Make(-10000.0f, -10000.0f, -10000.0f, 1.0f), 2, 2, 10000.0f);
        } else {
            fgGrid = new ("WGrid", 0) WGrid(carpGrid->fMin, carpGrid->fNumRows, carpGrid->fNumCols, carpGrid->fEdgeSize);
        }

        {
            unsigned int numNodes = cDatGroup->DataCountType(MAKE_UDATA_TYPE('CG') | 'cn');
            const UData *nodeDat = cDatGroup->DataLocateFirst(MAKE_UDATA_TYPE('CG') | 'cn', -1, -1);
            if (nodeDat != cDatGroup->DataEnd()) {
                const WGridNode *n = static_cast<const WGridNode *>(nodeDat->GetDataConst());
                for (unsigned int i = 0; i < nodeDat->DataCount(); i++) {
                    fgGrid->fNodes[n->GetNodeInd()] = const_cast<WGridNode *>(n);
                    n = reinterpret_cast<const WGridNode *>(reinterpret_cast<const char *>(n) + n->TotalSize());
                }
            }
        }
    } else {
        fgGrid = new WGrid(UMath::Vector4Make(-10000.0f, -10000.0f, -10000.0f, 1.0f), 2, 2, 10000.0f);
    }
}

void WGrid::Shutdown() {
    if (fgGrid != nullptr) {
        for (int i = 0; i < static_cast<int>(fgGrid->fNumRows * fgGrid->fNumCols); i++) {
            if (fgGrid->fNodes[i]) {
                fgGrid->fNodes[i]->ShutDown();
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
    int iNumNodes;
    static int iMaxNumNodes = 100;
    float fDirX;
    float fDirY;
    int iStartPosX;
    int iStartPosY;
    int iEndPosX;
    int iEndPosY;
    UMath::Vector2 points[2];

    points[0].x = seg[0].x;
    points[0].y = seg[0].z;
    points[1].x = seg[1].x;
    points[1].y = seg[1].z;

    fDirX = points[1].x - points[0].x;
    fDirY = points[1].y - points[0].y;

    iStartPosX = FLOAT2INT(FLOAT2INT(points[0].x - this->fMin.x) * this->fInvEdgeSize);
    iStartPosY = FLOAT2INT(FLOAT2INT(points[0].y - this->fMin.z) * this->fInvEdgeSize);

    bool bStartPosOut = false;
    if (iStartPosX < 0 || iStartPosX >= static_cast<int>(this->fNumCols) || iStartPosY < 0 || iStartPosY >= static_cast<int>(this->fNumRows)) {
        bStartPosOut = true;
    }

    iEndPosX = FLOAT2INT(FLOAT2INT(points[1].x - this->fMin.x) * this->fInvEdgeSize);
    iEndPosY = FLOAT2INT(FLOAT2INT(points[1].y - this->fMin.z) * this->fInvEdgeSize);

    bool bEndPosOut = false;
    if (iEndPosX < 0 || iEndPosX >= static_cast<int>(this->fNumCols) || iEndPosY < 0 || iEndPosY >= static_cast<int>(this->fNumRows)) {
        bEndPosOut = true;
    }

    if (fabsf(fDirX) < 0.05f) {
        fDirX = 0.05f;
    }
    if (fabsf(fDirY) < 0.05f) {
        fDirY = 0.05f;
    }

    if (bStartPosOut) {
        if (bEndPosOut) {
            return;
        } else {
            float fBarrierPosX;
            float fBarrierPosY;
            float fRevDirX = -fDirX;
            float fRevDirY = -fDirY;

            if (fRevDirX > 0.0f) {
                fBarrierPosX = static_cast<float>(this->fNumCols) * this->fEdgeSize + this->fMin.x - 0.1f;
            } else {
                fBarrierPosX = this->fMin.x + 0.1f;
            }
            if (fRevDirY > 0.0f) {
                fBarrierPosY = static_cast<float>(this->fNumRows) * this->fEdgeSize + this->fMin.z - 0.1f;
            } else {
                fBarrierPosY = this->fMin.z + 0.1f;
            }

            float fBarrierDistX = (fBarrierPosX - points[1].x) / fRevDirX;
            float fBarrierDistY = (fBarrierPosY - points[1].y) / fRevDirY;

            if (fBarrierDistX < fBarrierDistY) {
                points[0].x = fBarrierPosX;
                points[0].y = fBarrierDistX * fRevDirY + points[1].y;
            } else {
                points[0].y = fBarrierPosY;
                points[0].x = fBarrierDistY * fRevDirX + points[1].x;
            }

            iStartPosX = FLOAT2INT(FLOAT2INT(points[0].x - this->fMin.x) * this->fInvEdgeSize);
            iStartPosY = FLOAT2INT(FLOAT2INT(points[0].y - this->fMin.z) * this->fInvEdgeSize);
        }
    } else {
        if (bEndPosOut) {
            float fBarrierPosX;
            if (fDirX > 0.0f) {
                fBarrierPosX = static_cast<float>(this->fNumCols) * this->fEdgeSize + this->fMin.x - 0.1f;
            } else {
                fBarrierPosX = this->fMin.x + 0.1f;
            }

            float fBarrierPosY;
            if (fDirY > 0.0f) {
                fBarrierPosY = static_cast<float>(this->fNumRows) * this->fEdgeSize + this->fMin.z - 0.1f;
            } else {
                fBarrierPosY = this->fMin.z + 0.1f;
            }

            float fBarrierDistX = (fBarrierPosX - points[0].x) / fDirX;
            float fBarrierDistY = (fBarrierPosY - points[0].y) / fDirY;

            if (fBarrierDistX < fBarrierDistY) {
                points[1].x = fBarrierPosX;
                points[1].y = fBarrierDistX * fDirY + points[0].y;
            } else {
                points[1].y = fBarrierPosY;
                points[1].x = fBarrierDistY * fDirX + points[0].x;
            }

            iEndPosX = FLOAT2INT(FLOAT2INT(points[1].x - this->fMin.x) * this->fInvEdgeSize);
            iEndPosY = FLOAT2INT(FLOAT2INT(points[1].y - this->fMin.z) * this->fInvEdgeSize);
        }
    }

    nodeIndList.push_back(this->GetNodeInd(iStartPosY, iStartPosX));
    iNumNodes = 1;

    float fLength = UMath::Sqrt(fDirX * fDirX + fDirY * fDirY);

    if (fLength <= this->fEdgeSize && (iStartPosX == iEndPosX || iStartPosY == iEndPosY)) {
        if (iStartPosX != iEndPosX || iStartPosY != iEndPosY) {
            nodeIndList.push_back(this->GetNodeInd(iEndPosY, iEndPosX));
        }
    } else {
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
        bool bEast;
        bool bNorth;
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

        bEast = fVx >= 0.0f;
        bNorth = fVy >= 0.0f;

        if (bEast) {
            fWallX = ceilf(fCurX * this->fInvEdgeSize) * this->fEdgeSize;
        } else {
            fWallX = floorf(fCurX * this->fInvEdgeSize) * this->fEdgeSize;
        }

        if (bNorth) {
            fWallY = ceilf(fCurY * this->fInvEdgeSize) * this->fEdgeSize;
        } else {
            fWallY = floorf(fCurY * this->fInvEdgeSize) * this->fEdgeSize;
        }

        while (iCurPosX != iEndPosX && iCurPosY != iEndPosY) {
            fRx = (fWallX - fCurX) * fInvVx;
            fRy = (fWallY - fCurY) * fInvVy;

            if (fRx < fRy) {
                fCurX = fWallX;
                fCurY = fRx * fVy + fCurY;
                if (bEast) {
                    iCurPosX++;
                    fWallX += this->fEdgeSize;
                } else {
                    iCurPosX--;
                    fWallX -= this->fEdgeSize;
                }
            } else {
                fCurX = fRy * fVx + fCurX;
                fCurY = fWallY;
                if (bNorth) {
                    iCurPosY++;
                    fWallY += this->fEdgeSize;
                } else {
                    iCurPosY--;
                    fWallY -= this->fEdgeSize;
                }
            }

            nodeIndList.push_back(this->GetNodeInd(iCurPosY, iCurPosX));
            iNumNodes++;

            if (iNumNodes > iMaxNumNodes) {
                nodeIndList.clear();
                WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
                return;
            }
        }

        if (iCurPosX != iEndPosX) {
            if (bEast) {
                while (++iCurPosX <= iEndPosX) {
                    nodeIndList.push_back(this->GetNodeInd(iCurPosY, iCurPosX));
                    iNumNodes++;
                    if (iNumNodes > iMaxNumNodes) {
                        nodeIndList.clear();
                        WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
                        return;
                    }
                }
                return;
            } else {
                while (--iCurPosX >= iEndPosX) {
                    nodeIndList.push_back(this->GetNodeInd(iCurPosY, iCurPosX));
                    iNumNodes++;
                    if (iNumNodes > iMaxNumNodes) {
                        nodeIndList.clear();
                        WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
                        return;
                    }
                }
                return;
            }
        }

        if (iCurPosY == iEndPosY) {
            return;
        }

        if (bNorth) {
            while (++iCurPosY <= iEndPosY) {
                nodeIndList.push_back(this->GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
                if (iNumNodes > iMaxNumNodes) {
                    nodeIndList.clear();
                    WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
                    return;
                }
            }
        } else {
            while (--iCurPosY >= iEndPosY) {
                nodeIndList.push_back(this->GetNodeInd(iCurPosY, iCurPosX));
                iNumNodes++;
                if (iNumNodes > iMaxNumNodes) {
                    nodeIndList.clear();
                    WGrid::Get().FindNodes(UMath::Vector4To3(seg[0]), 1.0f, nodeIndList);
                    return;
                }
            }
        }
    }
}
