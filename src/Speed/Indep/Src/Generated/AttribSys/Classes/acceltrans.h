#ifndef _attrib_gen_acceltrans_h
#define _attrib_gen_acceltrans_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct acceltrans : Instance {
    struct _LayoutStruct {
        EA::Reflection::UInt32 AccelFromIdle_PEAK_T;   // offset 0x0, size 0x4
        EA::Reflection::UInt32 AccelFromIdle_RESUME_T; // offset 0x4, size 0x4
        EA::Reflection::UInt32 AccelFromIdle_PEAK_RPM; // offset 0x8, size 0x4
        EA::Reflection::Float AccelFromIdle_PEAK_VOL;  // offset 0xc, size 0x4
    };

    typedef EA::Reflection::UInt32 TypeOf_AccelFromIdle_INTERUPT_T;
    typedef EA::Reflection::UInt32 TypeOf_AccelFromIdle_PEAK_RPM;
    typedef EA::Reflection::UInt32 TypeOf_AccelFromIdle_PEAK_T;
    typedef EA::Reflection::Float TypeOf_AccelFromIdle_PEAK_VOL;
    typedef EA::Reflection::UInt32 TypeOf_AccelFromIdle_RESUME_T;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("acceltrans");
    acceltrans(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    acceltrans(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    acceltrans(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    acceltrans(const acceltrans &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    acceltrans(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~acceltrans() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xff77f451;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xff77f451, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const acceltrans &operator=(const acceltrans &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const acceltrans &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AccelFromIdle_INTERUPT_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x49fb8ce5);
    }
    bool AccelFromIdle_INTERUPT_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x49fb8ce5, result);
    }
    const EA::Reflection::UInt32 &AccelFromIdle_INTERUPT_T() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x49fb8ce5);
    }
    bool SET_AccelFromIdle_INTERUPT_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x49fb8ce5, input);
    }
    bool AccelFromIdle_PEAK_RPM(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x07b946f1);
    }
    bool AccelFromIdle_PEAK_RPM(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelFromIdle_PEAK_RPM, result);
    }
    const EA::Reflection::UInt32 &AccelFromIdle_PEAK_RPM() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelFromIdle_PEAK_RPM);
    }
    bool SET_AccelFromIdle_PEAK_RPM(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelFromIdle_PEAK_RPM, input);
    }
    bool AccelFromIdle_PEAK_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xbd153261);
    }
    bool AccelFromIdle_PEAK_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelFromIdle_PEAK_T, result);
    }
    const EA::Reflection::UInt32 &AccelFromIdle_PEAK_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelFromIdle_PEAK_T);
    }
    bool SET_AccelFromIdle_PEAK_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelFromIdle_PEAK_T, input);
    }
    bool AccelFromIdle_PEAK_VOL(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x922b60d7);
    }
    bool AccelFromIdle_PEAK_VOL(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelFromIdle_PEAK_VOL, result);
    }
    const EA::Reflection::Float &AccelFromIdle_PEAK_VOL() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelFromIdle_PEAK_VOL);
    }
    bool SET_AccelFromIdle_PEAK_VOL(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelFromIdle_PEAK_VOL, input);
    }
    bool AccelFromIdle_RESUME_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x44bccb6e);
    }
    bool AccelFromIdle_RESUME_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelFromIdle_RESUME_T, result);
    }
    const EA::Reflection::UInt32 &AccelFromIdle_RESUME_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelFromIdle_RESUME_T);
    }
    bool SET_AccelFromIdle_RESUME_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelFromIdle_RESUME_T, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    acceltrans &ConvertFromInstance(Instance &src) {}
    const acceltrans &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key acceltrans = 0xff77f451;

}; // namespace ClassName

namespace Hash {
namespace acceltrans {

static const Key AccelFromIdle_INTERUPT_T = 0x49fb8ce5;
static const Key AccelFromIdle_PEAK_RPM = 0x07b946f1;
static const Key AccelFromIdle_PEAK_T = 0xbd153261;
static const Key AccelFromIdle_PEAK_VOL = 0x922b60d7;
static const Key AccelFromIdle_RESUME_T = 0x44bccb6e;

}; // namespace acceltrans
}; // namespace Hash

inline Key Gen::acceltrans::ClassKey() {
    return ClassName::acceltrans;
}

}; // namespace Attrib

#endif
