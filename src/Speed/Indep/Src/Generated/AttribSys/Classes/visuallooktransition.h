#ifndef _attrib_gen_visuallooktransition_h
#define _attrib_gen_visuallooktransition_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct visuallooktransition : Instance {
    typedef EA::Reflection::Float TypeOf__testvalue;
    typedef UMath::Matrix4 TypeOf_normal;
    typedef UMath::Matrix4 TypeOf_uves;
    typedef UMath::Matrix4 TypeOf_uves_extreme;
    typedef UMath::Matrix4 TypeOf_uves_pulse;
    typedef EA::Reflection::Float TypeOf_uves_pulse_length;
    typedef EA::Reflection::Float TypeOf_uves_pulse_trigger;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("visuallooktransition");
    visuallooktransition(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    visuallooktransition(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    visuallooktransition(const Instance &src) : Instance(src) {}
    visuallooktransition(const visuallooktransition &src) : Instance(src) {}
    visuallooktransition(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~visuallooktransition() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x0f409aa6;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x0f409aa6, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const visuallooktransition &operator=(const visuallooktransition &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const visuallooktransition &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool _testvalue(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4ef07d5d);
    }
    bool _testvalue(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4ef07d5d, result);
    }
    const EA::Reflection::Float &_testvalue() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4ef07d5d);
    }
    bool SET__testvalue(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4ef07d5d, input);
    }
    bool normal(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x0eb3d357);
    }
    bool normal(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x0eb3d357, result);
    }
    const UMath::Matrix4 &normal() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x0eb3d357);
    }
    bool SET_normal(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x0eb3d357, input);
    }
    bool uves(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x681bef75);
    }
    bool uves(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x681bef75, result);
    }
    const UMath::Matrix4 &uves() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x681bef75);
    }
    bool SET_uves(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x681bef75, input);
    }
    bool uves_extreme(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x0e629621);
    }
    bool uves_extreme(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x0e629621, result);
    }
    const UMath::Matrix4 &uves_extreme() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x0e629621);
    }
    bool SET_uves_extreme(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x0e629621, input);
    }
    bool uves_pulse(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x334f1e4d);
    }
    bool uves_pulse(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x334f1e4d, result);
    }
    const UMath::Matrix4 &uves_pulse() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x334f1e4d);
    }
    bool SET_uves_pulse(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x334f1e4d, input);
    }
    bool uves_pulse_length(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0b82d479);
    }
    bool uves_pulse_length(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0b82d479, result);
    }
    const EA::Reflection::Float &uves_pulse_length() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0b82d479);
    }
    bool SET_uves_pulse_length(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0b82d479, input);
    }
    bool uves_pulse_trigger(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x53769b6c);
    }
    bool uves_pulse_trigger(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x53769b6c, result);
    }
    const EA::Reflection::Float &uves_pulse_trigger() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x53769b6c);
    }
    bool SET_uves_pulse_trigger(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x53769b6c, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    visuallooktransition &ConvertFromInstance(Instance &src) {}
    const visuallooktransition &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key visuallooktransition = 0x0f409aa6;

}; // namespace ClassName

namespace Hash {
namespace visuallooktransition {

static const Key _testvalue = 0x4ef07d5d;
static const Key normal = 0x0eb3d357;
static const Key uves = 0x681bef75;
static const Key uves_extreme = 0x0e629621;
static const Key uves_pulse = 0x334f1e4d;
static const Key uves_pulse_length = 0x0b82d479;
static const Key uves_pulse_trigger = 0x53769b6c;

}; // namespace visuallooktransition
}; // namespace Hash

inline Key Gen::visuallooktransition::ClassKey() {
    return ClassName::visuallooktransition;
}

}; // namespace Attrib

#endif
