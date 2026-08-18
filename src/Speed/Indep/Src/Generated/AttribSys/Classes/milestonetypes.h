#ifndef _attrib_gen_milestonetypes_h
#define _attrib_gen_milestonetypes_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct milestonetypes : Instance {
    struct _LayoutStruct {
        EA::Reflection::Text CollectionName; // offset 0x0, size 0x4
        EA::Reflection::Int32 MilestoneType; // offset 0x4, size 0x4
    };

    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Int32 TypeOf_LocalizationTag;
    typedef EA::Reflection::Int32 TypeOf_MilestoneType;
    typedef EA::Reflection::Bool TypeOf_ResetWhenPursuitStarts;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("milestonetypes");
    milestonetypes(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    milestonetypes(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    milestonetypes(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    milestonetypes(const milestonetypes &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    milestonetypes(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~milestonetypes() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xe4c3d904;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xe4c3d904, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const milestonetypes &operator=(const milestonetypes &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const milestonetypes &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool LocalizationTag(TAttrib<EA::Reflection::Int32> &result) const;
    bool LocalizationTag(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &LocalizationTag() const;
    bool SET_LocalizationTag(const EA::Reflection::Int32 &input);

    bool MilestoneType(TAttrib<EA::Reflection::Int32> &result) const;
    bool MilestoneType(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MilestoneType() const;
    bool SET_MilestoneType(const EA::Reflection::Int32 &input);

    bool ResetWhenPursuitStarts(TAttrib<EA::Reflection::Bool> &result) const;
    bool ResetWhenPursuitStarts(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &ResetWhenPursuitStarts() const;
    bool SET_ResetWhenPursuitStarts(const EA::Reflection::Bool &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    milestonetypes &ConvertFromInstance(Instance &src) {}
    const milestonetypes &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key milestonetypes = 0xe4c3d904;

}; // namespace ClassName

namespace Hash {
namespace milestonetypes {

static const Key CollectionName = 0x9ca1c8f9;
static const Key LocalizationTag = 0xdb89ab5c;
static const Key MilestoneType = 0x16df392a;
static const Key ResetWhenPursuitStarts = 0xab4a196f;

}; // namespace milestonetypes
}; // namespace Hash

inline Key Gen::milestonetypes::ClassKey() {
    return ClassName::milestonetypes;
}

inline bool Gen::milestonetypes::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::milestonetypes::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::milestonetypes::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::milestonetypes::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::milestonetypes::LocalizationTag(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdb89ab5c);
}
inline const EA::Reflection::Int32 &Gen::milestonetypes::LocalizationTag() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdb89ab5c);
}
inline bool Gen::milestonetypes::LocalizationTag(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdb89ab5c, result);
}
inline bool Gen::milestonetypes::SET_LocalizationTag(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdb89ab5c, input);
}

inline bool Gen::milestonetypes::MilestoneType(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x16df392a);
}
inline const EA::Reflection::Int32 &Gen::milestonetypes::MilestoneType() const {
    ATTRIB_CODEGEN_GETLAYOUT(MilestoneType);
}
inline bool Gen::milestonetypes::MilestoneType(EA::Reflection::Int32 &result) const {
    result = MilestoneType();
    return true;
}
inline bool Gen::milestonetypes::SET_MilestoneType(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MilestoneType, input);
}

inline bool Gen::milestonetypes::ResetWhenPursuitStarts(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xab4a196f);
}
inline const EA::Reflection::Bool &Gen::milestonetypes::ResetWhenPursuitStarts() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xab4a196f);
}
inline bool Gen::milestonetypes::ResetWhenPursuitStarts(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xab4a196f, result);
}
inline bool Gen::milestonetypes::SET_ResetWhenPursuitStarts(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xab4a196f, input);
}

}; // namespace Attrib

#endif
