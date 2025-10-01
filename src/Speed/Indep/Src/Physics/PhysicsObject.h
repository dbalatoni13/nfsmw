#ifndef PHYSICS_PHYSICSOBJECT_H
#define PHYSICS_PHYSICSOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

struct _type_ID_POMechanics {
    const char *name() {
        return "ID_POMechanics";
    };
};

struct _type_ID_POBehaviors {
    const char *name() {
        return "ID_POBehaviors";
    };
};

class PhysicsObject : public Sim::Object,
                      public ISimable,
                      public IBody,
                      public IAttachable,
                      public UTL::Collections::GarbageNode<PhysicsObject, 160>,
                      protected UTL::Collections::Container<Behavior, _type_UContainer> {
  public:
    struct Behaviors : protected UTL::Std::list<Behavior *, _type_ID_POBehaviors> {
        // total size: 0x8
    };

    PhysicsObject(const Attrib::Instance &attribs, SimableType objType, unsigned int wuid, unsigned int num_interfaces);
    PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType, HSIMABLE owner, unsigned int wuid);

    // Overrides
    virtual ~PhysicsObject();

  private:
    // total size: 0xAC
    WWorldPos *mWPos;             // offset 0x58, size 0x4
    SimableType mObjType;         // offset 0x5C, size 0x4
    HSIMABLE mOwner;              // offset 0x60, size 0x4
    Attrib::Instance mAttributes; // offset 0x64, size 0x14
    IRigidBody *mRigidBody;       // offset 0x78, size 0x4
    HSIMTASK mSimulateTask;       // offset 0x7C, size 0x4
    // TODO correct one?
    Sim::IEntity *mEntity;                                                    // offset 0x80, size 0x4
    IPlayer *mPlayer;                                                         // offset 0x84, size 0x4
    HSIMSERVICE mBodyService;                                                 // offset 0x88, size 0x4
    unsigned int mWorldID;                                                    // offset 0x8C, size 0x4
    UTL::Std::map<unsigned int, Behavior *, _type_ID_POMechanics> mMechanics; // offset 0x90, size 0x10
    Behaviors mBehaviors;                                                     // offset 0xA0, size 0x8
    Attachments *mAttachments;                                                // offset 0xA8, size 0x4
};

#endif
