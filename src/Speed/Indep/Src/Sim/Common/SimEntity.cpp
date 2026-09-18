#include "../SimEntity.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"

template <>
UTL::Collections::GarbageNode<Sim::Entity, 8>::Collector UTL::Collections::GarbageNode<Sim::Entity, 8>::_mCollector =
    UTL::Collections::GarbageNode<Sim::Entity, 8>::Collector();

namespace Sim {

Entity::Entity()
    : Object(4),         //
#ifdef EA_PLATFORM_GAMECUBE
      // Expresion-sentencia de GNU: da el reparto de registros de GameCube
      // (con `this` o con un static_cast el .text cambia). MSVC no la acepta.
      IEntity(({ UTL::COM::Object *owner = this; owner; })), //
#else
      IEntity(this), //
#endif
      IAttachable(this), //
      mSimable(nullptr), //
      mAttachments(new Attachments(this)) {
    IEntity::AddToList(ENTITY_ALL);
}

Entity::~Entity() {
    DetachPhysics();
    delete mAttachments;
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

bool Entity::Attach(IUnknown *object) {
    if (UTL::COM::ComparePtr(mSimable, object)) {
        return false;
    }
    ISimable *simable;
    if (object->QueryInterface(&simable)) {

        if (mSimable) {

            Detach(mSimable);
            mSimable = nullptr;
        }
        mSimable = simable;
    }

    return mAttachments->Attach(object);
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

void Entity::AttachPhysics(ISimable *object) {
    if (!object) {
        DetachPhysics();
    } else {
        Attach(object);
    }
}

void Entity::DetachPhysics() {
    if (mSimable) {
        Detach(mSimable);
        mSimable = nullptr;
    }
}

}; // namespace Sim

template <>
UTL::COM::Factory<Sim::Param, Sim::IEntity, UCrc32>::Prototype *UTL::COM::Factory<Sim::Param, Sim::IEntity, UCrc32>::Prototype::mHead = NULL;
