#ifndef MISC_VOLUMETREE_H
#define MISC_VOLUMETREE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

class vAABB {
  public:
    float PositionX;            // offset 0x0
    float PositionY;            // offset 0x4
    float PositionZ;            // offset 0x8
    int16 ParentIndex;          // offset 0xC
    int16 NumChildren;          // offset 0xE
    float ExtentX;              // offset 0x10
    float ExtentY;              // offset 0x14
    float ExtentZ;              // offset 0x18
    int16 ChildrenIndicies[10]; // offset 0x1C

    void Empty();
    void Create();
    // void Create();
    float GetVolume();
    int Contains(float x, float y, float z);
    int Encloses();
    int Intersects();
};

class vAABBTree {
  public:
    void SwapEndian();
    vAABB *QueryLeaf(float x, float y, float z);

    vAABB *NodeArray;     // offset 0x0, size 0x4
    int16 NumLeafNodes;   // offset 0x4, size 0x2
    int16 NumParentNodes; // offset 0x6, size 0x2
    int16 TotalNodes;     // offset 0x8, size 0x2
    int16 Depth;          // offset 0xA, size 0x2
    int32 pad1;           // offset 0xC, size 0x4

  private:
    vAABB *QueryLeafHelper(vAABB *aabb, float x, float y, float z);
};

#endif
