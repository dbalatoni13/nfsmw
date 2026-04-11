#ifndef WORLD_SPACENODE_H
#define WORLD_SPACENODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <types.h>

class SpaceNode : public bTNode<SpaceNode> {
  public:
    // void *operator new(unsigned int size) {}

    // void operator delete(void *ptr) {}

    void SetNameHash(unsigned int name_hash) {
        NameHash = name_hash;
    }

    const char *GetName() {
        return Name;
    }

    unsigned int GetNameHash() {
        return NameHash;
    }

    SpaceNode *GetParent() {
        return Parent;
    }

    bMatrix4 *GetWorldMatrix() {
        if (Dirty) {
            Update();
        }
        return &WorldMatrix;
    }

    bVector3 *GetWorldVelocity() {
        if (Dirty) {
            Update();
        }
        return &WorldVelocity;
    }

    bVector3 *GetWorldAngularVelocity() {
        if (Dirty) {
            Update();
        }
        return &WorldAngularVelocity;
    }

    bMatrix4 *GetLocalMatrix() {
        return &LocalMatrix;
    }

    bVector3 *GetLocalVelocity() {
        return &LocalVelocity;
    }

    bVector3 *GetLocalAngularVelocity() {
        return &LocalAngularVelocity;
    }

    bMatrix4 *GetBlendingMatrices() {
        return BlendingMatrices;
    }

    void SetLocalMatrix(bMatrix4 *matrix) {
        LocalMatrix = *matrix;
        SetDirty();
    }

    void SetLocalVelocity(bVector3 *velocity) {
        LocalVelocity = *velocity;
        SetDirty();
    }

    void SetBlendingMatrices(bMatrix4 *matrix) {
        BlendingMatrices = matrix;
    }

    void SetDirty() {
        if (!Dirty) {
            ReallySetDirty();
        }
    }

    SpaceNode(SpaceNode *parent);

    virtual ~SpaceNode();

    void DoSnapshot(ReplaySnapshot *snapshot);

    void SetName(const char *name);

    void SetParent(SpaceNode *new_parent);

    void RemoveFromParent();

    void AddChild(SpaceNode *child);

    void RemoveChild(SpaceNode *child);

    void RemoveAllChildren();

    void Lock();

    void Unlock();

    void ReallySetDirty();

    void Update();

    void SetWorldMatrix(bMatrix4 *matrix);

    void SetWorldVelocity(bVector3 *velocity);

  private:
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
SpaceNode *CreateSpaceNode(SpaceNode *parent);
void DeleteSpaceNode(SpaceNode *space_node);

#endif
