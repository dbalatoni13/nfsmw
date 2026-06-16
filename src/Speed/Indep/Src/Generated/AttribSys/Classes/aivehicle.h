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
    bool AccelerationMultiplier(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xcc320329);
    }
    bool AccelerationMultiplier(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xcc320329, result);
    }
    const EA::Reflection::Float &AccelerationMultiplier() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xcc320329);
    }
    bool SET_AccelerationMultiplier(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xcc320329, input);
    }
    bool CostToStateForDestroying(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xce1cedfe);
    }
    bool CostToStateForDestroying(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xce1cedfe, result);
    }
    const EA::Reflection::Int32 &CostToStateForDestroying() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xce1cedfe);
    }
    bool SET_CostToStateForDestroying(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xce1cedfe, input);
    }
    bool DetachmentID(TAttrib<Csis::Type_subject_battalion> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Csis::Type_subject_battalion, 0x899decb6);
    }
    bool DetachmentID(Csis::Type_subject_battalion &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DetachmentID, result);
    }
    const Csis::Type_subject_battalion &DetachmentID() const {
        ATTRIB_CODEGEN_GETLAYOUT(DetachmentID);
    }
    bool SET_DetachmentID(const Csis::Type_subject_battalion &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DetachmentID, input);
    }
    bool MAXIMUM_AI_SPEED(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7ca3d0a5);
    }
    bool MAXIMUM_AI_SPEED(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MAXIMUM_AI_SPEED, result);
    }
    const EA::Reflection::Float &MAXIMUM_AI_SPEED() const {
        ATTRIB_CODEGEN_GETLAYOUT(MAXIMUM_AI_SPEED);
    }
    bool SET_MAXIMUM_AI_SPEED(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MAXIMUM_AI_SPEED, input);
    }
    bool PlayerCollisions(TAttrib<AICollisionReactionRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(AICollisionReactionRecord, 0x489212da);
    }
    bool PlayerCollisions(AICollisionReactionRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, result, index);
    }
    const AICollisionReactionRecord &PlayerCollisions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, index);
    }
    unsigned int Num_PlayerCollisions() const {
        ATTRIB_CODEGEN_GETLENGTH(0x489212da);
    }
    bool SET_PlayerCollisions(const AICollisionReactionRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(AICollisionReactionRecord, 0x489212da, input, index);
    }
    bool PlayerCollisionsDefault(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x75a2bcd7);
    }
    bool PlayerCollisionsDefault(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x75a2bcd7, result);
    }
    const RefSpec &PlayerCollisionsDefault() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x75a2bcd7);
    }
    bool SET_PlayerCollisionsDefault(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x75a2bcd7, input);
    }
    bool RepPointsForDestroying(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc07c8040);
    }
    bool RepPointsForDestroying(EA::Reflection::Int32 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, result, index);
    }
    const EA::Reflection::Int32 &RepPointsForDestroying(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, index);
    }
    unsigned int Num_RepPointsForDestroying() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc07c8040);
    }
    bool SET_RepPointsForDestroying(const EA::Reflection::Int32 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Int32, 0xc07c8040, input, index);
    }
    bool TETHER_WEIGHT(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb8211faf);
    }
    bool TETHER_WEIGHT(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TETHER_WEIGHT, result);
    }
    const EA::Reflection::Float &TETHER_WEIGHT() const {
        ATTRIB_CODEGEN_GETLAYOUT(TETHER_WEIGHT);
    }
    bool SET_TETHER_WEIGHT(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TETHER_WEIGHT, input);
    }
    bool TopSpeedMultiplier(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xec57e16b);
    }
    bool TopSpeedMultiplier(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xec57e16b, result);
    }
    const EA::Reflection::Float &TopSpeedMultiplier() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xec57e16b);
    }
    bool SET_TopSpeedMultiplier(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xec57e16b, input);
    }

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

}; // namespace Attrib

#endif
