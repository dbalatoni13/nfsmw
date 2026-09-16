//
//
//
//
//
//
//
//
//
//
//
//
#ifndef __INC_VOLUMETREEHPP
#define __INC_VOLUMETREEHPP

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x20
// Decl: 23
class vSphere {
  public:
    // Decl: 26
    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&this->PositionX);
    }
    // Decl: 27
    float GetRadius() {
        return this->Radius;
    }

    float PositionX; // offset 0x0, size 0x4, Decl: 31
    float PositionY; // offset 0x4, size 0x4, Decl: 32
    float PositionZ; // offset 0x8, size 0x4, Decl: 33
    float Radius;    // offset 0xC, size 0x4, Decl: 34

    uint16 ParentIndex;         // offset 0x10, size 0x2, Decl: 36
    uint16 ChildrenIndicies[7]; // offset 0x12, size 0xE, Decl: 37
};

// total size: 0x30
// Decl: 47
class vAABB {
  public:
    void Empty(); // Decl: 56

    void Create(bVector3 *position, bVector3 *extent); // Decl: 58
    void Create(vSphere *vsphere);                     // Decl: 59

    float GetVolume(); // Decl: 62

    int Contains(float x, float y, float z); // Decl: 64
    int Encloses(vAABB *test_aabb);          // Decl: 65
    int Intersects(vAABB *test_aabb);        // Decl: 66

    float PositionX; // offset 0x0, size 0x4, Decl: 71
    float PositionY; // offset 0x4, size 0x4, Decl: 72
    float PositionZ; // offset 0x8, size 0x4, Decl: 73

    int16 ParentIndex; // offset 0xC, size 0x2, Decl: 75
    int16 NumChildren; // offset 0xE, size 0x2, Decl: 76

    float ExtentX; // offset 0x10, size 0x4, Decl: 78
    float ExtentY; // offset 0x14, size 0x4, Decl: 79
    float ExtentZ; // offset 0x18, size 0x4, Decl: 80

    int16 ChildrenIndicies[10]; // offset 0x1C, size 0x14, Decl: 82
};

// total size: 0x10
// Decl: 92
class vAABBTree {
  public:
    void SwapEndian();                           // Decl: 95
    vAABB *QueryLeaf(float x, float y, float z); // Decl: 100

  private:
    vAABB *QueryLeafHelper(vAABB *aabb, float x, float y, float z); // Decl: 104

  public:
    vAABB *NodeArray; // offset 0x0, size 0x4, Decl: 108

    int16 NumLeafNodes;   // offset 0x4, size 0x2, Decl: 110
    int16 NumParentNodes; // offset 0x6, size 0x2, Decl: 111
    int16 TotalNodes;     // offset 0x8, size 0x2, Decl: 112
    int16 Depth;          // offset 0xA, size 0x2, Decl: 113

    int32 pad1; // offset 0xC, size 0x4, Decl: 115
};

#endif
