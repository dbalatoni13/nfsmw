#ifndef WORLD_SPACENODE_H
#define WORLD_SPACENODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <types.h>


class SpaceNode : public bTNode<SpaceNode> {
  public:
    void SetDirty();

    void SetLocalMatrix(bMatrix4 *matrix) {
        this->LocalMatrix = *matrix;
        this->SetDirty();
    }

    // Members
    bTList<SpaceNode> ChildrenList;   // offset 0x8, size 0x8
    bMatrix4 WorldMatrix;             // offset 0x10, size 0x40
    bMatrix4 LocalMatrix;             // offset 0x50, size 0x40
    bVector3 WorldVelocity;           // offset 0x90, size 0x10
    bVector3 LocalVelocity;           // offset 0xA0, size 0x10
    bVector3 WorldAngularVelocity;    // offset 0xB0, size 0x10
    bVector3 LocalAngularVelocity;    // offset 0xC0, size 0x10
    unsigned int NameHash;            // offset 0xD0, size 0x4
    const char *Name;                 // offset 0xD4, size 0x4
    int ReferenceCount;               // offset 0xD8, size 0x4
    unsigned int LastUpdateTimeStamp; // offset 0xDC, size 0x4
    SpaceNode *Parent;                // offset 0xE0, size 0x4
    unsigned int Dirty;               // offset 0xE4, size 0x4
    bMatrix4 *BlendingMatrices;       // offset 0xE8, size 0x4
};

void InitSpaceNodes();
void CloseSpaceNodes();

#endif
