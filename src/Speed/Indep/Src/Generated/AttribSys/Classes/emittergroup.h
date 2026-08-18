#ifndef _attrib_gen_emittergroup_h
#define _attrib_gen_emittergroup_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct emittergroup : Instance {
    struct _LayoutStruct {
        Private _Array_Emitters;             // offset 0x0, size 0x8
        RefSpec Emitters[6];                 // offset 0x8, size 0x48
        Private _Array_IntensityRanges;      // offset 0x50, size 0x8
        UMath::Vector2 IntensityRanges[5];   // offset 0x58, size 0x28
        EA::Reflection::Text CollectionName; // offset 0x80, size 0x4
        EA::Reflection::Float FarClip;       // offset 0x84, size 0x4
    };

    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef RefSpec TypeOf_Emitters;
    typedef EA::Reflection::Float TypeOf_FarClip;
    typedef UMath::Vector2 TypeOf_IntensityRanges;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("emittergroup");
    emittergroup(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emittergroup(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emittergroup(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emittergroup(const emittergroup &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emittergroup(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~emittergroup() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xaba86e60;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xaba86e60, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const emittergroup &operator=(const emittergroup &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const emittergroup &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool Emitters(TAttrib<RefSpec> &result) const;
    bool Emitters(RefSpec &result, unsigned int index) const;
    const RefSpec &Emitters(unsigned int index) const;
    unsigned int Num_Emitters() const;
    bool SET_Emitters(const RefSpec &input, unsigned int index);

    bool FarClip(TAttrib<EA::Reflection::Float> &result) const;
    bool FarClip(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FarClip() const;
    bool SET_FarClip(const EA::Reflection::Float &input);

    bool IntensityRanges(TAttrib<UMath::Vector2> &result) const;
    bool IntensityRanges(UMath::Vector2 &result, unsigned int index) const;
    const UMath::Vector2 &IntensityRanges(unsigned int index) const;
    unsigned int Num_IntensityRanges() const;
    bool SET_IntensityRanges(const UMath::Vector2 &input, unsigned int index);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    emittergroup &ConvertFromInstance(Instance &src) {}
    const emittergroup &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key emittergroup = 0xaba86e60;

}; // namespace ClassName

namespace Hash {
namespace emittergroup {

static const Key CollectionName = 0x9ca1c8f9;
static const Key Emitters = 0x1c943636;
static const Key FarClip = 0x66052349;
static const Key IntensityRanges = 0x6d261bde;

}; // namespace emittergroup
}; // namespace Hash

inline Key Gen::emittergroup::ClassKey() {
    return ClassName::emittergroup;
}

inline bool Gen::emittergroup::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::emittergroup::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::emittergroup::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::emittergroup::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::emittergroup::Emitters(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x1c943636);
}
inline const RefSpec &Gen::emittergroup::Emitters(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(RefSpec, Emitters, index);
}
inline bool Gen::emittergroup::Emitters(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(Emitters, result, index);
}
inline unsigned int Gen::emittergroup::Num_Emitters() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(Emitters);
}
inline bool Gen::emittergroup::SET_Emitters(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(Emitters, input, index);
}

inline bool Gen::emittergroup::FarClip(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x66052349);
}
inline const EA::Reflection::Float &Gen::emittergroup::FarClip() const {
    ATTRIB_CODEGEN_GETLAYOUT(FarClip);
}
inline bool Gen::emittergroup::FarClip(EA::Reflection::Float &result) const {
    result = FarClip();
    return true;
}
inline bool Gen::emittergroup::SET_FarClip(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FarClip, input);
}

inline bool Gen::emittergroup::IntensityRanges(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x6d261bde);
}
inline const UMath::Vector2 &Gen::emittergroup::IntensityRanges(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(UMath::Vector2, IntensityRanges, index);
}
inline bool Gen::emittergroup::IntensityRanges(UMath::Vector2 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(IntensityRanges, result, index);
}
inline unsigned int Gen::emittergroup::Num_IntensityRanges() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(IntensityRanges);
}
inline bool Gen::emittergroup::SET_IntensityRanges(const UMath::Vector2 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(IntensityRanges, input, index);
}

}; // namespace Attrib

#endif
