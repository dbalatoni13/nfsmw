#ifndef _attrib_gen_emitteruv_h
#define _attrib_gen_emitteruv_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct emitteruv : Instance {
    struct _LayoutStruct {
        EA::Reflection::Float EndV;   // offset 0x0, size 0x4
        EA::Reflection::Float StartU; // offset 0x4, size 0x4
        EA::Reflection::Float EndU;   // offset 0x8, size 0x4
        EA::Reflection::Float StartV; // offset 0xc, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_EndU;
    typedef EA::Reflection::Float TypeOf_EndV;
    typedef EA::Reflection::Float TypeOf_StartU;
    typedef EA::Reflection::Float TypeOf_StartV;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("emitteruv");
    emitteruv(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitteruv(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitteruv(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitteruv(const emitteruv &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitteruv(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~emitteruv() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xe4983a7d;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xe4983a7d, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const emitteruv &operator=(const emitteruv &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const emitteruv &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool EndU(TAttrib<EA::Reflection::Float> &result) const;
    bool EndU(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EndU() const;
    bool SET_EndU(const EA::Reflection::Float &input);

    bool EndV(TAttrib<EA::Reflection::Float> &result) const;
    bool EndV(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EndV() const;
    bool SET_EndV(const EA::Reflection::Float &input);

    bool StartU(TAttrib<EA::Reflection::Float> &result) const;
    bool StartU(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StartU() const;
    bool SET_StartU(const EA::Reflection::Float &input);

    bool StartV(TAttrib<EA::Reflection::Float> &result) const;
    bool StartV(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StartV() const;
    bool SET_StartV(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    emitteruv &ConvertFromInstance(Instance &src) {}
    const emitteruv &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key emitteruv = 0xe4983a7d;

}; // namespace ClassName

namespace Hash {
namespace emitteruv {

static const Key EndU = 0x841d0551;
static const Key EndV = 0x0d28675b;
static const Key StartU = 0x98b1da40;
static const Key StartV = 0xb1ebd0d4;

}; // namespace emitteruv
}; // namespace Hash

inline Key Gen::emitteruv::ClassKey() {
    return ClassName::emitteruv;
}

inline bool Gen::emitteruv::EndU(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x841d0551);
}
inline const EA::Reflection::Float &Gen::emitteruv::EndU() const {
    ATTRIB_CODEGEN_GETLAYOUT(EndU);
}
inline bool Gen::emitteruv::EndU(EA::Reflection::Float &result) const {
    result = EndU();
    return true;
}
inline bool Gen::emitteruv::SET_EndU(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EndU, input);
}

inline bool Gen::emitteruv::EndV(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0d28675b);
}
inline const EA::Reflection::Float &Gen::emitteruv::EndV() const {
    ATTRIB_CODEGEN_GETLAYOUT(EndV);
}
inline bool Gen::emitteruv::EndV(EA::Reflection::Float &result) const {
    result = EndV();
    return true;
}
inline bool Gen::emitteruv::SET_EndV(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EndV, input);
}

inline bool Gen::emitteruv::StartU(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x98b1da40);
}
inline const EA::Reflection::Float &Gen::emitteruv::StartU() const {
    ATTRIB_CODEGEN_GETLAYOUT(StartU);
}
inline bool Gen::emitteruv::StartU(EA::Reflection::Float &result) const {
    result = StartU();
    return true;
}
inline bool Gen::emitteruv::SET_StartU(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(StartU, input);
}

inline bool Gen::emitteruv::StartV(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb1ebd0d4);
}
inline const EA::Reflection::Float &Gen::emitteruv::StartV() const {
    ATTRIB_CODEGEN_GETLAYOUT(StartV);
}
inline bool Gen::emitteruv::StartV(EA::Reflection::Float &result) const {
    result = StartV();
    return true;
}
inline bool Gen::emitteruv::SET_StartV(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(StartV, input);
}

}; // namespace Attrib

#endif
