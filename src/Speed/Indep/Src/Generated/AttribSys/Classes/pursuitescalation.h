#ifndef _attrib_gen_pursuitescalation_h
#define _attrib_gen_pursuitescalation_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct pursuitescalation : Instance {
    typedef RefSpec TypeOf_heattable;
    typedef RefSpec TypeOf_racetable;
    typedef RefSpec TypeOf_supportracetable;
    typedef RefSpec TypeOf_supporttable;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("pursuitescalation");
    pursuitescalation(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    pursuitescalation(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    pursuitescalation(const Instance &src) : Instance(src) {}
    pursuitescalation(const pursuitescalation &src) : Instance(src) {}
    pursuitescalation(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~pursuitescalation() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xd6d4330b;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xd6d4330b, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const pursuitescalation &operator=(const pursuitescalation &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const pursuitescalation &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool heattable(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd4b0cc11);
    }
    bool heattable(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xd4b0cc11, result, index);
    }
    const RefSpec &heattable(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xd4b0cc11, index);
    }
    unsigned int Num_heattable() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd4b0cc11);
    }
    bool SET_heattable(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xd4b0cc11, input, index);
    }
    bool racetable(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x2283ecaf);
    }
    bool racetable(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x2283ecaf, result, index);
    }
    const RefSpec &racetable(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x2283ecaf, index);
    }
    unsigned int Num_racetable() const {
        ATTRIB_CODEGEN_GETLENGTH(0x2283ecaf);
    }
    bool SET_racetable(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x2283ecaf, input, index);
    }
    bool supportracetable(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe5332008);
    }
    bool supportracetable(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xe5332008, result, index);
    }
    const RefSpec &supportracetable(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xe5332008, index);
    }
    unsigned int Num_supportracetable() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe5332008);
    }
    bool SET_supportracetable(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xe5332008, input, index);
    }
    bool supporttable(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xf3918f68);
    }
    bool supporttable(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xf3918f68, result, index);
    }
    const RefSpec &supporttable(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xf3918f68, index);
    }
    unsigned int Num_supporttable() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf3918f68);
    }
    bool SET_supporttable(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xf3918f68, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    pursuitescalation &ConvertFromInstance(Instance &src) {}
    const pursuitescalation &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key pursuitescalation = 0xd6d4330b;

}; // namespace ClassName

namespace Hash {
namespace pursuitescalation {

static const Key heattable = 0xd4b0cc11;
static const Key racetable = 0x2283ecaf;
static const Key supportracetable = 0xe5332008;
static const Key supporttable = 0xf3918f68;

}; // namespace pursuitescalation
}; // namespace Hash

inline Key Gen::pursuitescalation::ClassKey() {
    return ClassName::pursuitescalation;
}

}; // namespace Attrib

#endif
