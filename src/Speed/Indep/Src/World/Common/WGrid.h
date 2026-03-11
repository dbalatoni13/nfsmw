#ifndef WORLD_COMMON_WGRID_H
#define WORLD_COMMON_WGRID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/World/Common/WGridNode.h"

struct UGroup;

struct WGrid {
    static bool Initialized() { return fgGrid != 0; }
    static WGrid &Get() { return *fgGrid; }

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

    inline bool IsValidNode(unsigned short nodeInd) {
        return nodeInd < fNumRows * fNumCols;
    }

    inline unsigned int GetNodeInd(unsigned int row, unsigned int col) const {
        return row * fNumCols + col;
    }

    inline WGridNode *GetNode(unsigned int ind) const {
        return fNodes[ind];
    }

    inline WGridNode *GetNode(unsigned int row, unsigned int col) const {
        return fNodes[GetNodeInd(row, col)];
    }

    static WGrid *fgGrid;
    static const UGroup *fgMapGroup;

    UMath::Vector4 fMin;       // offset 0x0, size 0x10
    float fEdgeSize;           // offset 0x10, size 0x4
    float fInvEdgeSize;        // offset 0x14, size 0x4
    unsigned int fNumRows;     // offset 0x18, size 0x4
    unsigned int fNumCols;     // offset 0x1C, size 0x4
    WGridNode **fNodes;        // offset 0x20, size 0x4
};

#endif
