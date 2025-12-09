#include "../SimEntity.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"

namespace Sim {

// UNSOLVED seems to be the same problem as in Sim::Activity
Entity::Entity()
    : Object(4),         //
      IEntity(this),     //
      IAttachable(this), //
      mSimable(nullptr), //
      mAttachments(new Attachments(this)) {
    IEntity::AddToList(ENTITY_ALL);
}

Entity::~Entity() {
    DetachPhysics();
    if (mAttachments) {
        delete mAttachments;
    }
}

void Entity::Kill() {
    ReleaseGC();
}

void Entity::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(mSimable, pOther)) {
        mSimable = nullptr;
    }
}

bool Entity::SetPosition(const UMath::Vector3 &position) const {
    if (mSimable) {
        IRigidBody *irb = mSimable->GetRigidBody();
        if (irb) {
            irb->SetPosition(position);
            return true;
        }
    }
    return false;
}

bool Entity::Detach(IUnknown *object) {
    if (UTL::COM::ComparePtr(mSimable, object)) {
        mSimable = nullptr;
    }
    return mAttachments->Detach(object);
}

const UMath::Vector3 &Entity::GetPosition() const {
    if (mSimable) {
        return mSimable->GetPosition();
    } else {
        return UMath::Vector3::kZero;
    }
}

// UNSOLVED
void Entity::AttachPhysics(ISimable *object) {
    if (!object) {
        DetachPhysics();
    } else {
        Attach(mSimable);
    }
}

void Entity::DetachPhysics() {
    if (mSimable) {
        Detach(mSimable);
        mSimable = nullptr;
    }
}

}; // namespace Sim
