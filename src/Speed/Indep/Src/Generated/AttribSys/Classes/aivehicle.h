#ifndef _attrib_gen_aivehicle_h
#define _attrib_gen_aivehicle_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct aivehicle : Instance {
    struct _LayoutStruct {
        EA::Reflection::Float TETHER_WEIGHT;       // offset 0x0, size 0x4
        Csis::Type_subject_battalion DetachmentID; // offset 0x4, size 0x4
        EA::Reflection::Float MAXIMUM_AI_SPEED;    // offset 0x8, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_AccelerationMultiplier;
    typedef EA::Reflection::Int32 TypeOf_CostToStateForDestroying;
    typedef Csis::Type_subject_battalion TypeOf_DetachmentID;
    typedef EA::Reflection::Float TypeOf_MAXIMUM_AI_SPEED;
    typedef AICollisionReactionRecord TypeOf_PlayerCollisions;
    typedef RefSpec TypeOf_PlayerCollisionsDefault;
    typedef EA::Reflection::Int32 TypeOf_RepPointsForDestroying;
    typedef EA::Reflection::Float TypeOf_TETHER_WEIGHT;
    typedef EA::Reflection::Float TypeOf_TopSpeedMultiplier;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("aivehicle");
    aivehicle(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aivehicle(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aivehicle(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aivehicle(const aivehicle &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aivehicle(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~aivehicle() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x22515733;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x22515733, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const aivehicle &operator=(const aivehicle &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const aivehicle &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AccelerationMultiplier(TAttrib<EA::Reflection::Float> &result) const;
    bool AccelerationMultiplier(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AccelerationMultiplier() const;
    bool SET_AccelerationMultiplier(const EA::Reflection::Float &input);

    bool CostToStateForDestroying(TAttrib<EA::Reflection::Int32> &result) const;
    bool CostToStateForDestroying(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &CostToStateForDestroying() const;
    bool SET_CostToStateForDestroying(const EA::Reflection::Int32 &input);

    bool DetachmentID(TAttrib<Csis::Type_subject_battalion> &result) const;
    bool DetachmentID(Csis::Type_subject_battalion &result) const;
    const Csis::Type_subject_battalion &DetachmentID() const;
    bool SET_DetachmentID(const Csis::Type_subject_battalion &input);

    bool MAXIMUM_AI_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool MAXIMUM_AI_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MAXIMUM_AI_SPEED() const;
    bool SET_MAXIMUM_AI_SPEED(const EA::Reflection::Float &input);

    bool PlayerCollisions(TAttrib<AICollisionReactionRecord> &result) const;
    bool PlayerCollisions(AICollisionReactionRecord &result, unsigned int index) const;
    const AICollisionReactionRecord &PlayerCollisions(unsigned int index) const;
    unsigned int Num_PlayerCollisions() const;
    bool SET_PlayerCollisions(const AICollisionReactionRecord &input, unsigned int index);

    bool PlayerCollisionsDefault(TAttrib<RefSpec> &result) const;
    bool PlayerCollisionsDefault(RefSpec &result) const;
    const RefSpec &PlayerCollisionsDefault() const;
    bool SET_PlayerCollisionsDefault(const RefSpec &input);

    bool RepPointsForDestroying(TAttrib<EA::Reflection::Int32> &result) const;
    bool RepPointsForDestroying(EA::Reflection::Int32 &result, unsigned int index) const;
    const EA::Reflection::Int32 &RepPointsForDestroying(unsigned int index) const;
    unsigned int Num_RepPointsForDestroying() const;
    bool SET_RepPointsForDestroying(const EA::Reflection::Int32 &input, unsigned int index);

    bool TETHER_WEIGHT(TAttrib<EA::Reflection::Float> &result) const;
    bool TETHER_WEIGHT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TETHER_WEIGHT() const;
    bool SET_TETHER_WEIGHT(const EA::Reflection::Float &input);

    bool TopSpeedMultiplier(TAttrib<EA::Reflection::Float> &result) const;
    bool TopSpeedMultiplier(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TopSpeedMultiplier() const;
    bool SET_TopSpeedMultiplier(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    aivehicle &ConvertFromInstance(Instance &src) {}
    const aivehicle &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key aivehicle = 0x22515733;

}; // namespace ClassName

namespace Hash {
namespace aivehicle {

static const Key AccelerationMultiplier = 0xcc320329;
static const Key CostToStateForDestroying = 0xce1cedfe;
static const Key DetachmentID = 0x899decb6;
static const Key MAXIMUM_AI_SPEED = 0x7ca3d0a5;
static const Key PlayerCollisions = 0x489212da;
static const Key PlayerCollisionsDefault = 0x75a2bcd7;
static const Key RepPointsForDestroying = 0xc07c8040;
static const Key TETHER_WEIGHT = 0xb8211faf;
static const Key TopSpeedMultiplier = 0xec57e16b;

}; // namespace aivehicle
}; // namespace Hash

inline Key Gen::aivehicle::ClassKey() {
    return ClassName::aivehicle;
}

inline bool Gen::aivehicle::AccelerationMultiplier(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xcc320329);
}
inline const EA::Reflection::Float &Gen::aivehicle::AccelerationMultiplier() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xcc320329);
}
inline bool Gen::aivehicle::AccelerationMultiplier(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xcc320329, result);
}
inline bool Gen::aivehicle::SET_AccelerationMultiplier(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xcc320329, input);
}

inline bool Gen::aivehicle::CostToStateForDestroying(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xce1cedfe);
}
inline const EA::Reflection::Int32 &Gen::aivehicle::CostToStateForDestroying() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xce1cedfe);
}
inline bool Gen::aivehicle::CostToStateForDestroying(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xce1cedfe, result);
}
inline bool Gen::aivehicle::SET_CostToStateForDestroying(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xce1cedfe, input);
}

inline bool Gen::aivehicle::DetachmentID(TAttrib<Csis::Type_subject_battalion> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Csis::Type_subject_battalion, 0x899decb6);
}
inline const Csis::Type_subject_battalion &Gen::aivehicle::DetachmentID() const {
    ATTRIB_CODEGEN_GETLAYOUT(DetachmentID);
}
inline bool Gen::aivehicle::DetachmentID(Csis::Type_subject_battalion &result) const {
    result = DetachmentID();
    return true;
}
inline bool Gen::aivehicle::SET_DetachmentID(const Csis::Type_subject_battalion &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DetachmentID, input);
}

inline bool Gen::aivehicle::MAXIMUM_AI_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7ca3d0a5);
}
inline const EA::Reflection::Float &Gen::aivehicle::MAXIMUM_AI_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(MAXIMUM_AI_SPEED);
}
inline bool Gen::aivehicle::MAXIMUM_AI_SPEED(EA::Reflection::Float &result) const {
    result = MAXIMUM_AI_SPEED();
    return true;
}
inline bool Gen::aivehicle::SET_MAXIMUM_AI_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MAXIMUM_AI_SPEED, input);
}

inline bool Gen::aivehicle::PlayerCollisions(TAttrib<AICollisionReactionRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AICollisionReactionRecord, 0x489212da);
}
inline const AICollisionReactionRecord &Gen::aivehicle::PlayerCollisions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, index);
}
inline bool Gen::aivehicle::PlayerCollisions(AICollisionReactionRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, result, index);
}
inline unsigned int Gen::aivehicle::Num_PlayerCollisions() const {
    ATTRIB_CODEGEN_GETLENGTH(0x489212da);
}
inline bool Gen::aivehicle::SET_PlayerCollisions(const AICollisionReactionRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, input, index);
}

inline bool Gen::aivehicle::PlayerCollisionsDefault(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x75a2bcd7);
}
inline const RefSpec &Gen::aivehicle::PlayerCollisionsDefault() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x75a2bcd7);
}
inline bool Gen::aivehicle::PlayerCollisionsDefault(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x75a2bcd7, result);
}
inline bool Gen::aivehicle::SET_PlayerCollisionsDefault(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x75a2bcd7, input);
}

inline bool Gen::aivehicle::RepPointsForDestroying(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc07c8040);
}
inline const EA::Reflection::Int32 &Gen::aivehicle::RepPointsForDestroying(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, index);
}
inline bool Gen::aivehicle::RepPointsForDestroying(EA::Reflection::Int32 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, result, index);
}
inline unsigned int Gen::aivehicle::Num_RepPointsForDestroying() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc07c8040);
}
inline bool Gen::aivehicle::SET_RepPointsForDestroying(const EA::Reflection::Int32 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, input, index);
}

inline bool Gen::aivehicle::TETHER_WEIGHT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb8211faf);
}
inline const EA::Reflection::Float &Gen::aivehicle::TETHER_WEIGHT() const {
    ATTRIB_CODEGEN_GETLAYOUT(TETHER_WEIGHT);
}
inline bool Gen::aivehicle::TETHER_WEIGHT(EA::Reflection::Float &result) const {
    result = TETHER_WEIGHT();
    return true;
}
inline bool Gen::aivehicle::SET_TETHER_WEIGHT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TETHER_WEIGHT, input);
}

inline bool Gen::aivehicle::TopSpeedMultiplier(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xec57e16b);
}
inline const EA::Reflection::Float &Gen::aivehicle::TopSpeedMultiplier() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xec57e16b);
}
inline bool Gen::aivehicle::TopSpeedMultiplier(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xec57e16b, result);
}
inline bool Gen::aivehicle::SET_TopSpeedMultiplier(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xec57e16b, input);
}

}; // namespace Attrib

#endif
