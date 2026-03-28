#ifndef MISC_VOLUMETREE_H
#define MISC_VOLUMETREE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class vAABB {
  public:
    float PositionX;            // offset 0x0
    float PositionY;            // offset 0x4
    float PositionZ;            // offset 0x8
    short ParentIndex;          // offset 0xC
    short NumChildren;          // offset 0xE
    float ExtentX;              // offset 0x10
    float ExtentY;              // offset 0x14
    float ExtentZ;              // offset 0x18
    short ChildrenIndicies[10]; // offset 0x1C

    int Contains(float x, float y, float z);
};

class vAABBTree {
  public:
    void SwapEndian();
    vAABB *QueryLeaf(float x, float y, float z);

    vAABB *NodeArray;     // offset 0x0, size 0x4
    short NumLeafNodes;   // offset 0x4, size 0x2
    short NumParentNodes; // offset 0x6, size 0x2
    short TotalNodes;     // offset 0x8, size 0x2
    short Depth;          // offset 0xA, size 0x2
    int pad1;             // offset 0xC, size 0x4

  private:
    vAABB *QueryLeafHelper(vAABB *aabb, float x, float y, float z);
};

#endif
