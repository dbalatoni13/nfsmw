#include "SpaceNode.hpp"

#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *SpaceNodeSlotPool;

SpaceNode *SpaceNode_Construct(SpaceNode *space_node, SpaceNode *parent) asm("__9SpaceNodeP9SpaceNode");

bTList<SpaceNode> SpaceNodeTrashList;

SpaceNode::SpaceNode(SpaceNode *parent) {
    bVector3 zero;

    this->ReferenceCount = 0;
    this->Dirty = 1;

    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&this->LocalMatrix));

    this->Parent = 0;
    bFill(&zero, 0.0f, 0.0f, 0.0f);
    bCopy(&this->LocalVelocity, &zero);
    bFill(&zero, 0.0f, 0.0f, 0.0f);
    bCopy(&this->WorldAngularVelocity, &zero);
    bCopy(&this->LocalAngularVelocity, &zero);

    this->SetParent(parent);
    this->BlendingMatrices = 0;
}

SpaceNode::~SpaceNode() {
    this->RemoveAllChildren();
    this->RemoveFromParent();
}

void SpaceNode::SetParent(SpaceNode *new_parent) {
    if (new_parent != this->Parent) {
        this->RemoveFromParent();
        if (new_parent != 0) {
            new_parent->AddChild(this);
        }
    }
}

void SpaceNode::RemoveFromParent() {
    if (this->Parent != 0) {
        this->Parent->RemoveChild(this);
    }
}

void SpaceNode::AddChild(SpaceNode *child) {
    child = this->ChildrenList.AddTail(child);
    child->Parent = this;
    this->Lock();
}

void SpaceNode::RemoveChild(SpaceNode *child) {
    child = child->Remove();
    child->Parent = 0;
    this->Unlock();
}

void SpaceNode::RemoveAllChildren() {
    while (this->ChildrenList.GetHead() != this->ChildrenList.EndOfList()) {
        this->RemoveChild(this->ChildrenList.GetHead());
    }
}

void SpaceNode::Lock() {
    this->ReferenceCount++;
}

void SpaceNode::Unlock() {
    this->ReferenceCount--;
    if (this->ReferenceCount == 0) {
        this->RemoveFromParent();
        SpaceNodeTrashList.AddTail(this);
    }
}

void SpaceNode::ReallySetDirty() {
    this->Dirty = 1;

    for (SpaceNode *child = this->ChildrenList.GetHead(); child != this->ChildrenList.EndOfList(); child = child->GetNext()) {
        if (!child->Dirty) {
            child->ReallySetDirty();
        }
    }
}

void SpaceNode::Update() {
    bVector3 world_velocity;

    if (this->Parent == 0) {
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&this->LocalMatrix), *reinterpret_cast<Mtx44 *>(&this->WorldMatrix));
        world_velocity = this->LocalVelocity;
    } else {
        if (this->Parent->Dirty != 0) {
            this->Parent->Update();
        }

        bMulMatrix(&this->WorldMatrix, &this->Parent->WorldMatrix, &this->LocalMatrix);

        if (this->Parent->Dirty != 0) {
            this->Parent->Update();
        }

        bMulMatrix(&world_velocity, &this->Parent->WorldMatrix, &this->LocalVelocity);

        if (this->Parent->Dirty != 0) {
            this->Parent->Update();
        }

        world_velocity.x -= this->Parent->WorldMatrix.v3.x;
        world_velocity.y -= this->Parent->WorldMatrix.v3.y;
        world_velocity.z -= this->Parent->WorldMatrix.v3.z;

        if (this->Parent->Dirty != 0) {
            this->Parent->Update();
        }

        world_velocity.x += this->Parent->WorldVelocity.x;
        world_velocity.y += this->Parent->WorldVelocity.y;
        world_velocity.z += this->Parent->WorldVelocity.z;
    }

    this->WorldVelocity = world_velocity;
    this->Dirty = 0;
}

SpaceNode *CreateSpaceNode(SpaceNode *parent) {
    SpaceNode *space_node = SpaceNode_Construct(static_cast<SpaceNode *>(bOMalloc(SpaceNodeSlotPool)), parent);

    space_node->Lock();
    return space_node;
}

void DeleteSpaceNode(SpaceNode *space_node) {
    space_node->Unlock();
}

void ServiceSpaceNodes() {
    while (!SpaceNodeTrashList.IsEmpty()) {
        delete SpaceNodeTrashList.RemoveHead();
    }
}

void InitSpaceNodes() {
    SpaceNodeSlotPool = bNewSlotPool(0xF4, 0x50, "SpaceNodeSlotPool", GetVirtualMemoryAllocParams());
}
