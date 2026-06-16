#ifndef _attrib_gen_explosion_h
#define _attrib_gen_explosion_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct explosion : Instance {
    struct _LayoutStruct {
        RefSpec BaseMaterial;                      // offset 0x0, size 0xc
        EA::Reflection::Float triggerThreshold;    // offset 0xc, size 0x4
        EA::Reflection::Float explosionForceLimit; // offset 0x10, size 0x4
        EA::Reflection::Float fallOffUnit;         // offset 0x14, size 0x4
        EA::Reflection::Float forceMultiplier;     // offset 0x18, size 0x4
        EA::Reflection::Float damageMultiplier;    // offset 0x1c, size 0x4
    };

    typedef RefSpec TypeOf_BaseMaterial;
    typedef EA::Reflection::Float TypeOf_damageMultiplier;
    typedef EA::Reflection::Float TypeOf_explosionForceLimit;
    typedef EA::Reflection::Float TypeOf_fallOffUnit;
    typedef EA::Reflection::Float TypeOf_forceMultiplier;
    typedef EA::Reflection::Float TypeOf_triggerThreshold;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("explosion");
    explosion(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    explosion(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    explosion(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    explosion(const explosion &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    explosion(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~explosion() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x6434f1fb;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x6434f1fb, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const explosion &operator=(const explosion &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const explosion &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BaseMaterial(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x5497a394);
    }
    bool BaseMaterial(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BaseMaterial, result);
    }
    const RefSpec &BaseMaterial() const {
        ATTRIB_CODEGEN_GETLAYOUT(BaseMaterial);
    }
    bool SET_BaseMaterial(const RefSpec &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BaseMaterial, input);
    }
    bool damageMultiplier(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6f789cb);
    }
    bool damageMultiplier(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(damageMultiplier, result);
    }
    const EA::Reflection::Float &damageMultiplier() const {
        ATTRIB_CODEGEN_GETLAYOUT(damageMultiplier);
    }
    bool SET_damageMultiplier(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(damageMultiplier, input);
    }
    bool explosionForceLimit(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x259021b6);
    }
    bool explosionForceLimit(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(explosionForceLimit, result);
    }
    const EA::Reflection::Float &explosionForceLimit() const {
        ATTRIB_CODEGEN_GETLAYOUT(explosionForceLimit);
    }
    bool SET_explosionForceLimit(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(explosionForceLimit, input);
    }
    bool fallOffUnit(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x297cba80);
    }
    bool fallOffUnit(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(fallOffUnit, result);
    }
    const EA::Reflection::Float &fallOffUnit() const {
        ATTRIB_CODEGEN_GETLAYOUT(fallOffUnit);
    }
    bool SET_fallOffUnit(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(fallOffUnit, input);
    }
    bool forceMultiplier(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3a5970f4);
    }
    bool forceMultiplier(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(forceMultiplier, result);
    }
    const EA::Reflection::Float &forceMultiplier() const {
        ATTRIB_CODEGEN_GETLAYOUT(forceMultiplier);
    }
    bool SET_forceMultiplier(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(forceMultiplier, input);
    }
    bool triggerThreshold(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6f45864);
    }
    bool triggerThreshold(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(triggerThreshold, result);
    }
    const EA::Reflection::Float &triggerThreshold() const {
        ATTRIB_CODEGEN_GETLAYOUT(triggerThreshold);
    }
    bool SET_triggerThreshold(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(triggerThreshold, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    explosion &ConvertFromInstance(Instance &src) {}
    const explosion &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key explosion = 0x6434f1fb;

}; // namespace ClassName

namespace Hash {
namespace explosion {

static const Key BaseMaterial = 0x5497a394;
static const Key damageMultiplier = 0xa6f789cb;
static const Key explosionForceLimit = 0x259021b6;
static const Key fallOffUnit = 0x297cba80;
static const Key forceMultiplier = 0x3a5970f4;
static const Key triggerThreshold = 0xa6f45864;

}; // namespace explosion
}; // namespace Hash

inline Key Gen::explosion::ClassKey() {
    return ClassName::explosion;
}

}; // namespace Attrib

#endif
