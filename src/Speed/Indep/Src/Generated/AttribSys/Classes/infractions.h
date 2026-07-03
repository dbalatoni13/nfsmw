#ifndef _attrib_gen_infractions_h
#define _attrib_gen_infractions_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct infractions : Instance {
    struct _LayoutStruct {
        EA::Reflection::UInt32 amount; // offset 0x0, size 0x4
    };

    typedef EA::Reflection::UInt32 TypeOf_amount;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("infractions");
    infractions(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    infractions(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    infractions(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    infractions(const infractions &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    infractions(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~infractions() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x2870de90;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x2870de90, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const infractions &operator=(const infractions &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const infractions &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool amount(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x912a2eb5);
    }
    bool amount(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(amount, result);
    }
    const EA::Reflection::UInt32 &amount() const {
        ATTRIB_CODEGEN_GETLAYOUT(amount);
    }
    bool SET_amount(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(amount, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    infractions &ConvertFromInstance(Instance &src) {}
    const infractions &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key infractions = 0x2870de90;

}; // namespace ClassName

namespace Hash {
namespace infractions {

static const Key amount = 0x912a2eb5;

}; // namespace infractions
}; // namespace Hash

inline Key Gen::infractions::ClassKey() {
    return ClassName::infractions;
}

}; // namespace Attrib

#endif
