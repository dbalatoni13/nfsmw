#ifndef _attrib_gen_fuelcell_effect_h
#define _attrib_gen_fuelcell_effect_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct fuelcell_effect : Instance {
    struct _LayoutStruct {
        EA::Reflection::Bool doTest; // offset 0x0, size 0x1
    };

    typedef RefSpec TypeOf_NGEmitter;
    typedef EA::Reflection::Bool TypeOf_doTest;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("fuelcell_effect");
    fuelcell_effect(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_effect(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_effect(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_effect(const fuelcell_effect &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_effect(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~fuelcell_effect() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x6f5943f1;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x6f5943f1, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const fuelcell_effect &operator=(const fuelcell_effect &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const fuelcell_effect &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool NGEmitter(TAttrib<RefSpec> &result) const;
    bool NGEmitter(RefSpec &result, unsigned int index) const;
    const RefSpec &NGEmitter(unsigned int index) const;
    unsigned int Num_NGEmitter() const;
    bool SET_NGEmitter(const RefSpec &input, unsigned int index);

    bool doTest(TAttrib<EA::Reflection::Bool> &result) const;
    bool doTest(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &doTest() const;
    bool SET_doTest(const EA::Reflection::Bool &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    fuelcell_effect &ConvertFromInstance(Instance &src) {}
    const fuelcell_effect &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key fuelcell_effect = 0x6f5943f1;

}; // namespace ClassName

namespace Hash {
namespace fuelcell_effect {

static const Key NGEmitter = 0xb0d98a89;
static const Key doTest = 0xaee3be58;

}; // namespace fuelcell_effect
}; // namespace Hash

inline Key Gen::fuelcell_effect::ClassKey() {
    return ClassName::fuelcell_effect;
}

inline bool Gen::fuelcell_effect::NGEmitter(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xb0d98a89);
}
inline const RefSpec &Gen::fuelcell_effect::NGEmitter(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xb0d98a89, index);
}
inline bool Gen::fuelcell_effect::NGEmitter(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xb0d98a89, result, index);
}
inline unsigned int Gen::fuelcell_effect::Num_NGEmitter() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb0d98a89);
}
inline bool Gen::fuelcell_effect::SET_NGEmitter(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xb0d98a89, input, index);
}

inline bool Gen::fuelcell_effect::doTest(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xaee3be58);
}
inline const EA::Reflection::Bool &Gen::fuelcell_effect::doTest() const {
    ATTRIB_CODEGEN_GETLAYOUT(doTest);
}
inline bool Gen::fuelcell_effect::doTest(EA::Reflection::Bool &result) const {
    result = doTest();
    return true;
}
inline bool Gen::fuelcell_effect::SET_doTest(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(doTest, input);
}

}; // namespace Attrib

#endif
