#ifndef _attrib_gen_pvehicle_h
#define _attrib_gen_pvehicle_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct pvehicle : Instance {
    struct _LayoutStruct {
        UMath::Vector4 TENSOR_SCALE;                 // offset 0x0, size 0x10
        Attrib::StringKey MODEL;                     // offset 0x10, size 0x10
        EA::Reflection::Text DefaultPresetRide;      // offset 0x20, size 0x4
        EA::Reflection::Text CollectionName;         // offset 0x24, size 0x4
        EA::Reflection::Int32 engine_upgrades;       // offset 0x28, size 0x4
        EA::Reflection::Int32 transmission_upgrades; // offset 0x2c, size 0x4
        EA::Reflection::Int32 nos_upgrades;          // offset 0x30, size 0x4
        EA::Reflection::Int32 brakes_upgrades;       // offset 0x34, size 0x4
        EA::Reflection::Float MASS;                  // offset 0x38, size 0x4
        EA::Reflection::Int32 tires_upgrades;        // offset 0x3c, size 0x4
        Csis::Type_car_type VerbalType;              // offset 0x40, size 0x4
        EA::Reflection::Int32 induction_upgrades;    // offset 0x44, size 0x4
        EA::Reflection::Int32 chassis_upgrades;      // offset 0x48, size 0x4
        EA::Reflection::UInt8 HornType;              // offset 0x4c, size 0x1
        EA::Reflection::UInt8 TrafficEngType;        // offset 0x4d, size 0x1
    };

    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_AUDIO;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_DAMAGE;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_DRAW;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_EFFECTS;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_ENGINE;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_INPUT;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_RESET;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_RIGIDBODY;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_MECHANIC_SUSPENSION;
    typedef Attrib::StringKey TypeOf_BEHAVIOR_ORDER;
    typedef Attrib::StringKey TypeOf_CLASS;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Text TypeOf_DefaultPresetRide;
    typedef Attrib::StringKey TypeOf_EventSequencer;
    typedef EA::Reflection::Float TypeOf_HandlingRating;
    typedef EA::Reflection::UInt8 TypeOf_HornType;
    typedef EA::Reflection::Float TypeOf_MASS;
    typedef Attrib::StringKey TypeOf_MODEL;
    typedef EffectLinkageRecord TypeOf_OnBottomOut;
    typedef EffectLinkageRecord TypeOf_OnBottomScrape;
    typedef EffectLinkageRecord TypeOf_OnHitGround;
    typedef EffectLinkageRecord TypeOf_OnHitObject;
    typedef EffectLinkageRecord TypeOf_OnHitWorld;
    typedef EffectLinkageRecord TypeOf_OnScrapeGround;
    typedef EffectLinkageRecord TypeOf_OnScrapeObject;
    typedef EffectLinkageRecord TypeOf_OnScrapeWorld;
    typedef RefSpec TypeOf_OnTireBlow;
    typedef EA::Reflection::Bool TypeOf_PlayerUsable;
    typedef EA::Reflection::Bool TypeOf_RandomOpponent;
    typedef UpgradeSpecs TypeOf_ShiftSND;
    typedef UMath::Vector4 TypeOf_TENSOR_SCALE;
    typedef EA::Reflection::UInt8 TypeOf_TrafficEngType;
    typedef RefSpec TypeOf_Trailer;
    typedef EA::Reflection::Bool TypeOf_TruckSndFX;
    typedef UpgradeSpecs TypeOf_TurboSND;
    typedef Csis::Type_car_type TypeOf_VerbalType;
    typedef eDRIVE_BY_TYPE TypeOf_WooshType;
    typedef RefSpec TypeOf_aivehicle;
    typedef RefSpec TypeOf_brakes;
    typedef EA::Reflection::Int32 TypeOf_brakes_current;
    typedef EA::Reflection::Int32 TypeOf_brakes_upgrades;
    typedef RefSpec TypeOf_chassis;
    typedef EA::Reflection::Int32 TypeOf_chassis_current;
    typedef EA::Reflection::Int32 TypeOf_chassis_upgrades;
    typedef RefSpec TypeOf_chopperspecs;
    typedef RefSpec TypeOf_damagespecs;
    typedef RefSpec TypeOf_engine;
    typedef EA::Reflection::Int32 TypeOf_engine_current;
    typedef EA::Reflection::Int32 TypeOf_engine_upgrades;
    typedef RefSpec TypeOf_engineaudio;
    typedef RefSpec TypeOf_frontend;
    typedef RefSpec TypeOf_induction;
    typedef EA::Reflection::Int32 TypeOf_induction_current;
    typedef EA::Reflection::Int32 TypeOf_induction_upgrades;
    typedef RefSpec TypeOf_junkman;
    typedef EA::Reflection::Int32 TypeOf_junkman_current;
    typedef RefSpec TypeOf_nos;
    typedef EA::Reflection::Int32 TypeOf_nos_current;
    typedef EA::Reflection::Int32 TypeOf_nos_upgrades;
    typedef RefSpec TypeOf_rigidbodyspecs;
    typedef RefSpec TypeOf_tires;
    typedef EA::Reflection::Int32 TypeOf_tires_current;
    typedef EA::Reflection::Int32 TypeOf_tires_upgrades;
    typedef RefSpec TypeOf_transmission;
    typedef EA::Reflection::Int32 TypeOf_transmission_current;
    typedef EA::Reflection::Int32 TypeOf_transmission_upgrades;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("pvehicle");
    pvehicle(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pvehicle(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pvehicle(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pvehicle(const pvehicle &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pvehicle(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~pvehicle() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x4a97ec8f;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x4a97ec8f, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const pvehicle &operator=(const pvehicle &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const pvehicle &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BEHAVIOR_MECHANIC_AUDIO(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_AUDIO(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_AUDIO() const;
    bool SET_BEHAVIOR_MECHANIC_AUDIO(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_DAMAGE(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_DAMAGE(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_DAMAGE() const;
    bool SET_BEHAVIOR_MECHANIC_DAMAGE(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_DRAW(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_DRAW(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_DRAW() const;
    bool SET_BEHAVIOR_MECHANIC_DRAW(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_EFFECTS(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_EFFECTS(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_EFFECTS() const;
    bool SET_BEHAVIOR_MECHANIC_EFFECTS(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_ENGINE(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_ENGINE(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_ENGINE() const;
    bool SET_BEHAVIOR_MECHANIC_ENGINE(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_INPUT(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_INPUT(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_INPUT() const;
    bool SET_BEHAVIOR_MECHANIC_INPUT(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_RESET(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_RESET(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_RESET() const;
    bool SET_BEHAVIOR_MECHANIC_RESET(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_RIGIDBODY(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_RIGIDBODY(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_RIGIDBODY() const;
    bool SET_BEHAVIOR_MECHANIC_RIGIDBODY(const Attrib::StringKey &input);

    bool BEHAVIOR_MECHANIC_SUSPENSION(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_MECHANIC_SUSPENSION(Attrib::StringKey &result) const;
    const Attrib::StringKey &BEHAVIOR_MECHANIC_SUSPENSION() const;
    bool SET_BEHAVIOR_MECHANIC_SUSPENSION(const Attrib::StringKey &input);

    bool BEHAVIOR_ORDER(TAttrib<Attrib::StringKey> &result) const;
    bool BEHAVIOR_ORDER(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &BEHAVIOR_ORDER(unsigned int index) const;
    unsigned int Num_BEHAVIOR_ORDER() const;
    bool SET_BEHAVIOR_ORDER(const Attrib::StringKey &input, unsigned int index);

    bool CLASS(TAttrib<Attrib::StringKey> &result) const;
    bool CLASS(Attrib::StringKey &result) const;
    const Attrib::StringKey &CLASS() const;
    bool SET_CLASS(const Attrib::StringKey &input);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool DefaultPresetRide(TAttrib<EA::Reflection::Text> &result) const;
    bool DefaultPresetRide(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &DefaultPresetRide() const;
    bool SET_DefaultPresetRide(const EA::Reflection::Text &input);

    bool EventSequencer(TAttrib<Attrib::StringKey> &result) const;
    bool EventSequencer(Attrib::StringKey &result) const;
    const Attrib::StringKey &EventSequencer() const;
    bool SET_EventSequencer(const Attrib::StringKey &input);

    bool HandlingRating(TAttrib<EA::Reflection::Float> &result) const;
    bool HandlingRating(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &HandlingRating(unsigned int index) const;
    unsigned int Num_HandlingRating() const;
    bool SET_HandlingRating(const EA::Reflection::Float &input, unsigned int index);

    bool HornType(TAttrib<EA::Reflection::UInt8> &result) const;
    bool HornType(EA::Reflection::UInt8 &result) const;
    const EA::Reflection::UInt8 &HornType() const;
    bool SET_HornType(const EA::Reflection::UInt8 &input);

    bool MASS(TAttrib<EA::Reflection::Float> &result) const;
    bool MASS(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MASS() const;
    bool SET_MASS(const EA::Reflection::Float &input);

    bool MODEL(TAttrib<Attrib::StringKey> &result) const;
    bool MODEL(Attrib::StringKey &result) const;
    const Attrib::StringKey &MODEL() const;
    bool SET_MODEL(const Attrib::StringKey &input);

    bool OnBottomOut(TAttrib<EffectLinkageRecord> &result) const;
    bool OnBottomOut(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnBottomOut(unsigned int index) const;
    unsigned int Num_OnBottomOut() const;
    bool SET_OnBottomOut(const EffectLinkageRecord &input, unsigned int index);

    bool OnBottomScrape(TAttrib<EffectLinkageRecord> &result) const;
    bool OnBottomScrape(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnBottomScrape(unsigned int index) const;
    unsigned int Num_OnBottomScrape() const;
    bool SET_OnBottomScrape(const EffectLinkageRecord &input, unsigned int index);

    bool OnHitGround(TAttrib<EffectLinkageRecord> &result) const;
    bool OnHitGround(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnHitGround(unsigned int index) const;
    unsigned int Num_OnHitGround() const;
    bool SET_OnHitGround(const EffectLinkageRecord &input, unsigned int index);

    bool OnHitObject(TAttrib<EffectLinkageRecord> &result) const;
    bool OnHitObject(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnHitObject(unsigned int index) const;
    unsigned int Num_OnHitObject() const;
    bool SET_OnHitObject(const EffectLinkageRecord &input, unsigned int index);

    bool OnHitWorld(TAttrib<EffectLinkageRecord> &result) const;
    bool OnHitWorld(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnHitWorld(unsigned int index) const;
    unsigned int Num_OnHitWorld() const;
    bool SET_OnHitWorld(const EffectLinkageRecord &input, unsigned int index);

    bool OnScrapeGround(TAttrib<EffectLinkageRecord> &result) const;
    bool OnScrapeGround(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnScrapeGround(unsigned int index) const;
    unsigned int Num_OnScrapeGround() const;
    bool SET_OnScrapeGround(const EffectLinkageRecord &input, unsigned int index);

    bool OnScrapeObject(TAttrib<EffectLinkageRecord> &result) const;
    bool OnScrapeObject(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnScrapeObject(unsigned int index) const;
    unsigned int Num_OnScrapeObject() const;
    bool SET_OnScrapeObject(const EffectLinkageRecord &input, unsigned int index);

    bool OnScrapeWorld(TAttrib<EffectLinkageRecord> &result) const;
    bool OnScrapeWorld(EffectLinkageRecord &result, unsigned int index) const;
    const EffectLinkageRecord &OnScrapeWorld(unsigned int index) const;
    unsigned int Num_OnScrapeWorld() const;
    bool SET_OnScrapeWorld(const EffectLinkageRecord &input, unsigned int index);

    bool OnTireBlow(TAttrib<RefSpec> &result) const;
    bool OnTireBlow(RefSpec &result) const;
    const RefSpec &OnTireBlow() const;
    bool SET_OnTireBlow(const RefSpec &input);

    bool PlayerUsable(TAttrib<EA::Reflection::Bool> &result) const;
    bool PlayerUsable(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &PlayerUsable() const;
    bool SET_PlayerUsable(const EA::Reflection::Bool &input);

    bool RandomOpponent(TAttrib<EA::Reflection::Bool> &result) const;
    bool RandomOpponent(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &RandomOpponent() const;
    bool SET_RandomOpponent(const EA::Reflection::Bool &input);

    bool ShiftSND(TAttrib<UpgradeSpecs> &result) const;
    bool ShiftSND(UpgradeSpecs &result, unsigned int index) const;
    const UpgradeSpecs &ShiftSND(unsigned int index) const;
    unsigned int Num_ShiftSND() const;
    bool SET_ShiftSND(const UpgradeSpecs &input, unsigned int index);

    bool TENSOR_SCALE(TAttrib<UMath::Vector4> &result) const;
    bool TENSOR_SCALE(UMath::Vector4 &result) const;
    const UMath::Vector4 &TENSOR_SCALE() const;
    bool SET_TENSOR_SCALE(const UMath::Vector4 &input);

    bool TrafficEngType(TAttrib<EA::Reflection::UInt8> &result) const;
    bool TrafficEngType(EA::Reflection::UInt8 &result) const;
    const EA::Reflection::UInt8 &TrafficEngType() const;
    bool SET_TrafficEngType(const EA::Reflection::UInt8 &input);

    bool Trailer(TAttrib<RefSpec> &result) const;
    bool Trailer(RefSpec &result) const;
    const RefSpec &Trailer() const;
    bool SET_Trailer(const RefSpec &input);

    bool TruckSndFX(TAttrib<EA::Reflection::Bool> &result) const;
    bool TruckSndFX(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &TruckSndFX() const;
    bool SET_TruckSndFX(const EA::Reflection::Bool &input);

    bool TurboSND(TAttrib<UpgradeSpecs> &result) const;
    bool TurboSND(UpgradeSpecs &result, unsigned int index) const;
    const UpgradeSpecs &TurboSND(unsigned int index) const;
    unsigned int Num_TurboSND() const;
    bool SET_TurboSND(const UpgradeSpecs &input, unsigned int index);

    bool VerbalType(TAttrib<Csis::Type_car_type> &result) const;
    bool VerbalType(Csis::Type_car_type &result) const;
    const Csis::Type_car_type &VerbalType() const;
    bool SET_VerbalType(const Csis::Type_car_type &input);

    bool WooshType(TAttrib<eDRIVE_BY_TYPE> &result) const;
    bool WooshType(eDRIVE_BY_TYPE &result) const;
    const eDRIVE_BY_TYPE &WooshType() const;
    bool SET_WooshType(const eDRIVE_BY_TYPE &input);

    bool aivehicle(TAttrib<RefSpec> &result) const;
    bool aivehicle(RefSpec &result) const;
    const RefSpec &aivehicle() const;
    bool SET_aivehicle(const RefSpec &input);

    bool brakes(TAttrib<RefSpec> &result) const;
    bool brakes(RefSpec &result, unsigned int index) const;
    const RefSpec &brakes(unsigned int index) const;
    unsigned int Num_brakes() const;
    bool SET_brakes(const RefSpec &input, unsigned int index);

    bool brakes_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool brakes_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &brakes_current() const;
    bool SET_brakes_current(const EA::Reflection::Int32 &input);

    bool brakes_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool brakes_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &brakes_upgrades() const;
    bool SET_brakes_upgrades(const EA::Reflection::Int32 &input);

    bool chassis(TAttrib<RefSpec> &result) const;
    bool chassis(RefSpec &result, unsigned int index) const;
    const RefSpec &chassis(unsigned int index) const;
    unsigned int Num_chassis() const;
    bool SET_chassis(const RefSpec &input, unsigned int index);

    bool chassis_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool chassis_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &chassis_current() const;
    bool SET_chassis_current(const EA::Reflection::Int32 &input);

    bool chassis_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool chassis_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &chassis_upgrades() const;
    bool SET_chassis_upgrades(const EA::Reflection::Int32 &input);

    bool chopperspecs(TAttrib<RefSpec> &result) const;
    bool chopperspecs(RefSpec &result) const;
    const RefSpec &chopperspecs() const;
    bool SET_chopperspecs(const RefSpec &input);

    bool damagespecs(TAttrib<RefSpec> &result) const;
    bool damagespecs(RefSpec &result) const;
    const RefSpec &damagespecs() const;
    bool SET_damagespecs(const RefSpec &input);

    bool engine(TAttrib<RefSpec> &result) const;
    bool engine(RefSpec &result, unsigned int index) const;
    const RefSpec &engine(unsigned int index) const;
    unsigned int Num_engine() const;
    bool SET_engine(const RefSpec &input, unsigned int index);

    bool engine_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool engine_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &engine_current() const;
    bool SET_engine_current(const EA::Reflection::Int32 &input);

    bool engine_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool engine_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &engine_upgrades() const;
    bool SET_engine_upgrades(const EA::Reflection::Int32 &input);

    bool engineaudio(TAttrib<RefSpec> &result) const;
    bool engineaudio(RefSpec &result, unsigned int index) const;
    const RefSpec &engineaudio(unsigned int index) const;
    unsigned int Num_engineaudio() const;
    bool SET_engineaudio(const RefSpec &input, unsigned int index);

    bool frontend(TAttrib<RefSpec> &result) const;
    bool frontend(RefSpec &result) const;
    const RefSpec &frontend() const;
    bool SET_frontend(const RefSpec &input);

    bool induction(TAttrib<RefSpec> &result) const;
    bool induction(RefSpec &result, unsigned int index) const;
    const RefSpec &induction(unsigned int index) const;
    unsigned int Num_induction() const;
    bool SET_induction(const RefSpec &input, unsigned int index);

    bool induction_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool induction_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &induction_current() const;
    bool SET_induction_current(const EA::Reflection::Int32 &input);

    bool induction_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool induction_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &induction_upgrades() const;
    bool SET_induction_upgrades(const EA::Reflection::Int32 &input);

    bool junkman(TAttrib<RefSpec> &result) const;
    bool junkman(RefSpec &result) const;
    const RefSpec &junkman() const;
    bool SET_junkman(const RefSpec &input);

    bool junkman_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool junkman_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &junkman_current() const;
    bool SET_junkman_current(const EA::Reflection::Int32 &input);

    bool nos(TAttrib<RefSpec> &result) const;
    bool nos(RefSpec &result, unsigned int index) const;
    const RefSpec &nos(unsigned int index) const;
    unsigned int Num_nos() const;
    bool SET_nos(const RefSpec &input, unsigned int index);

    bool nos_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool nos_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &nos_current() const;
    bool SET_nos_current(const EA::Reflection::Int32 &input);

    bool nos_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool nos_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &nos_upgrades() const;
    bool SET_nos_upgrades(const EA::Reflection::Int32 &input);

    bool rigidbodyspecs(TAttrib<RefSpec> &result) const;
    bool rigidbodyspecs(RefSpec &result) const;
    const RefSpec &rigidbodyspecs() const;
    bool SET_rigidbodyspecs(const RefSpec &input);

    bool tires(TAttrib<RefSpec> &result) const;
    bool tires(RefSpec &result, unsigned int index) const;
    const RefSpec &tires(unsigned int index) const;
    unsigned int Num_tires() const;
    bool SET_tires(const RefSpec &input, unsigned int index);

    bool tires_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool tires_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &tires_current() const;
    bool SET_tires_current(const EA::Reflection::Int32 &input);

    bool tires_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool tires_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &tires_upgrades() const;
    bool SET_tires_upgrades(const EA::Reflection::Int32 &input);

    bool transmission(TAttrib<RefSpec> &result) const;
    bool transmission(RefSpec &result, unsigned int index) const;
    const RefSpec &transmission(unsigned int index) const;
    unsigned int Num_transmission() const;
    bool SET_transmission(const RefSpec &input, unsigned int index);

    bool transmission_current(TAttrib<EA::Reflection::Int32> &result) const;
    bool transmission_current(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &transmission_current() const;
    bool SET_transmission_current(const EA::Reflection::Int32 &input);

    bool transmission_upgrades(TAttrib<EA::Reflection::Int32> &result) const;
    bool transmission_upgrades(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &transmission_upgrades() const;
    bool SET_transmission_upgrades(const EA::Reflection::Int32 &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    pvehicle &ConvertFromInstance(Instance &src) {}
    const pvehicle &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key pvehicle = 0x4a97ec8f;

}; // namespace ClassName

namespace Hash {
namespace pvehicle {

static const Key BEHAVIOR_MECHANIC_AUDIO = 0xfb0b5be9;
static const Key BEHAVIOR_MECHANIC_DAMAGE = 0x858ed6e3;
static const Key BEHAVIOR_MECHANIC_DRAW = 0xb230ade1;
static const Key BEHAVIOR_MECHANIC_EFFECTS = 0x59c2beb1;
static const Key BEHAVIOR_MECHANIC_ENGINE = 0xa3e13328;
static const Key BEHAVIOR_MECHANIC_INPUT = 0xc3fa0cc4;
static const Key BEHAVIOR_MECHANIC_RESET = 0x8013456f;
static const Key BEHAVIOR_MECHANIC_RIGIDBODY = 0x8ba55001;
static const Key BEHAVIOR_MECHANIC_SUSPENSION = 0x511abd7b;
static const Key BEHAVIOR_ORDER = 0x104e9d16;
static const Key CLASS = 0x0ef6ddf2;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DefaultPresetRide = 0xaa27e71c;
static const Key EventSequencer = 0x5aab860f;
static const Key HandlingRating = 0xaacbe2e7;
static const Key HornType = 0xa481f8ff;
static const Key MASS = 0x4a56503d;
static const Key MODEL = 0x9047c9e0;
static const Key OnBottomOut = 0x31047ebc;
static const Key OnBottomScrape = 0xfc03ad6a;
static const Key OnHitGround = 0xd9c6cdfd;
static const Key OnHitObject = 0x18915735;
static const Key OnHitWorld = 0xe3167336;
static const Key OnScrapeGround = 0xdb823931;
static const Key OnScrapeObject = 0xdacb1c11;
static const Key OnScrapeWorld = 0x7100960c;
static const Key OnTireBlow = 0xd42e792f;
static const Key PlayerUsable = 0xf099b6ac;
static const Key RandomOpponent = 0x6df0abfe;
static const Key ShiftSND = 0x8ae8bee0;
static const Key TENSOR_SCALE = 0x868dffdd;
static const Key TrafficEngType = 0x09af94ed;
static const Key Trailer = 0x9a5537fe;
static const Key TruckSndFX = 0xeacb7696;
static const Key TurboSND = 0x0e9bfb66;
static const Key VerbalType = 0x368a544c;
static const Key WooshType = 0x7e744600;
static const Key aivehicle = 0x22515733;
static const Key brakes = 0x36350867;
static const Key brakes_current = 0x552c3d22;
static const Key brakes_upgrades = 0x4e168120;
static const Key chassis = 0xafa210f0;
static const Key chassis_current = 0x7845f9bc;
static const Key chassis_upgrades = 0xf62a062d;
static const Key chopperspecs = 0x5d898ee7;
static const Key damagespecs = 0xc1f0b434;
static const Key engine = 0xf1f5fbc7;
static const Key engine_current = 0xb12ccb69;
static const Key engine_upgrades = 0x7708db25;
static const Key engineaudio = 0x50eab0e6;
static const Key frontend = 0x85885722;
static const Key induction = 0xc92a0142;
static const Key induction_current = 0x7f440672;
static const Key induction_upgrades = 0x7202e66e;
static const Key junkman = 0x171737e9;
static const Key junkman_current = 0xcdc136e8;
static const Key nos = 0xb1669f64;
static const Key nos_current = 0x8f37beae;
static const Key nos_upgrades = 0x9dc66e81;
static const Key rigidbodyspecs = 0x7c90bb38;
static const Key tires = 0xbd38d1ca;
static const Key tires_current = 0x34e73f83;
static const Key tires_upgrades = 0x75b0f76b;
static const Key transmission = 0x07a7a3e5;
static const Key transmission_current = 0x170d5554;
static const Key transmission_upgrades = 0x860e4531;

}; // namespace pvehicle
}; // namespace Hash

inline Key Gen::pvehicle::ClassKey() {
    return ClassName::pvehicle;
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_AUDIO(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xfb0b5be9);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_AUDIO() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xfb0b5be9);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_AUDIO(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xfb0b5be9, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_AUDIO(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xfb0b5be9, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_DAMAGE(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x858ed6e3);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_DAMAGE() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x858ed6e3);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_DAMAGE(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x858ed6e3, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_DAMAGE(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x858ed6e3, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_DRAW(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xb230ade1);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_DRAW() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xb230ade1);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_DRAW(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xb230ade1, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_DRAW(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xb230ade1, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_EFFECTS(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x59c2beb1);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_EFFECTS() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x59c2beb1);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_EFFECTS(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x59c2beb1, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_EFFECTS(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x59c2beb1, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_ENGINE(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xa3e13328);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_ENGINE() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xa3e13328);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_ENGINE(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xa3e13328, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_ENGINE(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xa3e13328, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_INPUT(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xc3fa0cc4);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_INPUT() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xc3fa0cc4);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_INPUT(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xc3fa0cc4, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_INPUT(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xc3fa0cc4, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_RESET(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x8013456f);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_RESET() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x8013456f);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_RESET(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x8013456f, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_RESET(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x8013456f, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_RIGIDBODY(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x8ba55001);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_RIGIDBODY() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x8ba55001);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_RIGIDBODY(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x8ba55001, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_RIGIDBODY(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x8ba55001, input);
}

inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_SUSPENSION(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x511abd7b);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_MECHANIC_SUSPENSION() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x511abd7b);
}
inline bool Gen::pvehicle::BEHAVIOR_MECHANIC_SUSPENSION(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x511abd7b, result);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_MECHANIC_SUSPENSION(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x511abd7b, input);
}

inline bool Gen::pvehicle::BEHAVIOR_ORDER(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x104e9d16);
}
inline const Attrib::StringKey &Gen::pvehicle::BEHAVIOR_ORDER(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x104e9d16, index);
}
inline bool Gen::pvehicle::BEHAVIOR_ORDER(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x104e9d16, result, index);
}
inline unsigned int Gen::pvehicle::Num_BEHAVIOR_ORDER() const {
    ATTRIB_CODEGEN_GETLENGTH(0x104e9d16);
}
inline bool Gen::pvehicle::SET_BEHAVIOR_ORDER(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x104e9d16, input, index);
}

inline bool Gen::pvehicle::CLASS(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x0ef6ddf2);
}
inline const Attrib::StringKey &Gen::pvehicle::CLASS() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x0ef6ddf2);
}
inline bool Gen::pvehicle::CLASS(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x0ef6ddf2, result);
}
inline bool Gen::pvehicle::SET_CLASS(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x0ef6ddf2, input);
}

inline bool Gen::pvehicle::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::pvehicle::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::pvehicle::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::pvehicle::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::pvehicle::DefaultPresetRide(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xaa27e71c);
}
inline const EA::Reflection::Text &Gen::pvehicle::DefaultPresetRide() const {
    ATTRIB_CODEGEN_GETLAYOUT(DefaultPresetRide);
}
inline bool Gen::pvehicle::DefaultPresetRide(EA::Reflection::Text &result) const {
    result = DefaultPresetRide();
    return true;
}
inline bool Gen::pvehicle::SET_DefaultPresetRide(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DefaultPresetRide, input);
}

inline bool Gen::pvehicle::EventSequencer(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5aab860f);
}
inline const Attrib::StringKey &Gen::pvehicle::EventSequencer() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x5aab860f);
}
inline bool Gen::pvehicle::EventSequencer(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x5aab860f, result);
}
inline bool Gen::pvehicle::SET_EventSequencer(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x5aab860f, input);
}

inline bool Gen::pvehicle::HandlingRating(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xaacbe2e7);
}
inline const EA::Reflection::Float &Gen::pvehicle::HandlingRating(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0xaacbe2e7, index);
}
inline bool Gen::pvehicle::HandlingRating(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0xaacbe2e7, result, index);
}
inline unsigned int Gen::pvehicle::Num_HandlingRating() const {
    ATTRIB_CODEGEN_GETLENGTH(0xaacbe2e7);
}
inline bool Gen::pvehicle::SET_HandlingRating(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0xaacbe2e7, input, index);
}

inline bool Gen::pvehicle::HornType(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xa481f8ff);
}
inline const EA::Reflection::UInt8 &Gen::pvehicle::HornType() const {
    ATTRIB_CODEGEN_GETLAYOUT(HornType);
}
inline bool Gen::pvehicle::HornType(EA::Reflection::UInt8 &result) const {
    result = HornType();
    return true;
}
inline bool Gen::pvehicle::SET_HornType(const EA::Reflection::UInt8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HornType, input);
}

inline bool Gen::pvehicle::MASS(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4a56503d);
}
inline const EA::Reflection::Float &Gen::pvehicle::MASS() const {
    ATTRIB_CODEGEN_GETLAYOUT(MASS);
}
inline bool Gen::pvehicle::MASS(EA::Reflection::Float &result) const {
    result = MASS();
    return true;
}
inline bool Gen::pvehicle::SET_MASS(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MASS, input);
}

inline bool Gen::pvehicle::MODEL(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9047c9e0);
}
inline const Attrib::StringKey &Gen::pvehicle::MODEL() const {
    ATTRIB_CODEGEN_GETLAYOUT(MODEL);
}
inline bool Gen::pvehicle::MODEL(Attrib::StringKey &result) const {
    result = MODEL();
    return true;
}
inline bool Gen::pvehicle::SET_MODEL(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MODEL, input);
}

inline bool Gen::pvehicle::OnBottomOut(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0x31047ebc);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnBottomOut(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0x31047ebc, index);
}
inline bool Gen::pvehicle::OnBottomOut(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0x31047ebc, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnBottomOut() const {
    ATTRIB_CODEGEN_GETLENGTH(0x31047ebc);
}
inline bool Gen::pvehicle::SET_OnBottomOut(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0x31047ebc, input, index);
}

inline bool Gen::pvehicle::OnBottomScrape(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xfc03ad6a);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnBottomScrape(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xfc03ad6a, index);
}
inline bool Gen::pvehicle::OnBottomScrape(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xfc03ad6a, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnBottomScrape() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfc03ad6a);
}
inline bool Gen::pvehicle::SET_OnBottomScrape(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xfc03ad6a, input, index);
}

inline bool Gen::pvehicle::OnHitGround(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xd9c6cdfd);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnHitGround(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, index);
}
inline bool Gen::pvehicle::OnHitGround(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnHitGround() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd9c6cdfd);
}
inline bool Gen::pvehicle::SET_OnHitGround(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xd9c6cdfd, input, index);
}

inline bool Gen::pvehicle::OnHitObject(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0x18915735);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnHitObject(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0x18915735, index);
}
inline bool Gen::pvehicle::OnHitObject(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0x18915735, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnHitObject() const {
    ATTRIB_CODEGEN_GETLENGTH(0x18915735);
}
inline bool Gen::pvehicle::SET_OnHitObject(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0x18915735, input, index);
}

inline bool Gen::pvehicle::OnHitWorld(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xe3167336);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnHitWorld(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, index);
}
inline bool Gen::pvehicle::OnHitWorld(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnHitWorld() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe3167336);
}
inline bool Gen::pvehicle::SET_OnHitWorld(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xe3167336, input, index);
}

inline bool Gen::pvehicle::OnScrapeGround(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xdb823931);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnScrapeGround(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, index);
}
inline bool Gen::pvehicle::OnScrapeGround(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnScrapeGround() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdb823931);
}
inline bool Gen::pvehicle::SET_OnScrapeGround(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xdb823931, input, index);
}

inline bool Gen::pvehicle::OnScrapeObject(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0xdacb1c11);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnScrapeObject(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, index);
}
inline bool Gen::pvehicle::OnScrapeObject(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnScrapeObject() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdacb1c11);
}
inline bool Gen::pvehicle::SET_OnScrapeObject(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0xdacb1c11, input, index);
}

inline bool Gen::pvehicle::OnScrapeWorld(TAttrib<EffectLinkageRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EffectLinkageRecord, 0x7100960c);
}
inline const EffectLinkageRecord &Gen::pvehicle::OnScrapeWorld(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, index);
}
inline bool Gen::pvehicle::OnScrapeWorld(EffectLinkageRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, result, index);
}
inline unsigned int Gen::pvehicle::Num_OnScrapeWorld() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7100960c);
}
inline bool Gen::pvehicle::SET_OnScrapeWorld(const EffectLinkageRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EffectLinkageRecord, 0x7100960c, input, index);
}

inline bool Gen::pvehicle::OnTireBlow(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd42e792f);
}
inline const RefSpec &Gen::pvehicle::OnTireBlow() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd42e792f);
}
inline bool Gen::pvehicle::OnTireBlow(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd42e792f, result);
}
inline bool Gen::pvehicle::SET_OnTireBlow(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd42e792f, input);
}

inline bool Gen::pvehicle::PlayerUsable(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xf099b6ac);
}
inline const EA::Reflection::Bool &Gen::pvehicle::PlayerUsable() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xf099b6ac);
}
inline bool Gen::pvehicle::PlayerUsable(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xf099b6ac, result);
}
inline bool Gen::pvehicle::SET_PlayerUsable(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xf099b6ac, input);
}

inline bool Gen::pvehicle::RandomOpponent(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6df0abfe);
}
inline const EA::Reflection::Bool &Gen::pvehicle::RandomOpponent() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6df0abfe);
}
inline bool Gen::pvehicle::RandomOpponent(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6df0abfe, result);
}
inline bool Gen::pvehicle::SET_RandomOpponent(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6df0abfe, input);
}

inline bool Gen::pvehicle::ShiftSND(TAttrib<UpgradeSpecs> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UpgradeSpecs, 0x8ae8bee0);
}
inline const UpgradeSpecs &Gen::pvehicle::ShiftSND(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(UpgradeSpecs, 0x8ae8bee0, index);
}
inline bool Gen::pvehicle::ShiftSND(UpgradeSpecs &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(UpgradeSpecs, 0x8ae8bee0, result, index);
}
inline unsigned int Gen::pvehicle::Num_ShiftSND() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8ae8bee0);
}
inline bool Gen::pvehicle::SET_ShiftSND(const UpgradeSpecs &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(UpgradeSpecs, 0x8ae8bee0, input, index);
}

inline bool Gen::pvehicle::TENSOR_SCALE(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x868dffdd);
}
inline const UMath::Vector4 &Gen::pvehicle::TENSOR_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(TENSOR_SCALE);
}
inline bool Gen::pvehicle::TENSOR_SCALE(UMath::Vector4 &result) const {
    result = TENSOR_SCALE();
    return true;
}
inline bool Gen::pvehicle::SET_TENSOR_SCALE(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TENSOR_SCALE, input);
}

inline bool Gen::pvehicle::TrafficEngType(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0x09af94ed);
}
inline const EA::Reflection::UInt8 &Gen::pvehicle::TrafficEngType() const {
    ATTRIB_CODEGEN_GETLAYOUT(TrafficEngType);
}
inline bool Gen::pvehicle::TrafficEngType(EA::Reflection::UInt8 &result) const {
    result = TrafficEngType();
    return true;
}
inline bool Gen::pvehicle::SET_TrafficEngType(const EA::Reflection::UInt8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TrafficEngType, input);
}

inline bool Gen::pvehicle::Trailer(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x9a5537fe);
}
inline const RefSpec &Gen::pvehicle::Trailer() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x9a5537fe);
}
inline bool Gen::pvehicle::Trailer(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x9a5537fe, result);
}
inline bool Gen::pvehicle::SET_Trailer(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x9a5537fe, input);
}

inline bool Gen::pvehicle::TruckSndFX(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xeacb7696);
}
inline const EA::Reflection::Bool &Gen::pvehicle::TruckSndFX() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xeacb7696);
}
inline bool Gen::pvehicle::TruckSndFX(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xeacb7696, result);
}
inline bool Gen::pvehicle::SET_TruckSndFX(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xeacb7696, input);
}

inline bool Gen::pvehicle::TurboSND(TAttrib<UpgradeSpecs> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UpgradeSpecs, 0x0e9bfb66);
}
inline const UpgradeSpecs &Gen::pvehicle::TurboSND(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(UpgradeSpecs, 0x0e9bfb66, index);
}
inline bool Gen::pvehicle::TurboSND(UpgradeSpecs &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(UpgradeSpecs, 0x0e9bfb66, result, index);
}
inline unsigned int Gen::pvehicle::Num_TurboSND() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0e9bfb66);
}
inline bool Gen::pvehicle::SET_TurboSND(const UpgradeSpecs &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(UpgradeSpecs, 0x0e9bfb66, input, index);
}

inline bool Gen::pvehicle::VerbalType(TAttrib<Csis::Type_car_type> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Csis::Type_car_type, 0x368a544c);
}
inline const Csis::Type_car_type &Gen::pvehicle::VerbalType() const {
    ATTRIB_CODEGEN_GETLAYOUT(VerbalType);
}
inline bool Gen::pvehicle::VerbalType(Csis::Type_car_type &result) const {
    result = VerbalType();
    return true;
}
inline bool Gen::pvehicle::SET_VerbalType(const Csis::Type_car_type &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VerbalType, input);
}

inline bool Gen::pvehicle::WooshType(TAttrib<eDRIVE_BY_TYPE> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(eDRIVE_BY_TYPE, 0x7e744600);
}
inline const eDRIVE_BY_TYPE &Gen::pvehicle::WooshType() const {
    ATTRIB_CODEGEN_GETVALUE(eDRIVE_BY_TYPE, 0x7e744600);
}
inline bool Gen::pvehicle::WooshType(eDRIVE_BY_TYPE &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(eDRIVE_BY_TYPE, 0x7e744600, result);
}
inline bool Gen::pvehicle::SET_WooshType(const eDRIVE_BY_TYPE &input) {
    ATTRIB_CODEGEN_SETVALUE(eDRIVE_BY_TYPE, 0x7e744600, input);
}

inline bool Gen::pvehicle::aivehicle(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x22515733);
}
inline const RefSpec &Gen::pvehicle::aivehicle() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x22515733);
}
inline bool Gen::pvehicle::aivehicle(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x22515733, result);
}
inline bool Gen::pvehicle::SET_aivehicle(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x22515733, input);
}

inline bool Gen::pvehicle::brakes(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x36350867);
}
inline const RefSpec &Gen::pvehicle::brakes(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x36350867, index);
}
inline bool Gen::pvehicle::brakes(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x36350867, result, index);
}
inline unsigned int Gen::pvehicle::Num_brakes() const {
    ATTRIB_CODEGEN_GETLENGTH(0x36350867);
}
inline bool Gen::pvehicle::SET_brakes(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x36350867, input, index);
}

inline bool Gen::pvehicle::brakes_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x552c3d22);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::brakes_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x552c3d22);
}
inline bool Gen::pvehicle::brakes_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x552c3d22, result);
}
inline bool Gen::pvehicle::SET_brakes_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x552c3d22, input);
}

inline bool Gen::pvehicle::brakes_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x4e168120);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::brakes_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(brakes_upgrades);
}
inline bool Gen::pvehicle::brakes_upgrades(EA::Reflection::Int32 &result) const {
    result = brakes_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_brakes_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(brakes_upgrades, input);
}

inline bool Gen::pvehicle::chassis(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xafa210f0);
}
inline const RefSpec &Gen::pvehicle::chassis(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xafa210f0, index);
}
inline bool Gen::pvehicle::chassis(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xafa210f0, result, index);
}
inline unsigned int Gen::pvehicle::Num_chassis() const {
    ATTRIB_CODEGEN_GETLENGTH(0xafa210f0);
}
inline bool Gen::pvehicle::SET_chassis(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xafa210f0, input, index);
}

inline bool Gen::pvehicle::chassis_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7845f9bc);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::chassis_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x7845f9bc);
}
inline bool Gen::pvehicle::chassis_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x7845f9bc, result);
}
inline bool Gen::pvehicle::SET_chassis_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x7845f9bc, input);
}

inline bool Gen::pvehicle::chassis_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf62a062d);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::chassis_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(chassis_upgrades);
}
inline bool Gen::pvehicle::chassis_upgrades(EA::Reflection::Int32 &result) const {
    result = chassis_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_chassis_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(chassis_upgrades, input);
}

inline bool Gen::pvehicle::chopperspecs(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x5d898ee7);
}
inline const RefSpec &Gen::pvehicle::chopperspecs() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x5d898ee7);
}
inline bool Gen::pvehicle::chopperspecs(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x5d898ee7, result);
}
inline bool Gen::pvehicle::SET_chopperspecs(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x5d898ee7, input);
}

inline bool Gen::pvehicle::damagespecs(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xc1f0b434);
}
inline const RefSpec &Gen::pvehicle::damagespecs() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xc1f0b434);
}
inline bool Gen::pvehicle::damagespecs(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xc1f0b434, result);
}
inline bool Gen::pvehicle::SET_damagespecs(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xc1f0b434, input);
}

inline bool Gen::pvehicle::engine(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xf1f5fbc7);
}
inline const RefSpec &Gen::pvehicle::engine(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xf1f5fbc7, index);
}
inline bool Gen::pvehicle::engine(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xf1f5fbc7, result, index);
}
inline unsigned int Gen::pvehicle::Num_engine() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf1f5fbc7);
}
inline bool Gen::pvehicle::SET_engine(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xf1f5fbc7, input, index);
}

inline bool Gen::pvehicle::engine_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xb12ccb69);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::engine_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xb12ccb69);
}
inline bool Gen::pvehicle::engine_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xb12ccb69, result);
}
inline bool Gen::pvehicle::SET_engine_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xb12ccb69, input);
}

inline bool Gen::pvehicle::engine_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7708db25);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::engine_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(engine_upgrades);
}
inline bool Gen::pvehicle::engine_upgrades(EA::Reflection::Int32 &result) const {
    result = engine_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_engine_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(engine_upgrades, input);
}

inline bool Gen::pvehicle::engineaudio(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x50eab0e6);
}
inline const RefSpec &Gen::pvehicle::engineaudio(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x50eab0e6, index);
}
inline bool Gen::pvehicle::engineaudio(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x50eab0e6, result, index);
}
inline unsigned int Gen::pvehicle::Num_engineaudio() const {
    ATTRIB_CODEGEN_GETLENGTH(0x50eab0e6);
}
inline bool Gen::pvehicle::SET_engineaudio(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x50eab0e6, input, index);
}

inline bool Gen::pvehicle::frontend(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x85885722);
}
inline const RefSpec &Gen::pvehicle::frontend() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x85885722);
}
inline bool Gen::pvehicle::frontend(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x85885722, result);
}
inline bool Gen::pvehicle::SET_frontend(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x85885722, input);
}

inline bool Gen::pvehicle::induction(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xc92a0142);
}
inline const RefSpec &Gen::pvehicle::induction(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xc92a0142, index);
}
inline bool Gen::pvehicle::induction(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xc92a0142, result, index);
}
inline unsigned int Gen::pvehicle::Num_induction() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc92a0142);
}
inline bool Gen::pvehicle::SET_induction(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xc92a0142, input, index);
}

inline bool Gen::pvehicle::induction_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7f440672);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::induction_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x7f440672);
}
inline bool Gen::pvehicle::induction_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x7f440672, result);
}
inline bool Gen::pvehicle::SET_induction_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x7f440672, input);
}

inline bool Gen::pvehicle::induction_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7202e66e);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::induction_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(induction_upgrades);
}
inline bool Gen::pvehicle::induction_upgrades(EA::Reflection::Int32 &result) const {
    result = induction_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_induction_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(induction_upgrades, input);
}

inline bool Gen::pvehicle::junkman(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x171737e9);
}
inline const RefSpec &Gen::pvehicle::junkman() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x171737e9);
}
inline bool Gen::pvehicle::junkman(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x171737e9, result);
}
inline bool Gen::pvehicle::SET_junkman(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x171737e9, input);
}

inline bool Gen::pvehicle::junkman_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xcdc136e8);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::junkman_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xcdc136e8);
}
inline bool Gen::pvehicle::junkman_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xcdc136e8, result);
}
inline bool Gen::pvehicle::SET_junkman_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xcdc136e8, input);
}

inline bool Gen::pvehicle::nos(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xb1669f64);
}
inline const RefSpec &Gen::pvehicle::nos(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xb1669f64, index);
}
inline bool Gen::pvehicle::nos(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xb1669f64, result, index);
}
inline unsigned int Gen::pvehicle::Num_nos() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb1669f64);
}
inline bool Gen::pvehicle::SET_nos(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xb1669f64, input, index);
}

inline bool Gen::pvehicle::nos_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x8f37beae);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::nos_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x8f37beae);
}
inline bool Gen::pvehicle::nos_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x8f37beae, result);
}
inline bool Gen::pvehicle::SET_nos_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x8f37beae, input);
}

inline bool Gen::pvehicle::nos_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x9dc66e81);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::nos_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(nos_upgrades);
}
inline bool Gen::pvehicle::nos_upgrades(EA::Reflection::Int32 &result) const {
    result = nos_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_nos_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(nos_upgrades, input);
}

inline bool Gen::pvehicle::rigidbodyspecs(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x7c90bb38);
}
inline const RefSpec &Gen::pvehicle::rigidbodyspecs() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x7c90bb38);
}
inline bool Gen::pvehicle::rigidbodyspecs(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x7c90bb38, result);
}
inline bool Gen::pvehicle::SET_rigidbodyspecs(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x7c90bb38, input);
}

inline bool Gen::pvehicle::tires(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xbd38d1ca);
}
inline const RefSpec &Gen::pvehicle::tires(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xbd38d1ca, index);
}
inline bool Gen::pvehicle::tires(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xbd38d1ca, result, index);
}
inline unsigned int Gen::pvehicle::Num_tires() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbd38d1ca);
}
inline bool Gen::pvehicle::SET_tires(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xbd38d1ca, input, index);
}

inline bool Gen::pvehicle::tires_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x34e73f83);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::tires_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x34e73f83);
}
inline bool Gen::pvehicle::tires_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x34e73f83, result);
}
inline bool Gen::pvehicle::SET_tires_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x34e73f83, input);
}

inline bool Gen::pvehicle::tires_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x75b0f76b);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::tires_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(tires_upgrades);
}
inline bool Gen::pvehicle::tires_upgrades(EA::Reflection::Int32 &result) const {
    result = tires_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_tires_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(tires_upgrades, input);
}

inline bool Gen::pvehicle::transmission(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x07a7a3e5);
}
inline const RefSpec &Gen::pvehicle::transmission(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x07a7a3e5, index);
}
inline bool Gen::pvehicle::transmission(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x07a7a3e5, result, index);
}
inline unsigned int Gen::pvehicle::Num_transmission() const {
    ATTRIB_CODEGEN_GETLENGTH(0x07a7a3e5);
}
inline bool Gen::pvehicle::SET_transmission(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x07a7a3e5, input, index);
}

inline bool Gen::pvehicle::transmission_current(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x170d5554);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::transmission_current() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x170d5554);
}
inline bool Gen::pvehicle::transmission_current(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x170d5554, result);
}
inline bool Gen::pvehicle::SET_transmission_current(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x170d5554, input);
}

inline bool Gen::pvehicle::transmission_upgrades(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x860e4531);
}
inline const EA::Reflection::Int32 &Gen::pvehicle::transmission_upgrades() const {
    ATTRIB_CODEGEN_GETLAYOUT(transmission_upgrades);
}
inline bool Gen::pvehicle::transmission_upgrades(EA::Reflection::Int32 &result) const {
    result = transmission_upgrades();
    return true;
}
inline bool Gen::pvehicle::SET_transmission_upgrades(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(transmission_upgrades, input);
}

}; // namespace Attrib

#endif
