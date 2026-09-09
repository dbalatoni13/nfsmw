#ifndef WORLD_COMMON_WGRID_H
#define WORLD_COMMON_WGRID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/World/Common/WGridNode.h"

class WGrid {
  public:
    static bool Initialized() {
        return fgGrid != nullptr;
    }
    static WGrid &Get() {
        return *fgGrid;
    }

    WGrid(const UMath::Vector4 &min, unsigned int rows, unsigned int cols, float edgeSize);
    ~WGrid();

    static void Init(const UGroup *mapGroup);
    static void Shutdown();
    static void Restart();

    void FindNodes(const UMath::Vector3 &pt, float radius, UTL::Vector<unsigned int, 16> &nodeIndList) const;
    void FindNodesBox(const UMath::Vector4 *pts, UTL::Vector<unsigned int, 16> &nodeIndList) const;
    void FindNodes(const UMath::Vector4 *seg, UTL::Vector<unsigned int, 16> &nodeIndList) const;
    void FindNodes(const UMath::Vector4 &min, const UMath::Vector4 &max, UTL::Vector<unsigned int, 16> &nodeIndList) const;
    void FindNodes(const UMath::Vector3 &pt, float radiusInner, float radiusOuter, UTL::Vector<unsigned int, 16> &nodeIndList) const;
    bool RangeCheck(const UMath::Vector3 *pts) const;

    bool IsValidNode(unsigned short nodeInd) {
        return nodeInd < this->fNumRows * this->fNumCols;
    }

    void RangeCheckROWCOL(unsigned int &row, unsigned int &col) const {
        if (col >= this->fNumCols) {
            if (col > 0x7FFFFFFEU) {
                col = 0;
            } else {
                col = this->fNumCols - 1;
            }
        }
        if (row >= this->fNumRows) {
            if (row > 0x7FFFFFFEU) {
                row = 0;
            } else {
                row = this->fNumRows - 1;
            }
        }
    }

    unsigned int GetNodeInd(unsigned int row, unsigned int col) const {
        return row * this->fNumCols + col;
    }

    void GetRowCol(const UMath::Vector3 &pt, unsigned int &row, unsigned int &col) const {
        col = FLOAT2INT((pt.x - this->fMin.x) * this->fInvEdgeSize);
        row = FLOAT2INT((pt.z - this->fMin.z) * this->fInvEdgeSize);
        this->RangeCheckROWCOL(row, col);
    }

    void GetRowCol(unsigned int ind, unsigned int &row, unsigned int &col) const {
        row = ind / this->fNumCols;
        col = ind - row * this->fNumCols;
        this->RangeCheckROWCOL(row, col);
    }

    WGridNode *GetNode(unsigned int row, unsigned int col) const {
        return this->fNodes[this->GetNodeInd(row, col)];
    }

    WGridNode *GetNode(unsigned int ind) const {
        unsigned int row, col;
        this->GetRowCol(ind, row, col);
        return this->GetNode(row, col);
    }

    UMath::Vector4 fMin;   // offset 0x0, size 0x10
    float fEdgeSize;       // offset 0x10, size 0x4
    float fInvEdgeSize;    // offset 0x14, size 0x4
    unsigned int fNumRows; // offset 0x18, size 0x4
    unsigned int fNumCols; // offset 0x1C, size 0x4
    WGridNode **fNodes;    // offset 0x20, size 0x4

  private:
    static WGrid *fgGrid;
    static const UGroup *fgMapGroup;
    static unsigned int fNumGrids;
};

#endif
