#ifndef _attrib_gen_effects_h
#define _attrib_gen_effects_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct effects : Instance {
    struct _LayoutStruct {
        RefSpec AudioFX_DEFAULT;             // offset 0x0, size 0xc
        EA::Reflection::Text CollectionName; // offset 0xc, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_AudioCullDist;
    typedef RefSpec TypeOf_AudioFX_BOTTOM;
    typedef RefSpec TypeOf_AudioFX_DEFAULT;
    typedef RefSpec TypeOf_AudioFX_FRONT;
    typedef RefSpec TypeOf_AudioFX_REAR;
    typedef RefSpec TypeOf_AudioFX_SIDE;
    typedef RefSpec TypeOf_AudioFX_TOP;
    typedef UMath::Vector4 TypeOf_AudioQuadratic;
    typedef EA::Reflection::Float TypeOf_BmMultiply;
    typedef EA::Reflection::Float TypeOf_BrMultiply;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef UMath::Vector4 TypeOf_ConstantBm;
    typedef UMath::Vector4 TypeOf_ConstantBr;
    typedef UMath::Vector4 TypeOf_EmitterQuadratic;
    typedef EA::Reflection::Bool TypeOf_FogEnable;
    typedef EA::Reflection::Float TypeOf_FogG;
    typedef EA::Reflection::Float TypeOf_InScatterMulitply;
    typedef EA::Reflection::Float TypeOf_InheritVelocity;
    typedef EA::Reflection::Float TypeOf_VisualCullDist;
    typedef RefSpec TypeOf_emittergroup;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("effects");
    effects(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    effects(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    effects(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    effects(const effects &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    effects(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~effects() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xebcee74c;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xebcee74c, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const effects &operator=(const effects &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const effects &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AudioCullDist(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x98dba438);
    }
    bool AudioCullDist(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x98dba438, result);
    }
    const EA::Reflection::Float &AudioCullDist() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x98dba438);
    }
    bool SET_AudioCullDist(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x98dba438, input);
    }
    bool AudioFX_BOTTOM(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe82f96cf);
    }
    bool AudioFX_BOTTOM(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xe82f96cf, result);
    }
    const RefSpec &AudioFX_BOTTOM() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xe82f96cf);
    }
    bool SET_AudioFX_BOTTOM(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xe82f96cf, input);
    }
    bool AudioFX_DEFAULT(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x20ef09ce);
    }
    bool AudioFX_DEFAULT(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AudioFX_DEFAULT, result);
    }
    const RefSpec &AudioFX_DEFAULT() const {
        ATTRIB_CODEGEN_GETLAYOUT(AudioFX_DEFAULT);
    }
    bool SET_AudioFX_DEFAULT(const RefSpec &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AudioFX_DEFAULT, input);
    }
    bool AudioFX_FRONT(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x53de9f00);
    }
    bool AudioFX_FRONT(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x53de9f00, result);
    }
    const RefSpec &AudioFX_FRONT() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x53de9f00);
    }
    bool SET_AudioFX_FRONT(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x53de9f00, input);
    }
    bool AudioFX_REAR(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x7bc5f444);
    }
    bool AudioFX_REAR(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x7bc5f444, result);
    }
    const RefSpec &AudioFX_REAR() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x7bc5f444);
    }
    bool SET_AudioFX_REAR(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x7bc5f444, input);
    }
    bool AudioFX_SIDE(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4c64ed7b);
    }
    bool AudioFX_SIDE(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x4c64ed7b, result);
    }
    const RefSpec &AudioFX_SIDE() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x4c64ed7b);
    }
    bool SET_AudioFX_SIDE(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x4c64ed7b, input);
    }
    bool AudioFX_TOP(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x74ef4fc8);
    }
    bool AudioFX_TOP(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x74ef4fc8, result);
    }
    const RefSpec &AudioFX_TOP() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x74ef4fc8);
    }
    bool SET_AudioFX_TOP(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x74ef4fc8, input);
    }
    bool AudioQuadratic(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x15e6552f);
    }
    bool AudioQuadratic(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x15e6552f, result);
    }
    const UMath::Vector4 &AudioQuadratic() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x15e6552f);
    }
    bool SET_AudioQuadratic(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x15e6552f, input);
    }
    bool BmMultiply(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc8baf5d6);
    }
    bool BmMultiply(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc8baf5d6, result);
    }
    const EA::Reflection::Float &BmMultiply() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc8baf5d6);
    }
    bool SET_BmMultiply(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc8baf5d6, input);
    }
    bool BrMultiply(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x555fd699);
    }
    bool BrMultiply(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x555fd699, result);
    }
    const EA::Reflection::Float &BrMultiply() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x555fd699);
    }
    bool SET_BrMultiply(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x555fd699, input);
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
    bool ConstantBm(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x2cf0b378);
    }
    bool ConstantBm(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x2cf0b378, result);
    }
    const UMath::Vector4 &ConstantBm() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x2cf0b378);
    }
    bool SET_ConstantBm(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x2cf0b378, input);
    }
    bool ConstantBr(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xa450f56c);
    }
    bool ConstantBr(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0xa450f56c, result);
    }
    const UMath::Vector4 &ConstantBr() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0xa450f56c);
    }
    bool SET_ConstantBr(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0xa450f56c, input);
    }
    bool EmitterQuadratic(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xa9402c33);
    }
    bool EmitterQuadratic(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0xa9402c33, result);
    }
    const UMath::Vector4 &EmitterQuadratic() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0xa9402c33);
    }
    bool SET_EmitterQuadratic(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0xa9402c33, input);
    }
    bool FogEnable(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x110882d5);
    }
    bool FogEnable(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x110882d5, result);
    }
    const EA::Reflection::Bool &FogEnable() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x110882d5);
    }
    bool SET_FogEnable(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x110882d5, input);
    }
    bool FogG(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdba22a95);
    }
    bool FogG(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xdba22a95, result);
    }
    const EA::Reflection::Float &FogG() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xdba22a95);
    }
    bool SET_FogG(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xdba22a95, input);
    }
    bool InScatterMulitply(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2f70d78a);
    }
    bool InScatterMulitply(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x2f70d78a, result);
    }
    const EA::Reflection::Float &InScatterMulitply() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x2f70d78a);
    }
    bool SET_InScatterMulitply(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x2f70d78a, input);
    }
    bool InheritVelocity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0099cb26);
    }
    bool InheritVelocity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0099cb26, result);
    }
    const EA::Reflection::Float &InheritVelocity() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0099cb26);
    }
    bool SET_InheritVelocity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0099cb26, input);
    }
    bool VisualCullDist(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1a2f2b1b);
    }
    bool VisualCullDist(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x1a2f2b1b, result);
    }
    const EA::Reflection::Float &VisualCullDist() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x1a2f2b1b);
    }
    bool SET_VisualCullDist(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x1a2f2b1b, input);
    }
    bool emittergroup(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xaba86e60);
    }
    bool emittergroup(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xaba86e60, result);
    }
    const RefSpec &emittergroup() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xaba86e60);
    }
    bool SET_emittergroup(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xaba86e60, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    effects &ConvertFromInstance(Instance &src) {}
    const effects &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key effects = 0xebcee74c;

}; // namespace ClassName

namespace Hash {
namespace effects {

static const Key AudioCullDist = 0x98dba438;
static const Key AudioFX_BOTTOM = 0xe82f96cf;
static const Key AudioFX_DEFAULT = 0x20ef09ce;
static const Key AudioFX_FRONT = 0x53de9f00;
static const Key AudioFX_REAR = 0x7bc5f444;
static const Key AudioFX_SIDE = 0x4c64ed7b;
static const Key AudioFX_TOP = 0x74ef4fc8;
static const Key AudioQuadratic = 0x15e6552f;
static const Key BmMultiply = 0xc8baf5d6;
static const Key BrMultiply = 0x555fd699;
static const Key CollectionName = 0x9ca1c8f9;
static const Key ConstantBm = 0x2cf0b378;
static const Key ConstantBr = 0xa450f56c;
static const Key EmitterQuadratic = 0xa9402c33;
static const Key FogEnable = 0x110882d5;
static const Key FogG = 0xdba22a95;
static const Key InScatterMulitply = 0x2f70d78a;
static const Key InheritVelocity = 0x0099cb26;
static const Key VisualCullDist = 0x1a2f2b1b;
static const Key emittergroup = 0xaba86e60;

}; // namespace effects
}; // namespace Hash

inline Key Gen::effects::ClassKey() {
    return ClassName::effects;
}

}; // namespace Attrib

#endif
