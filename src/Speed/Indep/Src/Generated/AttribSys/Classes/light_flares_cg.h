#ifndef _attrib_gen_light_flares_cg_h
#define _attrib_gen_light_flares_cg_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct light_flares_cg : Instance {
    struct _LayoutStruct {
        UMath::Vector4 colour;                // offset 0x0, size 0x10
        EA::Reflection::Float MinSize;        // offset 0x10, size 0x4
        EA::Reflection::Float MinScale;       // offset 0x14, size 0x4
        EA::Reflection::Float Power;          // offset 0x18, size 0x4
        EA::Reflection::UInt32 flare_texture; // offset 0x1c, size 0x4
        EA::Reflection::Float Maxscale;       // offset 0x20, size 0x4
        EA::Reflection::Float MaxSize;        // offset 0x24, size 0x4
        EA::Reflection::Float ZBias;          // offset 0x28, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_MaxSize;
    typedef EA::Reflection::Float TypeOf_Maxscale;
    typedef EA::Reflection::Float TypeOf_MinScale;
    typedef EA::Reflection::Float TypeOf_MinSize;
    typedef EA::Reflection::Float TypeOf_Power;
    typedef EA::Reflection::Float TypeOf_ZBias;
    typedef UMath::Vector4 TypeOf_colour;
    typedef EA::Reflection::UInt32 TypeOf_flare_texture;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("light_flares_cg");
    light_flares_cg(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    light_flares_cg(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    light_flares_cg(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    light_flares_cg(const light_flares_cg &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    light_flares_cg(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~light_flares_cg() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xc7c5806d;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xc7c5806d, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const light_flares_cg &operator=(const light_flares_cg &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const light_flares_cg &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool MaxSize(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf7649e63);
    }
    bool MaxSize(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MaxSize, result);
    }
    const EA::Reflection::Float &MaxSize() const {
        ATTRIB_CODEGEN_GETLAYOUT(MaxSize);
    }
    bool SET_MaxSize(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MaxSize, input);
    }
    bool Maxscale(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x07c20250);
    }
    bool Maxscale(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Maxscale, result);
    }
    const EA::Reflection::Float &Maxscale() const {
        ATTRIB_CODEGEN_GETLAYOUT(Maxscale);
    }
    bool SET_Maxscale(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Maxscale, input);
    }
    bool MinScale(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0fa46807);
    }
    bool MinScale(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinScale, result);
    }
    const EA::Reflection::Float &MinScale() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinScale);
    }
    bool SET_MinScale(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinScale, input);
    }
    bool MinSize(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1a0b5461);
    }
    bool MinSize(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinSize, result);
    }
    const EA::Reflection::Float &MinSize() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinSize);
    }
    bool SET_MinSize(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinSize, input);
    }
    bool Power(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x96e40580);
    }
    bool Power(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Power, result);
    }
    const EA::Reflection::Float &Power() const {
        ATTRIB_CODEGEN_GETLAYOUT(Power);
    }
    bool SET_Power(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Power, input);
    }
    bool ZBias(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa02742a6);
    }
    bool ZBias(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ZBias, result);
    }
    const EA::Reflection::Float &ZBias() const {
        ATTRIB_CODEGEN_GETLAYOUT(ZBias);
    }
    bool SET_ZBias(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ZBias, input);
    }
    bool colour(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x05b98b2c);
    }
    bool colour(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(colour, result);
    }
    const UMath::Vector4 &colour() const {
        ATTRIB_CODEGEN_GETLAYOUT(colour);
    }
    bool SET_colour(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(colour, input);
    }
    bool flare_texture(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xf093aacf);
    }
    bool flare_texture(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(flare_texture, result);
    }
    const EA::Reflection::UInt32 &flare_texture() const {
        ATTRIB_CODEGEN_GETLAYOUT(flare_texture);
    }
    bool SET_flare_texture(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(flare_texture, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    light_flares_cg &ConvertFromInstance(Instance &src) {}
    const light_flares_cg &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key light_flares_cg = 0xc7c5806d;

}; // namespace ClassName

namespace Hash {
namespace light_flares_cg {

static const Key MaxSize = 0xf7649e63;
static const Key Maxscale = 0x07c20250;
static const Key MinScale = 0x0fa46807;
static const Key MinSize = 0x1a0b5461;
static const Key Power = 0x96e40580;
static const Key ZBias = 0xa02742a6;
static const Key colour = 0x05b98b2c;
static const Key flare_texture = 0xf093aacf;

}; // namespace light_flares_cg
}; // namespace Hash

inline Key Gen::light_flares_cg::ClassKey() {
    return ClassName::light_flares_cg;
}

}; // namespace Attrib

#endif
