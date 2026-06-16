#ifndef _attrib_gen_speech_h
#define _attrib_gen_speech_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct speech : Instance {
    struct _LayoutStruct {
        EA::Reflection::Text CollectionName; // offset 0x0, size 0x4
        SPCHType_1_EventID SpeechID;         // offset 0x4, size 0x4
        EA::Reflection::Float BackTime;      // offset 0x8, size 0x4
        EA::Reflection::Float expiry;        // offset 0xc, size 0x4
        EA::Reflection::Float CullingRange;  // offset 0x10, size 0x4
        EA::Reflection::Float Interval;      // offset 0x14, size 0x4
        EA::Reflection::Int16 priority;      // offset 0x18, size 0x2
        EA::Reflection::Bool DoNotDropout;   // offset 0x1a, size 0x1
    };

    typedef EA::Reflection::Float TypeOf_BackTime;
    typedef EA::Reflection::UInt8 TypeOf_Clarity;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Float TypeOf_CullingRange;
    typedef EA::Reflection::Float TypeOf_DeadAir;
    typedef RefSpec TypeOf_DepFollow;
    typedef EA::Reflection::Bool TypeOf_DoNotDropout;
    typedef EA::Reflection::Float TypeOf_EnforceDeadAir;
    typedef EA::Reflection::Float TypeOf_InitDelay;
    typedef EA::Reflection::Bool TypeOf_Interruptable;
    typedef EA::Reflection::Float TypeOf_Interval;
    typedef EA::Reflection::Int8 TypeOf_MaxHeat;
    typedef EA::Reflection::Int32 TypeOf_MaxPlayback;
    typedef EA::Reflection::Float TypeOf_MaxPlayerSpeed;
    typedef EA::Reflection::Int8 TypeOf_MinHeat;
    typedef EA::Reflection::Float TypeOf_MinPlayerSpeed;
    typedef EA::Reflection::Bool TypeOf_OnScreenOnly;
    typedef EA::Reflection::Bool TypeOf_Pan;
    typedef EA::Reflection::Bool TypeOf_RadioChirp;
    typedef RefSpec TypeOf_RecallList;
    typedef EA::Reflection::Bool TypeOf_RedundancyCheckIsGlobal;
    typedef SPCHType_1_EventID TypeOf_SpeechID;
    typedef EA::Reflection::Bool TypeOf_cache_OnCreate;
    typedef EA::Reflection::Bool TypeOf_cache_SysInit;
    typedef EA::Reflection::Float TypeOf_expiry;
    typedef EA::Reflection::Bool TypeOf_interrupt;
    typedef EA::Reflection::Int16 TypeOf_priority;
    typedef EA::Reflection::Bool TypeOf_reqLOS;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("speech");
    speech(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speech(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speech(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speech(const speech &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speech(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~speech() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xc593dd47;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xc593dd47, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const speech &operator=(const speech &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const speech &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BackTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8e319c7c);
    }
    bool BackTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BackTime, result);
    }
    const EA::Reflection::Float &BackTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(BackTime);
    }
    bool SET_BackTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BackTime, input);
    }
    bool Clarity(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xfe3254ea);
    }
    bool Clarity(EA::Reflection::UInt8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt8, 0xfe3254ea, result);
    }
    const EA::Reflection::UInt8 &Clarity() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt8, 0xfe3254ea);
    }
    bool SET_Clarity(const EA::Reflection::UInt8 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt8, 0xfe3254ea, input);
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
    bool CullingRange(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbbfc6532);
    }
    bool CullingRange(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CullingRange, result);
    }
    const EA::Reflection::Float &CullingRange() const {
        ATTRIB_CODEGEN_GETLAYOUT(CullingRange);
    }
    bool SET_CullingRange(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CullingRange, input);
    }
    bool DeadAir(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x793293dd);
    }
    bool DeadAir(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x793293dd, result);
    }
    const EA::Reflection::Float &DeadAir() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x793293dd);
    }
    bool SET_DeadAir(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x793293dd, input);
    }
    bool DepFollow(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xc8c5d475);
    }
    bool DepFollow(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xc8c5d475, result, index);
    }
    const RefSpec &DepFollow(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xc8c5d475, index);
    }
    unsigned int Num_DepFollow() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc8c5d475);
    }
    bool SET_DepFollow(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xc8c5d475, input, index);
    }
    bool DoNotDropout(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xa684e35a);
    }
    bool DoNotDropout(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DoNotDropout, result);
    }
    const EA::Reflection::Bool &DoNotDropout() const {
        ATTRIB_CODEGEN_GETLAYOUT(DoNotDropout);
    }
    bool SET_DoNotDropout(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DoNotDropout, input);
    }
    bool EnforceDeadAir(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdb3bd2f3);
    }
    bool EnforceDeadAir(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xdb3bd2f3, result);
    }
    const EA::Reflection::Float &EnforceDeadAir() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xdb3bd2f3);
    }
    bool SET_EnforceDeadAir(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xdb3bd2f3, input);
    }
    bool InitDelay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xffbdad32);
    }
    bool InitDelay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xffbdad32, result);
    }
    const EA::Reflection::Float &InitDelay() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xffbdad32);
    }
    bool SET_InitDelay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xffbdad32, input);
    }
    bool Interruptable(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x87930ed2);
    }
    bool Interruptable(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x87930ed2, result);
    }
    const EA::Reflection::Bool &Interruptable() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x87930ed2);
    }
    bool SET_Interruptable(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x87930ed2, input);
    }
    bool Interval(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x13593bf1);
    }
    bool Interval(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Interval, result);
    }
    const EA::Reflection::Float &Interval() const {
        ATTRIB_CODEGEN_GETLAYOUT(Interval);
    }
    bool SET_Interval(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Interval, input);
    }
    bool MaxHeat(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x77f20778);
    }
    bool MaxHeat(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int8, 0x77f20778, result);
    }
    const EA::Reflection::Int8 &MaxHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int8, 0x77f20778);
    }
    bool SET_MaxHeat(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int8, 0x77f20778, input);
    }
    bool MaxPlayback(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7e18c2f8);
    }
    bool MaxPlayback(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x7e18c2f8, result);
    }
    const EA::Reflection::Int32 &MaxPlayback() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x7e18c2f8);
    }
    bool SET_MaxPlayback(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x7e18c2f8, input);
    }
    bool MaxPlayerSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb9befde0);
    }
    bool MaxPlayerSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb9befde0, result);
    }
    const EA::Reflection::Float &MaxPlayerSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb9befde0);
    }
    bool SET_MaxPlayerSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb9befde0, input);
    }
    bool MinHeat(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x69603485);
    }
    bool MinHeat(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int8, 0x69603485, result);
    }
    const EA::Reflection::Int8 &MinHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int8, 0x69603485);
    }
    bool SET_MinHeat(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int8, 0x69603485, input);
    }
    bool MinPlayerSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0994e624);
    }
    bool MinPlayerSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0994e624, result);
    }
    const EA::Reflection::Float &MinPlayerSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0994e624);
    }
    bool SET_MinPlayerSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0994e624, input);
    }
    bool OnScreenOnly(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4b331604);
    }
    bool OnScreenOnly(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4b331604, result);
    }
    const EA::Reflection::Bool &OnScreenOnly() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4b331604);
    }
    bool SET_OnScreenOnly(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4b331604, input);
    }
    bool Pan(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4d663bfc);
    }
    bool Pan(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4d663bfc, result);
    }
    const EA::Reflection::Bool &Pan() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4d663bfc);
    }
    bool SET_Pan(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4d663bfc, input);
    }
    bool RadioChirp(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2976b959);
    }
    bool RadioChirp(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2976b959, result);
    }
    const EA::Reflection::Bool &RadioChirp() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2976b959);
    }
    bool SET_RadioChirp(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2976b959, input);
    }
    bool RecallList(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4710d6d5);
    }
    bool RecallList(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x4710d6d5, result, index);
    }
    const RefSpec &RecallList(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x4710d6d5, index);
    }
    unsigned int Num_RecallList() const {
        ATTRIB_CODEGEN_GETLENGTH(0x4710d6d5);
    }
    bool SET_RecallList(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x4710d6d5, input, index);
    }
    bool RedundancyCheckIsGlobal(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3db0e0e9);
    }
    bool RedundancyCheckIsGlobal(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3db0e0e9, result);
    }
    const EA::Reflection::Bool &RedundancyCheckIsGlobal() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3db0e0e9);
    }
    bool SET_RedundancyCheckIsGlobal(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3db0e0e9, input);
    }
    bool SpeechID(TAttrib<SPCHType_1_EventID> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(SPCHType_1_EventID, 0xb059f232);
    }
    bool SpeechID(SPCHType_1_EventID &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpeechID, result);
    }
    const SPCHType_1_EventID &SpeechID() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpeechID);
    }
    bool SET_SpeechID(const SPCHType_1_EventID &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpeechID, input);
    }
    bool cache_OnCreate(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb6f42d73);
    }
    bool cache_OnCreate(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb6f42d73, result);
    }
    const EA::Reflection::Bool &cache_OnCreate() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb6f42d73);
    }
    bool SET_cache_OnCreate(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb6f42d73, input);
    }
    bool cache_SysInit(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x8b5d5c9b);
    }
    bool cache_SysInit(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x8b5d5c9b, result);
    }
    const EA::Reflection::Bool &cache_SysInit() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x8b5d5c9b);
    }
    bool SET_cache_SysInit(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x8b5d5c9b, input);
    }
    bool expiry(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4d8a523d);
    }
    bool expiry(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(expiry, result);
    }
    const EA::Reflection::Float &expiry() const {
        ATTRIB_CODEGEN_GETLAYOUT(expiry);
    }
    bool SET_expiry(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(expiry, input);
    }
    bool interrupt(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xaecbb4ad);
    }
    bool interrupt(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xaecbb4ad, result);
    }
    const EA::Reflection::Bool &interrupt() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xaecbb4ad);
    }
    bool SET_interrupt(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xaecbb4ad, input);
    }
    bool priority(TAttrib<EA::Reflection::Int16> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int16, 0xa96fce56);
    }
    bool priority(EA::Reflection::Int16 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(priority, result);
    }
    const EA::Reflection::Int16 &priority() const {
        ATTRIB_CODEGEN_GETLAYOUT(priority);
    }
    bool SET_priority(const EA::Reflection::Int16 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(priority, input);
    }
    bool reqLOS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe0241fc1);
    }
    bool reqLOS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe0241fc1, result);
    }
    const EA::Reflection::Bool &reqLOS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe0241fc1);
    }
    bool SET_reqLOS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe0241fc1, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    speech &ConvertFromInstance(Instance &src) {}
    const speech &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key speech = 0xc593dd47;

}; // namespace ClassName

namespace Hash {
namespace speech {

static const Key BackTime = 0x8e319c7c;
static const Key Clarity = 0xfe3254ea;
static const Key CollectionName = 0x9ca1c8f9;
static const Key CullingRange = 0xbbfc6532;
static const Key DeadAir = 0x793293dd;
static const Key DepFollow = 0xc8c5d475;
static const Key DoNotDropout = 0xa684e35a;
static const Key EnforceDeadAir = 0xdb3bd2f3;
static const Key InitDelay = 0xffbdad32;
static const Key Interruptable = 0x87930ed2;
static const Key Interval = 0x13593bf1;
static const Key MaxHeat = 0x77f20778;
static const Key MaxPlayback = 0x7e18c2f8;
static const Key MaxPlayerSpeed = 0xb9befde0;
static const Key MinHeat = 0x69603485;
static const Key MinPlayerSpeed = 0x0994e624;
static const Key OnScreenOnly = 0x4b331604;
static const Key Pan = 0x4d663bfc;
static const Key RadioChirp = 0x2976b959;
static const Key RecallList = 0x4710d6d5;
static const Key RedundancyCheckIsGlobal = 0x3db0e0e9;
static const Key SpeechID = 0xb059f232;
static const Key cache_OnCreate = 0xb6f42d73;
static const Key cache_SysInit = 0x8b5d5c9b;
static const Key expiry = 0x4d8a523d;
static const Key interrupt = 0xaecbb4ad;
static const Key priority = 0xa96fce56;
static const Key reqLOS = 0xe0241fc1;

}; // namespace speech
}; // namespace Hash

inline Key Gen::speech::ClassKey() {
    return ClassName::speech;
}

}; // namespace Attrib

#endif
