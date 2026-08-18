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
    bool CanInterupt(TAttrib<EA::Reflection::Bool> &result) const;
    bool CanInterupt(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CanInterupt() const;
    bool SET_CanInterupt(const EA::Reflection::Bool &input);

    bool IsPositioned(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsPositioned(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsPositioned() const;
    bool SET_IsPositioned(const EA::Reflection::Bool &input);

    bool VolSlot(TAttrib<eVOL_MOMENT> &result) const;
    bool VolSlot(eVOL_MOMENT &result) const;
    const eVOL_MOMENT &VolSlot() const;
    bool SET_VolSlot(const eVOL_MOMENT &input);

    bool param(TAttrib<Csis::Type_SoundFX_Param> &result) const;
    bool param(Csis::Type_SoundFX_Param &result) const;
    const Csis::Type_SoundFX_Param &param() const;
    bool SET_param(const Csis::Type_SoundFX_Param &input);

    bool stream(TAttrib<Csis::Type_SoundFX_Type> &result) const;
    bool stream(Csis::Type_SoundFX_Type &result) const;
    const Csis::Type_SoundFX_Type &stream() const;
    bool SET_stream(const Csis::Type_SoundFX_Type &input);

    bool strmpriority(TAttrib<EA::Reflection::Int8> &result) const;
    bool strmpriority(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &strmpriority() const;
    bool SET_strmpriority(const EA::Reflection::Int8 &input);

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

inline bool Gen::aud_moment_strm::CanInterupt(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb34fc11d);
}
inline const EA::Reflection::Bool &Gen::aud_moment_strm::CanInterupt() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb34fc11d);
}
inline bool Gen::aud_moment_strm::CanInterupt(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb34fc11d, result);
}
inline bool Gen::aud_moment_strm::SET_CanInterupt(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb34fc11d, input);
}

inline bool Gen::aud_moment_strm::IsPositioned(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xc4e7f7c2);
}
inline const EA::Reflection::Bool &Gen::aud_moment_strm::IsPositioned() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xc4e7f7c2);
}
inline bool Gen::aud_moment_strm::IsPositioned(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xc4e7f7c2, result);
}
inline bool Gen::aud_moment_strm::SET_IsPositioned(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xc4e7f7c2, input);
}

inline bool Gen::aud_moment_strm::VolSlot(TAttrib<eVOL_MOMENT> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(eVOL_MOMENT, 0x1dfda17f);
}
inline const eVOL_MOMENT &Gen::aud_moment_strm::VolSlot() const {
    ATTRIB_CODEGEN_GETLAYOUT(VolSlot);
}
inline bool Gen::aud_moment_strm::VolSlot(eVOL_MOMENT &result) const {
    result = VolSlot();
    return true;
}
inline bool Gen::aud_moment_strm::SET_VolSlot(const eVOL_MOMENT &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VolSlot, input);
}

inline bool Gen::aud_moment_strm::param(TAttrib<Csis::Type_SoundFX_Param> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Csis::Type_SoundFX_Param, 0xe54fb484);
}
inline const Csis::Type_SoundFX_Param &Gen::aud_moment_strm::param() const {
    ATTRIB_CODEGEN_GETLAYOUT(param);
}
inline bool Gen::aud_moment_strm::param(Csis::Type_SoundFX_Param &result) const {
    result = param();
    return true;
}
inline bool Gen::aud_moment_strm::SET_param(const Csis::Type_SoundFX_Param &input) {
    ATTRIB_CODEGEN_SETLAYOUT(param, input);
}

inline bool Gen::aud_moment_strm::stream(TAttrib<Csis::Type_SoundFX_Type> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Csis::Type_SoundFX_Type, 0xd248a446);
}
inline const Csis::Type_SoundFX_Type &Gen::aud_moment_strm::stream() const {
    ATTRIB_CODEGEN_GETLAYOUT(stream);
}
inline bool Gen::aud_moment_strm::stream(Csis::Type_SoundFX_Type &result) const {
    result = stream();
    return true;
}
inline bool Gen::aud_moment_strm::SET_stream(const Csis::Type_SoundFX_Type &input) {
    ATTRIB_CODEGEN_SETLAYOUT(stream, input);
}

inline bool Gen::aud_moment_strm::strmpriority(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xdbe77715);
}
inline const EA::Reflection::Int8 &Gen::aud_moment_strm::strmpriority() const {
    ATTRIB_CODEGEN_GETLAYOUT(strmpriority);
}
inline bool Gen::aud_moment_strm::strmpriority(EA::Reflection::Int8 &result) const {
    result = strmpriority();
    return true;
}
inline bool Gen::aud_moment_strm::SET_strmpriority(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(strmpriority, input);
}

}; // namespace Attrib

#endif
