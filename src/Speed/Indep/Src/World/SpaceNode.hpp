#ifndef SPACENODE_HPP
#define SPACENODE_HPP

#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <types.h>

extern SlotPool *SpaceNodeSlotPool;

// total size: 0xF4
class SpaceNode : public bTNode<SpaceNode> {
  public:
    USE_SLOTALLOC(SpaceNodeSlotPool);

    void SetNameHash(uint32 name_hash) {
        this->NameHash = name_hash;
    }

    const char *GetName() {
        return this->Name;
    }

    unsigned int GetNameHash() {
        return this->NameHash;
    }

    SpaceNode *GetParent() {
        return this->Parent;
    }

    bMatrix4 *GetWorldMatrix() {
        if (this->Dirty) {
            this->Update();
        }
        return &this->WorldMatrix;
    }

    bVector3 *GetWorldVelocity() {
        if (this->Dirty) {
            this->Update();
        }
        return &this->WorldVelocity;
    }

    bVector3 *GetWorldAngularVelocity() {
        if (this->Dirty) {
            this->Update();
        }
        return &this->WorldAngularVelocity;
    }

    bMatrix4 *GetLocalMatrix() {
        return &this->LocalMatrix;
    }

    bVector3 *GetLocalVelocity() {
        return &this->LocalVelocity;
    }

    bVector3 *GetLocalAngularVelocity() {
        return &this->LocalAngularVelocity;
    }

    bMatrix4 *GetBlendingMatrices() {
        return this->BlendingMatrices;
    }

    void SetLocalMatrix(bMatrix4 *matrix) {
        this->LocalMatrix = *matrix;
        this->SetDirty();
    }

    void SetLocalVelocity(bVector3 *velocity) {
        this->LocalVelocity = *velocity;
        this->SetDirty();
    }

    void SetBlendingMatrices(bMatrix4 *matrix) {
        this->BlendingMatrices = matrix;
    }

    void SetDirty() {
        if (!this->Dirty) {
            this->ReallySetDirty();
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
    bTList<SpaceNode> ChildrenList; // offset 0x8, size 0x8
    bMatrix4 WorldMatrix;           // offset 0x10, size 0x40
    bMatrix4 LocalMatrix;           // offset 0x50, size 0x40
    bVector3 WorldVelocity;         // offset 0x90, size 0x10
    bVector3 LocalVelocity;         // offset 0xA0, size 0x10
    bVector3 WorldAngularVelocity;  // offset 0xB0, size 0x10
    bVector3 LocalAngularVelocity;  // offset 0xC0, size 0x10
    uint32 NameHash;                // offset 0xD0, size 0x4
    const char *Name;               // offset 0xD4, size 0x4
    int32 ReferenceCount;           // offset 0xD8, size 0x4
    uint32 LastUpdateTimeStamp;     // offset 0xDC, size 0x4
    SpaceNode *Parent;              // offset 0xE0, size 0x4
    uint32 Dirty;                   // offset 0xE4, size 0x4
    bMatrix4 *BlendingMatrices;     // offset 0xE8, size 0x4
    uint32 pad1;                    // offset 0xEC, size 0x4
};

void ServiceSpaceNodes();
void InitSpaceNodes();
void CloseSpaceNodes();
SpaceNode *CreateSpaceNode(SpaceNode *parent);
void DeleteSpaceNode(SpaceNode *space_node);

#endif
