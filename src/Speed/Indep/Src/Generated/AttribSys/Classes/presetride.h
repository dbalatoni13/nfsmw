#ifndef _attrib_gen_presetride_h
#define _attrib_gen_presetride_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct presetride : Instance {
    typedef EA::Reflection::Int32 TypeOf_brakes;
    typedef EA::Reflection::Int32 TypeOf_chassis;
    typedef EA::Reflection::Int32 TypeOf_engine;
    typedef EA::Reflection::Int32 TypeOf_induction;
    typedef EA::Reflection::Int32 TypeOf_nos;
    typedef EA::Reflection::Int32 TypeOf_tires;
    typedef EA::Reflection::Int32 TypeOf_transmission;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("presetride");
    presetride(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    presetride(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    presetride(const Instance &src) : Instance(src) {}
    presetride(const presetride &src) : Instance(src) {}
    presetride(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~presetride() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x27e73952;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x27e73952, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const presetride &operator=(const presetride &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const presetride &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool brakes(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x36350867);
    }
    bool brakes(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x36350867, result);
    }
    const EA::Reflection::Int32 &brakes() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x36350867);
    }
    bool SET_brakes(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x36350867, input);
    }
    bool chassis(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xafa210f0);
    }
    bool chassis(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xafa210f0, result);
    }
    const EA::Reflection::Int32 &chassis() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xafa210f0);
    }
    bool SET_chassis(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xafa210f0, input);
    }
    bool engine(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf1f5fbc7);
    }
    bool engine(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf1f5fbc7, result);
    }
    const EA::Reflection::Int32 &engine() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf1f5fbc7);
    }
    bool SET_engine(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf1f5fbc7, input);
    }
    bool induction(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc92a0142);
    }
    bool induction(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc92a0142, result);
    }
    const EA::Reflection::Int32 &induction() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc92a0142);
    }
    bool SET_induction(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc92a0142, input);
    }
    bool nos(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xb1669f64);
    }
    bool nos(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xb1669f64, result);
    }
    const EA::Reflection::Int32 &nos() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xb1669f64);
    }
    bool SET_nos(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xb1669f64, input);
    }
    bool tires(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xbd38d1ca);
    }
    bool tires(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xbd38d1ca, result);
    }
    const EA::Reflection::Int32 &tires() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xbd38d1ca);
    }
    bool SET_tires(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xbd38d1ca, input);
    }
    bool transmission(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x07a7a3e5);
    }
    bool transmission(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x07a7a3e5, result);
    }
    const EA::Reflection::Int32 &transmission() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x07a7a3e5);
    }
    bool SET_transmission(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x07a7a3e5, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    presetride &ConvertFromInstance(Instance &src) {}
    const presetride &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key presetride = 0x27e73952;

}; // namespace ClassName

namespace Hash {
namespace presetride {

static const Key brakes = 0x36350867;
static const Key chassis = 0xafa210f0;
static const Key engine = 0xf1f5fbc7;
static const Key induction = 0xc92a0142;
static const Key nos = 0xb1669f64;
static const Key tires = 0xbd38d1ca;
static const Key transmission = 0x07a7a3e5;

}; // namespace presetride
}; // namespace Hash

inline Key Gen::presetride::ClassKey() {
    return ClassName::presetride;
}

}; // namespace Attrib

#endif
