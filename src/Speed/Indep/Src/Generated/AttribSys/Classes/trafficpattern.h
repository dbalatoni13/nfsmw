#ifndef _attrib_gen_trafficpattern_h
#define _attrib_gen_trafficpattern_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct trafficpattern : Instance {
    struct _LayoutStruct {
        EA::Reflection::Text CollectionName; // offset 0x0, size 0x4
    };

    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Float TypeOf_SpawnTime;
    typedef EA::Reflection::Float TypeOf_SpeedHighway;
    typedef EA::Reflection::Float TypeOf_SpeedStreet;
    typedef TrafficPatternRecord TypeOf_Vehicles;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("trafficpattern");
    trafficpattern(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    trafficpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    trafficpattern(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    trafficpattern(const trafficpattern &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    trafficpattern(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~trafficpattern() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x20d08342;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x20d08342, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const trafficpattern &operator=(const trafficpattern &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const trafficpattern &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
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
    bool SpawnTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbf2fdb5c);
    }
    bool SpawnTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xbf2fdb5c, result);
    }
    const EA::Reflection::Float &SpawnTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xbf2fdb5c);
    }
    bool SET_SpawnTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xbf2fdb5c, input);
    }
    bool SpeedHighway(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9e404e33);
    }
    bool SpeedHighway(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9e404e33, result);
    }
    const EA::Reflection::Float &SpeedHighway() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9e404e33);
    }
    bool SET_SpeedHighway(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9e404e33, input);
    }
    bool SpeedStreet(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7c44962f);
    }
    bool SpeedStreet(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7c44962f, result);
    }
    const EA::Reflection::Float &SpeedStreet() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7c44962f);
    }
    bool SET_SpeedStreet(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7c44962f, input);
    }
    bool Vehicles(TAttrib<TrafficPatternRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(TrafficPatternRecord, 0x94e3c795);
    }
    bool Vehicles(TrafficPatternRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(TrafficPatternRecord, 0x94e3c795, result, index);
    }
    const TrafficPatternRecord &Vehicles(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(TrafficPatternRecord, 0x94e3c795, index);
    }
    unsigned int Num_Vehicles() const {
        ATTRIB_CODEGEN_GETLENGTH(0x94e3c795);
    }
    bool SET_Vehicles(const TrafficPatternRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(TrafficPatternRecord, 0x94e3c795, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    trafficpattern &ConvertFromInstance(Instance &src) {}
    const trafficpattern &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key trafficpattern = 0x20d08342;

}; // namespace ClassName

namespace Hash {
namespace trafficpattern {

static const Key CollectionName = 0x9ca1c8f9;
static const Key SpawnTime = 0xbf2fdb5c;
static const Key SpeedHighway = 0x9e404e33;
static const Key SpeedStreet = 0x7c44962f;
static const Key Vehicles = 0x94e3c795;

}; // namespace trafficpattern
}; // namespace Hash

inline Key Gen::trafficpattern::ClassKey() {
    return ClassName::trafficpattern;
}

}; // namespace Attrib

#endif
