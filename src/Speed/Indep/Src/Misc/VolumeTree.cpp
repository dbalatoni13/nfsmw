#include "Speed/Indep/bWare/Inc/bMath.hpp"

void bEndianSwap16(void *ptr);
void bEndianSwap32(void *ptr);

struct vAABB {
    float PositionX;          // offset 0x0
    float PositionY;          // offset 0x4
    float PositionZ;          // offset 0x8
    short ParentIndex;        // offset 0xC
    short NumChildren;        // offset 0xE
    float ExtentX;            // offset 0x10
    float ExtentY;            // offset 0x14
    float ExtentZ;            // offset 0x18
    short ChildrenIndicies[10]; // offset 0x1C

    int Contains(float x, float y, float z);
};

struct vAABBTree {
    vAABB *NodeArray;         // offset 0x0
    short NumLeafNodes;       // offset 0x4
    short NumParentNodes;     // offset 0x6
    short TotalNodes;         // offset 0x8
    short Depth;              // offset 0xA
    int pad1;                 // offset 0xC

    void SwapEndian();
    vAABB *QueryLeafHelper(vAABB *aabb, float x, float y, float z);
    vAABB *QueryLeaf(float x, float y, float z);
};

int vAABB::Contains(float x, float y, float z) {
    if (bAbs(x - PositionX) < ExtentX &&
        bAbs(y - PositionY) < ExtentY &&
        bAbs(z - PositionZ) < ExtentZ) {
        return 1;
    }
    return 0;
}

void vAABBTree::SwapEndian() {
    bEndianSwap16(&NumLeafNodes);
    bEndianSwap16(&NumParentNodes);
    bEndianSwap16(&TotalNodes);
    bEndianSwap16(&Depth);
    for (int i = 0; i < TotalNodes; i++) {
        vAABB *node = &NodeArray[i];
        bEndianSwap32(&node->PositionX);
        bEndianSwap32(&node->PositionY);
        bEndianSwap32(&node->PositionZ);
        bEndianSwap16(&node->ParentIndex);
        bEndianSwap16(&node->NumChildren);
        bEndianSwap32(&node->ExtentX);
        bEndianSwap32(&node->ExtentY);
        bEndianSwap32(&node->ExtentZ);
        for (int j = 0; j < 10; j++) {
            bEndianSwap16(&node->ChildrenIndicies[j]);
        }
    }
}

vAABB *vAABBTree::QueryLeafHelper(vAABB *aabb, float x, float y, float z) {
    if (aabb->NumChildren == 0) {
        return aabb;
    }
    for (int i = 0; i < aabb->NumChildren; i++) {
        vAABB *child = &NodeArray[aabb->ChildrenIndicies[i]];
        if (child->Contains(x, y, z)) {
            return QueryLeafHelper(child, x, y, z);
        }
    }
    return nullptr;
}

vAABB *vAABBTree::QueryLeaf(float x, float y, float z) {
    if (TotalNodes == 0) {
        return nullptr;
    }
    vAABB *root = &NodeArray[TotalNodes - 1];
    if (root->Contains(x, y, z)) {
        return QueryLeafHelper(root, x, y, z);
    }
    return nullptr;
}
