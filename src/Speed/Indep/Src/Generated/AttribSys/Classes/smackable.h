#ifndef _attrib_gen_smackable_h
#define _attrib_gen_smackable_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct smackable : Instance {
    struct _LayoutStruct {
        Attrib::StringKey EventSequencer;    // offset 0x0, size 0x10
        EA::Reflection::Text CollectionName; // offset 0x10, size 0x4
        EA::Reflection::Float MASS;          // offset 0x14, size 0x4
        EA::Reflection::Float DETACH_FORCE;  // offset 0x18, size 0x4
        EA::Reflection::Bool NO_CAR_EFFECT;  // offset 0x1c, size 0x1
        EA::Reflection::Bool IsWooshable;    // offset 0x1d, size 0x1
    };

    typedef EA::Reflection::Bool TypeOf_AI_AVOIDABLE;
    typedef EA::Reflection::Bool TypeOf_ALLOW_OFF_WORLD;
    typedef EA::Reflection::Float TypeOf_AUTO_SIMPLIFY;
    typedef Attrib::StringKey TypeOf_BEHAVIORS;
    typedef EA::Reflection::Bool TypeOf_CAMERA_AVOIDABLE;
    typedef EA::Reflection::Int32 TypeOf_CAN_SIMPLIFY;
    typedef EA::Reflection::UInt32 TypeOf_COST_TO_STATE;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Float TypeOf_DETACH_FORCE;
    typedef EA::Reflection::Float TypeOf_DROPOUT;
    typedef Attrib::StringKey TypeOf_EventSequencer;
    typedef EA::Reflection::Float TypeOf_ExplosionEffect;
    typedef EA::Reflection::Bool TypeOf_IsWooshable;
    typedef EA::Reflection::Float TypeOf_KILL_OFF_SCREEN;
    typedef EA::Reflection::Float TypeOf_MASS;
    typedef UMath::Vector3 TypeOf_MOMENT;
    typedef EA::Reflection::Bool TypeOf_NO_CAR_EFFECT;
    typedef EffectLinkageRecord TypeOf_OnDetached;
    typedef EffectLinkageRecord TypeOf_OnHitGround;
    typedef EffectLinkageRecord TypeOf_OnHitObject;
    typedef EffectLinkageRecord TypeOf_OnHitWorld;
    typedef EffectLinkageRecord TypeOf_OnScrapeGround;
    typedef EffectLinkageRecord TypeOf_OnScrapeObject;
    typedef EffectLinkageRecord TypeOf_OnScrapeWorld;
    typedef EA::Reflection::Float TypeOf_RESPAWN_TIME;
    typedef EA::Reflection::Bool TypeOf_SimplePhysics;
    typedef eDRIVE_BY_TYPE TypeOf_WooshType;
    typedef EA::Reflection::Bool TypeOf_no_trigger;
    typedef RefSpec TypeOf_rigidbodyspecs;
    typedef EA::Reflection::Bool TypeOf_start_sequencer;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("smackable");
    smackable(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    smackable(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    smackable(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    smackable(const smackable &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    smackable(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~smackable() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xce70d7db;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xce70d7db, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const smackable &operator=(const smackable &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const smackable &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AI_AVOIDABLE(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6e4de905);
    }
    bool AI_AVOIDABLE(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6e4de905, result);
    }
    const EA::Reflection::Bool &AI_AVOIDABLE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6e4de905);
    }
    bool SET_AI_AVOIDABLE(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6e4de905, input);
    }
    bool ALLOW_OFF_WORLD(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xbee139f1);
    }
    bool ALLOW_OFF_WORLD(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xbee139f1, result);
    }
    const EA::Reflection::Bool &ALLOW_OFF_WORLD() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xbee139f1);
    }
    bool SET_ALLOW_OFF_WORLD(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xbee139f1, input);
    }
    bool AUTO_SIMPLIFY(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb5c0dac8);
    }
    bool AUTO_SIMPLIFY(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb5c0dac8, result);
    }
    const EA::Reflection::Float &AUTO_SIMPLIFY() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb5c0dac8);
    }
    bool SET_AUTO_SIMPLIFY(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb5c0dac8, input);
    }
    bool BEHAVIORS(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xda5f19f9);
    }
    bool BEHAVIORS(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xda5f19f9, result, index);
    }
    const Attrib::StringKey &BEHAVIORS(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xda5f19f9, index);
    }
    unsigned int Num_BEHAVIORS() const {
        ATTRIB_CODEGEN_GETLENGTH(0xda5f19f9);
    }
    bool SET_BEHAVIORS(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xda5f19f9, input, index);
    }
    bool CAMERA_AVOIDABLE(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe9d83d0c);
    }
    bool CAMERA_AVOIDABLE(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe9d83d0c, result);
    }
    const EA::Reflection::Bool &CAMERA_AVOIDABLE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe9d83d0c);
    }
    bool SET_CAMERA_AVOIDABLE(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe9d83d0c, input);
    }
    bool CAN_SIMPLIFY(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd1d3909a);
    }
    bool CAN_SIMPLIFY(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd1d3909a, result);
    }
    const EA::Reflection::Int32 &CAN_SIMPLIFY() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd1d3909a);
    }
    bool SET_CAN_SIMPLIFY(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd1d3909a, input);
    }
    bool COST_TO_STATE(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x6db7d192);
    }
    bool COST_TO_STATE(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x6db7d192, result);
    }
    const EA::Reflection::UInt32 &COST_TO_STATE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x6db7d192);
    }
    bool SET_COST_TO_STATE(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x6db7d192, input);
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
    }
    bool CollectionName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CollectionName, result);
    }
    const EA::Reflection::Text &CollectionName() const {
        ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
    }
    bool SET_CollectionName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
    }
    bool DETACH_FORCE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbe71dbad);
    }
    bool DETACH_FORCE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DETACH_FORCE, result);
    }
    const EA::Reflection::Float &DETACH_FORCE() const {
        ATTRIB_CODEGEN_GETLAYOUT(DETACH_FORCE);
    }
    bool SET_DETACH_FORCE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DETACH_FORCE, input);
    }
    bool DROPOUT(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x44f1273b);
    }
    bool DROPOUT(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x44f1273b, result, index);
    }
    const EA::Reflection::Float &DROPOUT(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x44f1273b, index);
    }
    unsigned int Num_DROPOUT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x44f1273b);
    }
    bool SET_DROPOUT(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x44f1273b, input, index);
    }
    bool EventSequencer(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5aab860f);
    }
    bool EventSequencer(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EventSequencer, result);
    }
    const Attrib::StringKey &EventSequencer() const {
        ATTRIB_CODEGEN_GETLAYOUT(EventSequencer);
    }
    bool SET_EventSequencer(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EventSequencer, input);
    }
    bool ExplosionEffect(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x360552da);
    }
    bool ExplosionEffect(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x360552da, result);
    }
    const EA::Reflection::Float &ExplosionEffect() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x360552da);
    }
    bool SET_ExplosionEffect(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x360552da, input);
    }
    bool IsWooshable(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6f002423);
    }
    bool IsWooshable(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IsWooshable, result);
    }
    const EA::Reflection::Bool &IsWooshable() const {
        ATTRIB_CODEGEN_GETLAYOUT(IsWooshable);
    }
    bool SET_IsWooshable(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IsWooshable, input);
    }
    bool KILL_OFF_SCREEN(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc141f7f8);
    }
    bool KILL_OFF_SCREEN(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc141f7f8, result);
    }
    const EA::Reflection::Float &KILL_OFF_SCREEN() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc141f7f8);
    }
    bool SET_KILL_OFF_SCREEN(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc141f7f8, input);
    }
    bool MASS(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4a56503d);
    }
    bool MASS(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MASS, result);
    }
    const EA::Reflection::Float &MASS() const {
        ATTRIB_CODEGEN_GETLAYOUT(MASS);
    }
    bool SET_MASS(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MASS, input);
    }
    bool MOMENT(TAttrib<UMath::Vector3> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector3, 0xfb19212f);
    }
    bool MOMENT(UMath::Vector3 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector3, 0xfb19212f, result);
    }
    const UMath::Vector3 &MOMENT() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector3, 0xfb19212f);
    }
    bool SET_MOMENT(const UMath::Vector3 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector3, 0xfb19212f, input);
    }
    bool NO_CAR_EFFECT(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1f989f01);
    }
    bool NO_CAR_EFFECT(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NO_CAR_EFFECT, result);
    }
    const EA::Reflection::Bool &NO_CAR_EFFECT() const {
        ATTRIB_CODEGEN_GETLAYOUT(NO_CAR_EFFECT);
    }
    bool SET_NO_CAR_EFFECT(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NO_CAR_EFFECT, input);
    }
    bool OnDetached(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xd177bdaa);
    }
    bool OnDetached(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xd177bdaa, result, index);
    }
    const EffectLinkageRecord &OnDetached(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xd177bdaa, index);
    }
    unsigned int Num_OnDetached() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd177bdaa);
    }
    bool SET_OnDetached(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xd177bdaa, input, index);
    }
    bool OnHitGround(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xd9c6cdfd);
    }
    bool OnHitGround(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, result, index);
    }
    const EffectLinkageRecord &OnHitGround(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, index);
    }
    unsigned int Num_OnHitGround() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd9c6cdfd);
    }
    bool SET_OnHitGround(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, input, index);
    }
    bool OnHitObject(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0x18915735);
    }
    bool OnHitObject(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0x18915735, result, index);
    }
    const EffectLinkageRecord &OnHitObject(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0x18915735, index);
    }
    unsigned int Num_OnHitObject() const {
        ATTRIB_CODEGEN_GETLENGTH(0x18915735);
    }
    bool SET_OnHitObject(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0x18915735, input, index);
    }
    bool OnHitWorld(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xe3167336);
    }
    bool OnHitWorld(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, result, index);
    }
    const EffectLinkageRecord &OnHitWorld(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, index);
    }
    unsigned int Num_OnHitWorld() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe3167336);
    }
    bool SET_OnHitWorld(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, input, index);
    }
    bool OnScrapeGround(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xdb823931);
    }
    bool OnScrapeGround(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, result, index);
    }
    const EffectLinkageRecord &OnScrapeGround(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, index);
    }
    unsigned int Num_OnScrapeGround() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdb823931);
    }
    bool SET_OnScrapeGround(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, input, index);
    }
    bool OnScrapeObject(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xdacb1c11);
    }
    bool OnScrapeObject(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, result, index);
    }
    const EffectLinkageRecord &OnScrapeObject(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, index);
    }
    unsigned int Num_OnScrapeObject() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdacb1c11);
    }
    bool SET_OnScrapeObject(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, input, index);
    }
    bool OnScrapeWorld(TAttrib<EffectLinkageRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0x7100960c);
    }
    bool OnScrapeWorld(EffectLinkageRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, result, index);
    }
    const EffectLinkageRecord &OnScrapeWorld(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, index);
    }
    unsigned int Num_OnScrapeWorld() const {
        ATTRIB_CODEGEN_GETLENGTH(0x7100960c);
    }
    bool SET_OnScrapeWorld(const EffectLinkageRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, input, index);
    }
    bool RESPAWN_TIME(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5f84f834);
    }
    bool RESPAWN_TIME(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5f84f834, result);
    }
    const EA::Reflection::Float &RESPAWN_TIME() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5f84f834);
    }
    bool SET_RESPAWN_TIME(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5f84f834, input);
    }
    bool SimplePhysics(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xee0011e3);
    }
    bool SimplePhysics(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xee0011e3, result);
    }
    const EA::Reflection::Bool &SimplePhysics() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xee0011e3);
    }
    bool SET_SimplePhysics(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xee0011e3, input);
    }
    bool WooshType(TAttrib<eDRIVE_BY_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(eDRIVE_BY_TYPE, 0x7e744600);
    }
    bool WooshType(eDRIVE_BY_TYPE &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(eDRIVE_BY_TYPE, 0x7e744600, result);
    }
    const eDRIVE_BY_TYPE &WooshType() const {
        ATTRIB_CODEGEN_GETVALUE(eDRIVE_BY_TYPE, 0x7e744600);
    }
    bool SET_WooshType(const eDRIVE_BY_TYPE &input) {
        ATTRIB_CODEGEN_SETVALUE(eDRIVE_BY_TYPE, 0x7e744600, input);
    }
    bool no_trigger(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x73c58cbf);
    }
    bool no_trigger(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x73c58cbf, result);
    }
    const EA::Reflection::Bool &no_trigger() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x73c58cbf);
    }
    bool SET_no_trigger(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x73c58cbf, input);
    }
    bool rigidbodyspecs(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x7c90bb38);
    }
    bool rigidbodyspecs(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x7c90bb38, result);
    }
    const RefSpec &rigidbodyspecs() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x7c90bb38);
    }
    bool SET_rigidbodyspecs(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x7c90bb38, input);
    }
    bool start_sequencer(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x5739788b);
    }
    bool start_sequencer(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x5739788b, result);
    }
    const EA::Reflection::Bool &start_sequencer() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x5739788b);
    }
    bool SET_start_sequencer(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x5739788b, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    smackable &ConvertFromInstance(Instance &src) {}
    const smackable &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key smackable = 0xce70d7db;

}; // namespace ClassName

namespace Hash {
namespace smackable {

static const Key AI_AVOIDABLE = 0x6e4de905;
static const Key ALLOW_OFF_WORLD = 0xbee139f1;
static const Key AUTO_SIMPLIFY = 0xb5c0dac8;
static const Key BEHAVIORS = 0xda5f19f9;
static const Key CAMERA_AVOIDABLE = 0xe9d83d0c;
static const Key CAN_SIMPLIFY = 0xd1d3909a;
static const Key COST_TO_STATE = 0x6db7d192;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DETACH_FORCE = 0xbe71dbad;
static const Key DROPOUT = 0x44f1273b;
static const Key EventSequencer = 0x5aab860f;
static const Key ExplosionEffect = 0x360552da;
static const Key IsWooshable = 0x6f002423;
static const Key KILL_OFF_SCREEN = 0xc141f7f8;
static const Key MASS = 0x4a56503d;
static const Key MOMENT = 0xfb19212f;
static const Key NO_CAR_EFFECT = 0x1f989f01;
static const Key OnDetached = 0xd177bdaa;
static const Key OnHitGround = 0xd9c6cdfd;
static const Key OnHitObject = 0x18915735;
static const Key OnHitWorld = 0xe3167336;
static const Key OnScrapeGround = 0xdb823931;
static const Key OnScrapeObject = 0xdacb1c11;
static const Key OnScrapeWorld = 0x7100960c;
static const Key RESPAWN_TIME = 0x5f84f834;
static const Key SimplePhysics = 0xee0011e3;
static const Key WooshType = 0x7e744600;
static const Key no_trigger = 0x73c58cbf;
static const Key rigidbodyspecs = 0x7c90bb38;
static const Key start_sequencer = 0x5739788b;

}; // namespace smackable
}; // namespace Hash

inline Key Gen::smackable::ClassKey() {
    return ClassName::smackable;
}

}; // namespace Attrib

#endif
