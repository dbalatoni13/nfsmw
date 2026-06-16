#ifndef _attrib_gen_world_h
#define _attrib_gen_world_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct world : Instance {
    typedef EA::Reflection::Int32 TypeOf_ACCUMULATE_SCORES;
    typedef Attrib::StringKey TypeOf_AIC_BOND_LGADGET;
    typedef Attrib::StringKey TypeOf_AIC_BOND_RGADGET;
    typedef EA::Reflection::Int32 TypeOf_ANIM_BANK_NIS_BUFFER;
    typedef EA::Reflection::Int32 TypeOf_BONDMOVE_DIE_CLEAR;
    typedef EA::Reflection::Int32 TypeOf_BONDMOVE_RESTART_CLEAR;
    typedef EA::Reflection::Int32 TypeOf_CHAIN_NEXT_MISSION;
    typedef Attrib::StringKey TypeOf_CHAIN_NEXT_MISSION_NAME;
    typedef EA::Reflection::Bool TypeOf_CHECK_PLAYER_BEHIND_TRAFFIC;
    typedef EA::Reflection::Float TypeOf_CONTROLLER_CURVE;
    typedef EA::Reflection::Int32 TypeOf_HENCH_SLOTS_AVAIL;
    typedef EA::Reflection::Bool TypeOf_LINEAR_TRACK;
    typedef EA::Reflection::UInt32 TypeOf_MAX_FRAGMENTS;
    typedef EA::Reflection::UInt32 TypeOf_MAX_NEWTONS;
    typedef EA::Reflection::UInt32 TypeOf_MAX_SMACKABLES;
    typedef EA::Reflection::Int32 TypeOf_MAX_TRAFFIC;
    typedef EA::Reflection::Float TypeOf_MAX_TRAFFIC_SPAWN_DISTANCE;
    typedef EA::Reflection::Float TypeOf_MIN_TRAFFIC_SPAWN_DISTANCE;
    typedef EA::Reflection::Int32 TypeOf_NUM_CHECKPOINTS;
    typedef EA::Reflection::Int32 TypeOf_NUM_PED_TYPES_GC;
    typedef EA::Reflection::Int32 TypeOf_NUM_PED_TYPES_PS2;
    typedef EA::Reflection::Int32 TypeOf_NUM_PED_TYPES_XBOX;
    typedef Attrib::StringKey TypeOf_PED_OBJECTS;
    typedef EA::Reflection::Float TypeOf_PED_SPAWN_RADIUS;
    typedef EA::Reflection::Bool TypeOf_RACE_SCORING;
    typedef EA::Reflection::Bool TypeOf_SPLITMISSION_CARRYDAMAGE;
    typedef Attrib::StringKey TypeOf_SPLITMISSION_NEXTHALF;
    typedef Attrib::StringKey TypeOf_SPLITMISSION_PREVHALF;
    typedef EA::Reflection::Bool TypeOf_TRAFFIC_LANE_CHANGES;
    typedef EA::Reflection::Float TypeOf_TRAFFIC_SPEED;
    typedef Attrib::StringKey TypeOf_TRAFFIC_TYPES;
    typedef EA::Reflection::Int32 TypeOf_VEHICLE_SLOTS_AVAIL;
    typedef Attrib::StringKey TypeOf_WORLD_TYPE;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("world");
    world(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    world(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    world(const Instance &src) : Instance(src) {}
    world(const world &src) : Instance(src) {}
    world(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~world() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x6d90da55;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x6d90da55, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const world &operator=(const world &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const world &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool ACCUMULATE_SCORES(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe2956904);
    }
    bool ACCUMULATE_SCORES(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xe2956904, result);
    }
    const EA::Reflection::Int32 &ACCUMULATE_SCORES() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xe2956904);
    }
    bool SET_ACCUMULATE_SCORES(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xe2956904, input);
    }
    bool AIC_BOND_LGADGET(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x73824203);
    }
    bool AIC_BOND_LGADGET(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x73824203, result);
    }
    const Attrib::StringKey &AIC_BOND_LGADGET() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x73824203);
    }
    bool SET_AIC_BOND_LGADGET(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x73824203, input);
    }
    bool AIC_BOND_RGADGET(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x1c778e02);
    }
    bool AIC_BOND_RGADGET(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x1c778e02, result);
    }
    const Attrib::StringKey &AIC_BOND_RGADGET() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x1c778e02);
    }
    bool SET_AIC_BOND_RGADGET(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x1c778e02, input);
    }
    bool ANIM_BANK_NIS_BUFFER(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdb48b25b);
    }
    bool ANIM_BANK_NIS_BUFFER(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdb48b25b, result);
    }
    const EA::Reflection::Int32 &ANIM_BANK_NIS_BUFFER() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdb48b25b);
    }
    bool SET_ANIM_BANK_NIS_BUFFER(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdb48b25b, input);
    }
    bool BONDMOVE_DIE_CLEAR(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf3542002);
    }
    bool BONDMOVE_DIE_CLEAR(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf3542002, result);
    }
    const EA::Reflection::Int32 &BONDMOVE_DIE_CLEAR() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf3542002);
    }
    bool SET_BONDMOVE_DIE_CLEAR(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf3542002, input);
    }
    bool BONDMOVE_RESTART_CLEAR(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x134550a4);
    }
    bool BONDMOVE_RESTART_CLEAR(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x134550a4, result);
    }
    const EA::Reflection::Int32 &BONDMOVE_RESTART_CLEAR() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x134550a4);
    }
    bool SET_BONDMOVE_RESTART_CLEAR(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x134550a4, input);
    }
    bool CHAIN_NEXT_MISSION(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x30f2cd5a);
    }
    bool CHAIN_NEXT_MISSION(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x30f2cd5a, result);
    }
    const EA::Reflection::Int32 &CHAIN_NEXT_MISSION() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x30f2cd5a);
    }
    bool SET_CHAIN_NEXT_MISSION(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x30f2cd5a, input);
    }
    bool CHAIN_NEXT_MISSION_NAME(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9df683fb);
    }
    bool CHAIN_NEXT_MISSION_NAME(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x9df683fb, result);
    }
    const Attrib::StringKey &CHAIN_NEXT_MISSION_NAME() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x9df683fb);
    }
    bool SET_CHAIN_NEXT_MISSION_NAME(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x9df683fb, input);
    }
    bool CHECK_PLAYER_BEHIND_TRAFFIC(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe8a7cce2);
    }
    bool CHECK_PLAYER_BEHIND_TRAFFIC(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe8a7cce2, result);
    }
    const EA::Reflection::Bool &CHECK_PLAYER_BEHIND_TRAFFIC() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe8a7cce2);
    }
    bool SET_CHECK_PLAYER_BEHIND_TRAFFIC(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe8a7cce2, input);
    }
    bool CONTROLLER_CURVE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x98b567dc);
    }
    bool CONTROLLER_CURVE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x98b567dc, result);
    }
    const EA::Reflection::Float &CONTROLLER_CURVE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x98b567dc);
    }
    bool SET_CONTROLLER_CURVE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x98b567dc, input);
    }
    bool HENCH_SLOTS_AVAIL(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xaea7d039);
    }
    bool HENCH_SLOTS_AVAIL(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xaea7d039, result);
    }
    const EA::Reflection::Int32 &HENCH_SLOTS_AVAIL() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xaea7d039);
    }
    bool SET_HENCH_SLOTS_AVAIL(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xaea7d039, input);
    }
    bool LINEAR_TRACK(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xfd47cfb6);
    }
    bool LINEAR_TRACK(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xfd47cfb6, result);
    }
    const EA::Reflection::Bool &LINEAR_TRACK() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xfd47cfb6);
    }
    bool SET_LINEAR_TRACK(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xfd47cfb6, input);
    }
    bool MAX_FRAGMENTS(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x113d4c46);
    }
    bool MAX_FRAGMENTS(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x113d4c46, result);
    }
    const EA::Reflection::UInt32 &MAX_FRAGMENTS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x113d4c46);
    }
    bool SET_MAX_FRAGMENTS(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x113d4c46, input);
    }
    bool MAX_NEWTONS(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0013821f);
    }
    bool MAX_NEWTONS(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x0013821f, result);
    }
    const EA::Reflection::UInt32 &MAX_NEWTONS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x0013821f);
    }
    bool SET_MAX_NEWTONS(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x0013821f, input);
    }
    bool MAX_SMACKABLES(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x68575d35);
    }
    bool MAX_SMACKABLES(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x68575d35, result);
    }
    const EA::Reflection::UInt32 &MAX_SMACKABLES() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x68575d35);
    }
    bool SET_MAX_SMACKABLES(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x68575d35, input);
    }
    bool MAX_TRAFFIC(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xfc01dc96);
    }
    bool MAX_TRAFFIC(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xfc01dc96, result);
    }
    const EA::Reflection::Int32 &MAX_TRAFFIC() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xfc01dc96);
    }
    bool SET_MAX_TRAFFIC(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xfc01dc96, input);
    }
    bool MAX_TRAFFIC_SPAWN_DISTANCE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3f4a4cec);
    }
    bool MAX_TRAFFIC_SPAWN_DISTANCE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3f4a4cec, result);
    }
    const EA::Reflection::Float &MAX_TRAFFIC_SPAWN_DISTANCE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3f4a4cec);
    }
    bool SET_MAX_TRAFFIC_SPAWN_DISTANCE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3f4a4cec, input);
    }
    bool MIN_TRAFFIC_SPAWN_DISTANCE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xecd3671d);
    }
    bool MIN_TRAFFIC_SPAWN_DISTANCE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xecd3671d, result);
    }
    const EA::Reflection::Float &MIN_TRAFFIC_SPAWN_DISTANCE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xecd3671d);
    }
    bool SET_MIN_TRAFFIC_SPAWN_DISTANCE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xecd3671d, input);
    }
    bool NUM_CHECKPOINTS(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x91ed18e9);
    }
    bool NUM_CHECKPOINTS(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x91ed18e9, result);
    }
    const EA::Reflection::Int32 &NUM_CHECKPOINTS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x91ed18e9);
    }
    bool SET_NUM_CHECKPOINTS(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x91ed18e9, input);
    }
    bool NUM_PED_TYPES_GC(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x3c0e521f);
    }
    bool NUM_PED_TYPES_GC(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x3c0e521f, result);
    }
    const EA::Reflection::Int32 &NUM_PED_TYPES_GC() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x3c0e521f);
    }
    bool SET_NUM_PED_TYPES_GC(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x3c0e521f, input);
    }
    bool NUM_PED_TYPES_PS2(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xfc206f2f);
    }
    bool NUM_PED_TYPES_PS2(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xfc206f2f, result);
    }
    const EA::Reflection::Int32 &NUM_PED_TYPES_PS2() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xfc206f2f);
    }
    bool SET_NUM_PED_TYPES_PS2(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xfc206f2f, input);
    }
    bool NUM_PED_TYPES_XBOX(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xac052d7a);
    }
    bool NUM_PED_TYPES_XBOX(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xac052d7a, result);
    }
    const EA::Reflection::Int32 &NUM_PED_TYPES_XBOX() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xac052d7a);
    }
    bool SET_NUM_PED_TYPES_XBOX(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xac052d7a, input);
    }
    bool PED_OBJECTS(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x968e5680);
    }
    bool PED_OBJECTS(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x968e5680, result);
    }
    const Attrib::StringKey &PED_OBJECTS() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x968e5680);
    }
    bool SET_PED_OBJECTS(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x968e5680, input);
    }
    bool PED_SPAWN_RADIUS(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb60cb556);
    }
    bool PED_SPAWN_RADIUS(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb60cb556, result);
    }
    const EA::Reflection::Float &PED_SPAWN_RADIUS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb60cb556);
    }
    bool SET_PED_SPAWN_RADIUS(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb60cb556, input);
    }
    bool RACE_SCORING(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xd52754da);
    }
    bool RACE_SCORING(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xd52754da, result);
    }
    const EA::Reflection::Bool &RACE_SCORING() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xd52754da);
    }
    bool SET_RACE_SCORING(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xd52754da, input);
    }
    bool SPLITMISSION_CARRYDAMAGE(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe58865d1);
    }
    bool SPLITMISSION_CARRYDAMAGE(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe58865d1, result);
    }
    const EA::Reflection::Bool &SPLITMISSION_CARRYDAMAGE() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe58865d1);
    }
    bool SET_SPLITMISSION_CARRYDAMAGE(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe58865d1, input);
    }
    bool SPLITMISSION_NEXTHALF(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xe05dec39);
    }
    bool SPLITMISSION_NEXTHALF(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xe05dec39, result);
    }
    const Attrib::StringKey &SPLITMISSION_NEXTHALF() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xe05dec39);
    }
    bool SET_SPLITMISSION_NEXTHALF(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xe05dec39, input);
    }
    bool SPLITMISSION_PREVHALF(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x375aed88);
    }
    bool SPLITMISSION_PREVHALF(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x375aed88, result);
    }
    const Attrib::StringKey &SPLITMISSION_PREVHALF() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x375aed88);
    }
    bool SET_SPLITMISSION_PREVHALF(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x375aed88, input);
    }
    bool TRAFFIC_LANE_CHANGES(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4463a62d);
    }
    bool TRAFFIC_LANE_CHANGES(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4463a62d, result);
    }
    const EA::Reflection::Bool &TRAFFIC_LANE_CHANGES() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4463a62d);
    }
    bool SET_TRAFFIC_LANE_CHANGES(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4463a62d, input);
    }
    bool TRAFFIC_SPEED(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x811c6606);
    }
    bool TRAFFIC_SPEED(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x811c6606, result);
    }
    const EA::Reflection::Float &TRAFFIC_SPEED() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x811c6606);
    }
    bool SET_TRAFFIC_SPEED(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x811c6606, input);
    }
    bool TRAFFIC_TYPES(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xb7606a9a);
    }
    bool TRAFFIC_TYPES(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xb7606a9a, result, index);
    }
    const Attrib::StringKey &TRAFFIC_TYPES(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xb7606a9a, index);
    }
    unsigned int Num_TRAFFIC_TYPES() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb7606a9a);
    }
    bool SET_TRAFFIC_TYPES(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xb7606a9a, input, index);
    }
    bool VEHICLE_SLOTS_AVAIL(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x80a91138);
    }
    bool VEHICLE_SLOTS_AVAIL(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x80a91138, result);
    }
    const EA::Reflection::Int32 &VEHICLE_SLOTS_AVAIL() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x80a91138);
    }
    bool SET_VEHICLE_SLOTS_AVAIL(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x80a91138, input);
    }
    bool WORLD_TYPE(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x84da8ef1);
    }
    bool WORLD_TYPE(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x84da8ef1, result);
    }
    const Attrib::StringKey &WORLD_TYPE() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x84da8ef1);
    }
    bool SET_WORLD_TYPE(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x84da8ef1, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    world &ConvertFromInstance(Instance &src) {}
    const world &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key world = 0x6d90da55;

}; // namespace ClassName

namespace Hash {
namespace world {

static const Key ACCUMULATE_SCORES = 0xe2956904;
static const Key AIC_BOND_LGADGET = 0x73824203;
static const Key AIC_BOND_RGADGET = 0x1c778e02;
static const Key ANIM_BANK_NIS_BUFFER = 0xdb48b25b;
static const Key BONDMOVE_DIE_CLEAR = 0xf3542002;
static const Key BONDMOVE_RESTART_CLEAR = 0x134550a4;
static const Key CHAIN_NEXT_MISSION = 0x30f2cd5a;
static const Key CHAIN_NEXT_MISSION_NAME = 0x9df683fb;
static const Key CHECK_PLAYER_BEHIND_TRAFFIC = 0xe8a7cce2;
static const Key CONTROLLER_CURVE = 0x98b567dc;
static const Key HENCH_SLOTS_AVAIL = 0xaea7d039;
static const Key LINEAR_TRACK = 0xfd47cfb6;
static const Key MAX_FRAGMENTS = 0x113d4c46;
static const Key MAX_NEWTONS = 0x0013821f;
static const Key MAX_SMACKABLES = 0x68575d35;
static const Key MAX_TRAFFIC = 0xfc01dc96;
static const Key MAX_TRAFFIC_SPAWN_DISTANCE = 0x3f4a4cec;
static const Key MIN_TRAFFIC_SPAWN_DISTANCE = 0xecd3671d;
static const Key NUM_CHECKPOINTS = 0x91ed18e9;
static const Key NUM_PED_TYPES_GC = 0x3c0e521f;
static const Key NUM_PED_TYPES_PS2 = 0xfc206f2f;
static const Key NUM_PED_TYPES_XBOX = 0xac052d7a;
static const Key PED_OBJECTS = 0x968e5680;
static const Key PED_SPAWN_RADIUS = 0xb60cb556;
static const Key RACE_SCORING = 0xd52754da;
static const Key SPLITMISSION_CARRYDAMAGE = 0xe58865d1;
static const Key SPLITMISSION_NEXTHALF = 0xe05dec39;
static const Key SPLITMISSION_PREVHALF = 0x375aed88;
static const Key TRAFFIC_LANE_CHANGES = 0x4463a62d;
static const Key TRAFFIC_SPEED = 0x811c6606;
static const Key TRAFFIC_TYPES = 0xb7606a9a;
static const Key VEHICLE_SLOTS_AVAIL = 0x80a91138;
static const Key WORLD_TYPE = 0x84da8ef1;

}; // namespace world
}; // namespace Hash

inline Key Gen::world::ClassKey() {
    return ClassName::world;
}

}; // namespace Attrib

#endif
