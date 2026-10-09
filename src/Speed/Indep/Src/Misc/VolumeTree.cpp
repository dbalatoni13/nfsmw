#include "VolumeTree.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define __abs bAbs // Decl: 59

// STRIPPED
void vAABB::Empty() {}

// STRIPPED
void vAABB::Create(bVector3 *position, bVector3 *extent) {}

// STRIPPED
void vAABB::Create(vSphere *vsphere) {}

// STRIPPED
float vAABB::GetVolume() {}

// TODO Dwarf non-matching
int vAABB::Contains(float x, float y, float z) {
    float delta_x = x - PositionX;
    float delta_y = y - PositionY;
    float delta_z = z - PositionZ;
    float ax = bAbs(delta_x);
    float extent_x = ExtentX;
    float ay = bAbs(delta_y);
    float extent_y = ExtentY;
    float az = bAbs(delta_z);
    float extent_z = ExtentZ;
    if (ax >= extent_x || ay >= extent_y || az >= extent_z) {
        return 0;
    }
    return 1;
}

// STRIPPED
int vAABB::Encloses(vAABB *test_aabb) {}

// STRIPPED
int vAABB::Intersects(vAABB *test_aabb) {}

void vAABBTree::SwapEndian() {
    bPlatEndianSwap(&NumLeafNodes);
    bPlatEndianSwap(&NumParentNodes);
    bPlatEndianSwap(&TotalNodes);
    bPlatEndianSwap(&Depth);
    for (int i = 0; i < TotalNodes; i++) {
        vAABB *aabb = &NodeArray[i];
        bPlatEndianSwap(&aabb->PositionX);
        bPlatEndianSwap(&aabb->PositionY);
        bPlatEndianSwap(&aabb->PositionZ);
        bPlatEndianSwap(&aabb->ParentIndex);
        bPlatEndianSwap(&aabb->NumChildren);
        bPlatEndianSwap(&aabb->ExtentX);
        bPlatEndianSwap(&aabb->ExtentY);
        bPlatEndianSwap(&aabb->ExtentZ);
        for (int j = 0; j < 10; j++) {
            bPlatEndianSwap(&aabb->ChildrenIndicies[j]);
        }
    }
}

vAABB *vAABBTree::QueryLeafHelper(vAABB *aabb, float x, float y, float z) {
    int num_children = aabb->NumChildren;
    vAABB *root = NodeArray;
    for (int i = 0; i < num_children; i++) {
        vAABB *child = &root[aabb->ChildrenIndicies[i]];
        if (child->Contains(x, y, z)) {
            if (child->NumChildren <= 0)
                return child;
            child = this->QueryLeafHelper(child, x, y, z);
            if (child != nullptr)
                return child;
        }
    }
    return nullptr;
}

vAABB *vAABBTree::QueryLeaf(float x, float y, float z) {
    vAABB *root = NodeArray;
    if (root == nullptr)
        return nullptr;
    if (!root->Contains(x, y, z))
        return nullptr;
    if (root->NumChildren <= 0)
        return root;

    return this->QueryLeafHelper(root, x, y, z);
}
