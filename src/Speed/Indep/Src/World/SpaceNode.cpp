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
    bVector3 rotated_velocity;

    if (Parent) {
        bMulMatrix(&WorldMatrix, Parent->GetWorldMatrix(), &LocalMatrix);
        bMulMatrix(&rotated_velocity, Parent->GetWorldMatrix(), &LocalVelocity);
        rotated_velocity -= *reinterpret_cast<bVector3 *>(&Parent->GetWorldMatrix()->v3);
        WorldVelocity = rotated_velocity + *Parent->GetWorldVelocity();
    } else {
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&LocalMatrix), *reinterpret_cast<Mtx44 *>(&WorldMatrix));
        WorldVelocity = LocalVelocity;
    }

    Dirty = 0;
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
        SpaceNode *head = SpaceNodeTrashList.RemoveHead();

        delete head;
    }
}

void InitSpaceNodes() {
    SpaceNodeSlotPool = bNewSlotPool(0xF4, 0x50, "SpaceNodeSlotPool", GetVirtualMemoryAllocParams());
}
