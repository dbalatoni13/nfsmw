#ifndef _attrib_gen_aud_moment_strm_h
#define _attrib_gen_aud_moment_strm_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct aud_moment_strm : Instance {
    struct _LayoutStruct {
        Csis::Type_SoundFX_Type stream;    // offset 0x0, size 0x4
        Csis::Type_SoundFX_Param param;    // offset 0x4, size 0x4
        eVOL_MOMENT VolSlot;               // offset 0x8, size 0x4
        EA::Reflection::Int8 strmpriority; // offset 0xc, size 0x1
    };

    typedef EA::Reflection::Bool TypeOf_CanInterupt;
    typedef EA::Reflection::Bool TypeOf_IsPositioned;
    typedef eVOL_MOMENT TypeOf_VolSlot;
    typedef Csis::Type_SoundFX_Param TypeOf_param;
    typedef Csis::Type_SoundFX_Type TypeOf_stream;
    typedef EA::Reflection::Int8 TypeOf_strmpriority;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("aud_moment_strm");
    aud_moment_strm(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aud_moment_strm(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aud_moment_strm(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aud_moment_strm(const aud_moment_strm &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    aud_moment_strm(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~aud_moment_strm() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xd2410816;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xd2410816, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const aud_moment_strm &operator=(const aud_moment_strm &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const aud_moment_strm &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CanInterupt(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb34fc11d);
    }
    bool CanInterupt(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb34fc11d, result);
    }
    const EA::Reflection::Bool &CanInterupt() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb34fc11d);
    }
    bool SET_CanInterupt(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb34fc11d, input);
    }
    bool IsPositioned(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xc4e7f7c2);
    }
    bool IsPositioned(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xc4e7f7c2, result);
    }
    const EA::Reflection::Bool &IsPositioned() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xc4e7f7c2);
    }
    bool SET_IsPositioned(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xc4e7f7c2, input);
    }
    bool VolSlot(TAttrib<eVOL_MOMENT> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(eVOL_MOMENT, 0x1dfda17f);
    }
    bool VolSlot(eVOL_MOMENT &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VolSlot, result);
    }
    const eVOL_MOMENT &VolSlot() const {
        ATTRIB_CODEGEN_GETLAYOUT(VolSlot);
    }
    bool SET_VolSlot(const eVOL_MOMENT &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VolSlot, input);
    }
    bool param(TAttrib<Csis::Type_SoundFX_Param> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Csis::Type_SoundFX_Param, 0xe54fb484);
    }
    bool param(Csis::Type_SoundFX_Param &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(param, result);
    }
    const Csis::Type_SoundFX_Param &param() const {
        ATTRIB_CODEGEN_GETLAYOUT(param);
    }
    bool SET_param(const Csis::Type_SoundFX_Param &input) {
        ATTRIB_CODEGEN_SETLAYOUT(param, input);
    }
    bool stream(TAttrib<Csis::Type_SoundFX_Type> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Csis::Type_SoundFX_Type, 0xd248a446);
    }
    bool stream(Csis::Type_SoundFX_Type &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(stream, result);
    }
    const Csis::Type_SoundFX_Type &stream() const {
        ATTRIB_CODEGEN_GETLAYOUT(stream);
    }
    bool SET_stream(const Csis::Type_SoundFX_Type &input) {
        ATTRIB_CODEGEN_SETLAYOUT(stream, input);
    }
    bool strmpriority(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xdbe77715);
    }
    bool strmpriority(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(strmpriority, result);
    }
    const EA::Reflection::Int8 &strmpriority() const {
        ATTRIB_CODEGEN_GETLAYOUT(strmpriority);
    }
    bool SET_strmpriority(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(strmpriority, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    aud_moment_strm &ConvertFromInstance(Instance &src) {}
    const aud_moment_strm &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key aud_moment_strm = 0xd2410816;

}; // namespace ClassName

namespace Hash {
namespace aud_moment_strm {

static const Key CanInterupt = 0xb34fc11d;
static const Key IsPositioned = 0xc4e7f7c2;
static const Key VolSlot = 0x1dfda17f;
static const Key param = 0xe54fb484;
static const Key stream = 0xd248a446;
static const Key strmpriority = 0xdbe77715;

}; // namespace aud_moment_strm
}; // namespace Hash

inline Key Gen::aud_moment_strm::ClassKey() {
    return ClassName::aud_moment_strm;
}

}; // namespace Attrib

#endif
