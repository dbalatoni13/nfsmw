#ifndef _attrib_gen_visuallook_h
#define _attrib_gen_visuallook_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct visuallook : Instance {
    struct _LayoutStruct {
        UMath::Matrix4 DetailMapCurve;              // offset 0x0, size 0x40
        UMath::Matrix4 BlackBloomCurve;             // offset 0x40, size 0x40
        UMath::Matrix4 ColourBloomCurve;            // offset 0x80, size 0x40
        UMath::Vector4 ColourBloomTint;             // offset 0xc0, size 0x10
        EA::Reflection::Float ColourBloomIntensity; // offset 0xd0, size 0x4
        EA::Reflection::Float Desaturation;         // offset 0xd4, size 0x4
        EA::Reflection::Float DetailMapIntensity;   // offset 0xd8, size 0x4
        EA::Reflection::Float BlackBloomIntensity;  // offset 0xdc, size 0x4
    };

    typedef UMath::Matrix4 TypeOf_BlackBloomCurve;
    typedef EA::Reflection::Float TypeOf_BlackBloomIntensity;
    typedef UMath::Matrix4 TypeOf_ColourBloomCurve;
    typedef EA::Reflection::Float TypeOf_ColourBloomIntensity;
    typedef UMath::Vector4 TypeOf_ColourBloomTint;
    typedef EA::Reflection::Float TypeOf_Desaturation;
    typedef UMath::Matrix4 TypeOf_DetailMapCurve;
    typedef EA::Reflection::Float TypeOf_DetailMapIntensity;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("visuallook");
    visuallook(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallook(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallook(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallook(const visuallook &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallook(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~visuallook() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x339f7d3d;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x339f7d3d, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const visuallook &operator=(const visuallook &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const visuallook &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BlackBloomCurve(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0xaaaccdf3);
    }
    bool BlackBloomCurve(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BlackBloomCurve, result);
    }
    const UMath::Matrix4 &BlackBloomCurve() const {
        ATTRIB_CODEGEN_GETLAYOUT(BlackBloomCurve);
    }
    bool SET_BlackBloomCurve(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BlackBloomCurve, input);
    }
    bool BlackBloomIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7e609e04);
    }
    bool BlackBloomIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BlackBloomIntensity, result);
    }
    const EA::Reflection::Float &BlackBloomIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(BlackBloomIntensity);
    }
    bool SET_BlackBloomIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BlackBloomIntensity, input);
    }
    bool ColourBloomCurve(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x1d521c97);
    }
    bool ColourBloomCurve(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ColourBloomCurve, result);
    }
    const UMath::Matrix4 &ColourBloomCurve() const {
        ATTRIB_CODEGEN_GETLAYOUT(ColourBloomCurve);
    }
    bool SET_ColourBloomCurve(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ColourBloomCurve, input);
    }
    bool ColourBloomIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0913f193);
    }
    bool ColourBloomIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ColourBloomIntensity, result);
    }
    const EA::Reflection::Float &ColourBloomIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(ColourBloomIntensity);
    }
    bool SET_ColourBloomIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ColourBloomIntensity, input);
    }
    bool ColourBloomTint(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x06d0cd52);
    }
    bool ColourBloomTint(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ColourBloomTint, result);
    }
    const UMath::Vector4 &ColourBloomTint() const {
        ATTRIB_CODEGEN_GETLAYOUT(ColourBloomTint);
    }
    bool SET_ColourBloomTint(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ColourBloomTint, input);
    }
    bool Desaturation(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x771bbe7f);
    }
    bool Desaturation(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Desaturation, result);
    }
    const EA::Reflection::Float &Desaturation() const {
        ATTRIB_CODEGEN_GETLAYOUT(Desaturation);
    }
    bool SET_Desaturation(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Desaturation, input);
    }
    bool DetailMapCurve(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x51376819);
    }
    bool DetailMapCurve(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DetailMapCurve, result);
    }
    const UMath::Matrix4 &DetailMapCurve() const {
        ATTRIB_CODEGEN_GETLAYOUT(DetailMapCurve);
    }
    bool SET_DetailMapCurve(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DetailMapCurve, input);
    }
    bool DetailMapIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x107c0f71);
    }
    bool DetailMapIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DetailMapIntensity, result);
    }
    const EA::Reflection::Float &DetailMapIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(DetailMapIntensity);
    }
    bool SET_DetailMapIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DetailMapIntensity, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    visuallook &ConvertFromInstance(Instance &src) {}
    const visuallook &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key visuallook = 0x339f7d3d;

}; // namespace ClassName

namespace Hash {
namespace visuallook {

static const Key BlackBloomCurve = 0xaaaccdf3;
static const Key BlackBloomIntensity = 0x7e609e04;
static const Key ColourBloomCurve = 0x1d521c97;
static const Key ColourBloomIntensity = 0x0913f193;
static const Key ColourBloomTint = 0x06d0cd52;
static const Key Desaturation = 0x771bbe7f;
static const Key DetailMapCurve = 0x51376819;
static const Key DetailMapIntensity = 0x107c0f71;

}; // namespace visuallook
}; // namespace Hash

inline Key Gen::visuallook::ClassKey() {
    return ClassName::visuallook;
}

}; // namespace Attrib

#endif
