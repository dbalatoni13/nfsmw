#ifndef _attrib_gen_collisionreactions_h
#define _attrib_gen_collisionreactions_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct collisionreactions : Instance {
    struct _LayoutStruct {
        CollisionReactionRecord REARSIDE_REACTION;  // offset 0x0, size 0x18
        CollisionReactionRecord FRONTSIDE_REACTION; // offset 0x18, size 0x18
        CollisionReactionRecord FRONT_REACTION;     // offset 0x30, size 0x18
        CollisionReactionRecord REAR_REACTION;      // offset 0x48, size 0x18
    };

    typedef CollisionReactionRecord TypeOf_FRONTSIDE_REACTION;
    typedef CollisionReactionRecord TypeOf_FRONT_REACTION;
    typedef CollisionReactionRecord TypeOf_REARSIDE_REACTION;
    typedef CollisionReactionRecord TypeOf_REAR_REACTION;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("collisionreactions");
    collisionreactions(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    collisionreactions(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    collisionreactions(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    collisionreactions(const collisionreactions &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    collisionreactions(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~collisionreactions() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xb32682f1;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xb32682f1, dynamicCollectionKey, spaceForAdditionalAttributes);
    }
    Key GenerateUniqueKey(const char *name, bool registerName) const {
        return GenerateUniqueKey(name, registerName);
    }
    void Change(const Collection *c) {
        Instance::Change(c);
    }
    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }
    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }
    void ChangeWithDefault(const RefSpec &refspec) {
        Instance::ChangeWithDefault(refspec);
    }
    void ChangeWithDefault(Key collectionkey) {
        Change(FindCollectionWithDefault(ClassKey(), collectionkey));
    }
    const collisionreactions &operator=(const collisionreactions &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const collisionreactions &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool FRONTSIDE_REACTION(TAttrib<CollisionReactionRecord> &result) const;
    bool FRONTSIDE_REACTION(CollisionReactionRecord &result) const;
    const CollisionReactionRecord &FRONTSIDE_REACTION() const;
    bool SET_FRONTSIDE_REACTION(const CollisionReactionRecord &input);

    bool FRONT_REACTION(TAttrib<CollisionReactionRecord> &result) const;
    bool FRONT_REACTION(CollisionReactionRecord &result) const;
    const CollisionReactionRecord &FRONT_REACTION() const;
    bool SET_FRONT_REACTION(const CollisionReactionRecord &input);

    bool REARSIDE_REACTION(TAttrib<CollisionReactionRecord> &result) const;
    bool REARSIDE_REACTION(CollisionReactionRecord &result) const;
    const CollisionReactionRecord &REARSIDE_REACTION() const;
    bool SET_REARSIDE_REACTION(const CollisionReactionRecord &input);

    bool REAR_REACTION(TAttrib<CollisionReactionRecord> &result) const;
    bool REAR_REACTION(CollisionReactionRecord &result) const;
    const CollisionReactionRecord &REAR_REACTION() const;
    bool SET_REAR_REACTION(const CollisionReactionRecord &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    collisionreactions &ConvertFromInstance(Instance &src) {}
    const collisionreactions &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key collisionreactions = 0xb32682f1;

}; // namespace ClassName

namespace Hash {
namespace collisionreactions {

static const Key FRONTSIDE_REACTION = 0x9a403ade;
static const Key FRONT_REACTION = 0x9186cdb9;
static const Key REARSIDE_REACTION = 0x5a6693d7;
static const Key REAR_REACTION = 0x035705d5;

}; // namespace collisionreactions
}; // namespace Hash

inline Key Gen::collisionreactions::ClassKey() {
    return ClassName::collisionreactions;
}

inline bool Gen::collisionreactions::FRONTSIDE_REACTION(TAttrib<CollisionReactionRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CollisionReactionRecord, 0x9a403ade);
}
inline const CollisionReactionRecord &Gen::collisionreactions::FRONTSIDE_REACTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(FRONTSIDE_REACTION);
}
inline bool Gen::collisionreactions::FRONTSIDE_REACTION(CollisionReactionRecord &result) const {
    result = FRONTSIDE_REACTION();
    return true;
}
inline bool Gen::collisionreactions::SET_FRONTSIDE_REACTION(const CollisionReactionRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FRONTSIDE_REACTION, input);
}

inline bool Gen::collisionreactions::FRONT_REACTION(TAttrib<CollisionReactionRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CollisionReactionRecord, 0x9186cdb9);
}
inline const CollisionReactionRecord &Gen::collisionreactions::FRONT_REACTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(FRONT_REACTION);
}
inline bool Gen::collisionreactions::FRONT_REACTION(CollisionReactionRecord &result) const {
    result = FRONT_REACTION();
    return true;
}
inline bool Gen::collisionreactions::SET_FRONT_REACTION(const CollisionReactionRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FRONT_REACTION, input);
}

inline bool Gen::collisionreactions::REARSIDE_REACTION(TAttrib<CollisionReactionRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CollisionReactionRecord, 0x5a6693d7);
}
inline const CollisionReactionRecord &Gen::collisionreactions::REARSIDE_REACTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(REARSIDE_REACTION);
}
inline bool Gen::collisionreactions::REARSIDE_REACTION(CollisionReactionRecord &result) const {
    result = REARSIDE_REACTION();
    return true;
}
inline bool Gen::collisionreactions::SET_REARSIDE_REACTION(const CollisionReactionRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(REARSIDE_REACTION, input);
}

inline bool Gen::collisionreactions::REAR_REACTION(TAttrib<CollisionReactionRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CollisionReactionRecord, 0x035705d5);
}
inline const CollisionReactionRecord &Gen::collisionreactions::REAR_REACTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(REAR_REACTION);
}
inline bool Gen::collisionreactions::REAR_REACTION(CollisionReactionRecord &result) const {
    result = REAR_REACTION();
    return true;
}
inline bool Gen::collisionreactions::SET_REAR_REACTION(const CollisionReactionRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(REAR_REACTION, input);
}

}; // namespace Attrib

#endif
