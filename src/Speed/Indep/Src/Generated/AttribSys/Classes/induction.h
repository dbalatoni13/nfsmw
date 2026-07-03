#ifndef _attrib_gen_induction_h
#define _attrib_gen_induction_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct induction : Instance {
    struct _LayoutStruct {
        EA::Reflection::Float LOW_BOOST;       // offset 0x0, size 0x4
        EA::Reflection::Float SPOOL_TIME_DOWN; // offset 0x4, size 0x4
        EA::Reflection::Float VACUUM;          // offset 0x8, size 0x4
        EA::Reflection::Float SPOOL;           // offset 0xc, size 0x4
        EA::Reflection::Float SPOOL_TIME_UP;   // offset 0x10, size 0x4
        EA::Reflection::Float PSI;             // offset 0x14, size 0x4
        EA::Reflection::Float HIGH_BOOST;      // offset 0x18, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_HIGH_BOOST;
    typedef EA::Reflection::Float TypeOf_LOW_BOOST;
    typedef EA::Reflection::Float TypeOf_PSI;
    typedef EA::Reflection::Float TypeOf_SPOOL;
    typedef EA::Reflection::Float TypeOf_SPOOL_TIME_DOWN;
    typedef EA::Reflection::Float TypeOf_SPOOL_TIME_UP;
    typedef EA::Reflection::Float TypeOf_VACUUM;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("induction");
    induction(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    induction(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    induction(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    induction(const induction &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    induction(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~induction() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xc92a0142;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xc92a0142, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const induction &operator=(const induction &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const induction &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool HIGH_BOOST(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7168ef58);
    }
    bool HIGH_BOOST(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(HIGH_BOOST, result);
    }
    const EA::Reflection::Float &HIGH_BOOST() const {
        ATTRIB_CODEGEN_GETLAYOUT(HIGH_BOOST);
    }
    bool SET_HIGH_BOOST(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(HIGH_BOOST, input);
    }
    bool LOW_BOOST(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7a3181c0);
    }
    bool LOW_BOOST(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LOW_BOOST, result);
    }
    const EA::Reflection::Float &LOW_BOOST() const {
        ATTRIB_CODEGEN_GETLAYOUT(LOW_BOOST);
    }
    bool SET_LOW_BOOST(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LOW_BOOST, input);
    }
    bool PSI(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xba0a7d04);
    }
    bool PSI(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PSI, result);
    }
    const EA::Reflection::Float &PSI() const {
        ATTRIB_CODEGEN_GETLAYOUT(PSI);
    }
    bool SET_PSI(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PSI, input);
    }
    bool SPOOL(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xcf11af87);
    }
    bool SPOOL(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SPOOL, result);
    }
    const EA::Reflection::Float &SPOOL() const {
        ATTRIB_CODEGEN_GETLAYOUT(SPOOL);
    }
    bool SET_SPOOL(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SPOOL, input);
    }
    bool SPOOL_TIME_DOWN(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1f158097);
    }
    bool SPOOL_TIME_DOWN(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SPOOL_TIME_DOWN, result);
    }
    const EA::Reflection::Float &SPOOL_TIME_DOWN() const {
        ATTRIB_CODEGEN_GETLAYOUT(SPOOL_TIME_DOWN);
    }
    bool SET_SPOOL_TIME_DOWN(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SPOOL_TIME_DOWN, input);
    }
    bool SPOOL_TIME_UP(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xac1c0d2f);
    }
    bool SPOOL_TIME_UP(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SPOOL_TIME_UP, result);
    }
    const EA::Reflection::Float &SPOOL_TIME_UP() const {
        ATTRIB_CODEGEN_GETLAYOUT(SPOOL_TIME_UP);
    }
    bool SET_SPOOL_TIME_UP(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SPOOL_TIME_UP, input);
    }
    bool VACUUM(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xee2c326f);
    }
    bool VACUUM(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VACUUM, result);
    }
    const EA::Reflection::Float &VACUUM() const {
        ATTRIB_CODEGEN_GETLAYOUT(VACUUM);
    }
    bool SET_VACUUM(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VACUUM, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    induction &ConvertFromInstance(Instance &src) {}
    const induction &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key induction = 0xc92a0142;

}; // namespace ClassName

namespace Hash {
namespace induction {

static const Key HIGH_BOOST = 0x7168ef58;
static const Key LOW_BOOST = 0x7a3181c0;
static const Key PSI = 0xba0a7d04;
static const Key SPOOL = 0xcf11af87;
static const Key SPOOL_TIME_DOWN = 0x1f158097;
static const Key SPOOL_TIME_UP = 0xac1c0d2f;
static const Key VACUUM = 0xee2c326f;

}; // namespace induction
}; // namespace Hash

inline Key Gen::induction::ClassKey() {
    return ClassName::induction;
}

}; // namespace Attrib

#endif
