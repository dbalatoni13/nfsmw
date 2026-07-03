#ifndef _attrib_gen_nos_h
#define _attrib_gen_nos_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct nos : Instance {
    struct _LayoutStruct {
        EA::Reflection::Float NOS_DISENGAGE;      // offset 0x0, size 0x4
        EA::Reflection::Float TORQUE_BOOST;       // offset 0x4, size 0x4
        EA::Reflection::Float FLOW_RATE;          // offset 0x8, size 0x4
        EA::Reflection::Float RECHARGE_MIN;       // offset 0xc, size 0x4
        EA::Reflection::Float NOS_CAPACITY;       // offset 0x10, size 0x4
        EA::Reflection::Float RECHARGE_MAX;       // offset 0x14, size 0x4
        EA::Reflection::Float RECHARGE_MAX_SPEED; // offset 0x18, size 0x4
        EA::Reflection::Float RECHARGE_MIN_SPEED; // offset 0x1c, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_FLOW_RATE;
    typedef EA::Reflection::Float TypeOf_NOS_CAPACITY;
    typedef EA::Reflection::Float TypeOf_NOS_DISENGAGE;
    typedef EA::Reflection::Float TypeOf_RECHARGE_MAX;
    typedef EA::Reflection::Float TypeOf_RECHARGE_MAX_SPEED;
    typedef EA::Reflection::Float TypeOf_RECHARGE_MIN;
    typedef EA::Reflection::Float TypeOf_RECHARGE_MIN_SPEED;
    typedef EA::Reflection::Float TypeOf_TORQUE_BOOST;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("nos");
    nos(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    nos(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    nos(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    nos(const nos &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    nos(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~nos() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xb1669f64;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xb1669f64, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const nos &operator=(const nos &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const nos &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool FLOW_RATE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3f3b1976);
    }
    bool FLOW_RATE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FLOW_RATE, result);
    }
    const EA::Reflection::Float &FLOW_RATE() const {
        ATTRIB_CODEGEN_GETLAYOUT(FLOW_RATE);
    }
    bool SET_FLOW_RATE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FLOW_RATE, input);
    }
    bool NOS_CAPACITY(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xaa032196);
    }
    bool NOS_CAPACITY(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NOS_CAPACITY, result);
    }
    const EA::Reflection::Float &NOS_CAPACITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(NOS_CAPACITY);
    }
    bool SET_NOS_CAPACITY(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NOS_CAPACITY, input);
    }
    bool NOS_DISENGAGE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xac733de3);
    }
    bool NOS_DISENGAGE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NOS_DISENGAGE, result);
    }
    const EA::Reflection::Float &NOS_DISENGAGE() const {
        ATTRIB_CODEGEN_GETLAYOUT(NOS_DISENGAGE);
    }
    bool SET_NOS_DISENGAGE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NOS_DISENGAGE, input);
    }
    bool RECHARGE_MAX(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x12a14452);
    }
    bool RECHARGE_MAX(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RECHARGE_MAX, result);
    }
    const EA::Reflection::Float &RECHARGE_MAX() const {
        ATTRIB_CODEGEN_GETLAYOUT(RECHARGE_MAX);
    }
    bool SET_RECHARGE_MAX(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RECHARGE_MAX, input);
    }
    bool RECHARGE_MAX_SPEED(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5799043b);
    }
    bool RECHARGE_MAX_SPEED(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RECHARGE_MAX_SPEED, result);
    }
    const EA::Reflection::Float &RECHARGE_MAX_SPEED() const {
        ATTRIB_CODEGEN_GETLAYOUT(RECHARGE_MAX_SPEED);
    }
    bool SET_RECHARGE_MAX_SPEED(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RECHARGE_MAX_SPEED, input);
    }
    bool RECHARGE_MIN(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0a03c0eb);
    }
    bool RECHARGE_MIN(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RECHARGE_MIN, result);
    }
    const EA::Reflection::Float &RECHARGE_MIN() const {
        ATTRIB_CODEGEN_GETLAYOUT(RECHARGE_MIN);
    }
    bool SET_RECHARGE_MIN(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RECHARGE_MIN, input);
    }
    bool RECHARGE_MIN_SPEED(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x97a0d2df);
    }
    bool RECHARGE_MIN_SPEED(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RECHARGE_MIN_SPEED, result);
    }
    const EA::Reflection::Float &RECHARGE_MIN_SPEED() const {
        ATTRIB_CODEGEN_GETLAYOUT(RECHARGE_MIN_SPEED);
    }
    bool SET_RECHARGE_MIN_SPEED(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RECHARGE_MIN_SPEED, input);
    }
    bool TORQUE_BOOST(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8de27c3c);
    }
    bool TORQUE_BOOST(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TORQUE_BOOST, result);
    }
    const EA::Reflection::Float &TORQUE_BOOST() const {
        ATTRIB_CODEGEN_GETLAYOUT(TORQUE_BOOST);
    }
    bool SET_TORQUE_BOOST(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TORQUE_BOOST, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    nos &ConvertFromInstance(Instance &src) {}
    const nos &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key nos = 0xb1669f64;

}; // namespace ClassName

namespace Hash {
namespace nos {

static const Key FLOW_RATE = 0x3f3b1976;
static const Key NOS_CAPACITY = 0xaa032196;
static const Key NOS_DISENGAGE = 0xac733de3;
static const Key RECHARGE_MAX = 0x12a14452;
static const Key RECHARGE_MAX_SPEED = 0x5799043b;
static const Key RECHARGE_MIN = 0x0a03c0eb;
static const Key RECHARGE_MIN_SPEED = 0x97a0d2df;
static const Key TORQUE_BOOST = 0x8de27c3c;

}; // namespace nos
}; // namespace Hash

inline Key Gen::nos::ClassKey() {
    return ClassName::nos;
}

}; // namespace Attrib

#endif
