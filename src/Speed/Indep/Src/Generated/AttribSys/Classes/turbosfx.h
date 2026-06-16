#ifndef _attrib_gen_turbosfx_h
#define _attrib_gen_turbosfx_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct turbosfx : Instance {
    struct _LayoutStruct {
        Attrib::StringKey BankName;          // offset 0x0, size 0x10
        EA::Reflection::UInt32 Vol_Spool;    // offset 0x10, size 0x4
        EA::Reflection::Float ChargeTime;    // offset 0x14, size 0x4
        EA::Reflection::UInt32 Vol_Blowoff1; // offset 0x18, size 0x4
        EA::Reflection::UInt32 Vol_Blowoff2; // offset 0x1c, size 0x4
    };

    typedef Attrib::StringKey TypeOf_BankName;
    typedef EA::Reflection::Float TypeOf_ChargeTime;
    typedef EA::Reflection::Float TypeOf_Leak_Rate;
    typedef EA::Reflection::UInt32 TypeOf_Vol_Blowoff1;
    typedef EA::Reflection::UInt32 TypeOf_Vol_Blowoff2;
    typedef EA::Reflection::UInt32 TypeOf_Vol_Spool;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("turbosfx");
    turbosfx(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    turbosfx(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    turbosfx(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    turbosfx(const turbosfx &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    turbosfx(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~turbosfx() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x55624a85;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x55624a85, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const turbosfx &operator=(const turbosfx &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const turbosfx &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BankName(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xbf49a7d9);
    }
    bool BankName(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BankName, result);
    }
    const Attrib::StringKey &BankName() const {
        ATTRIB_CODEGEN_GETLAYOUT(BankName);
    }
    bool SET_BankName(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BankName, input);
    }
    bool ChargeTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4a985286);
    }
    bool ChargeTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ChargeTime, result);
    }
    const EA::Reflection::Float &ChargeTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(ChargeTime);
    }
    bool SET_ChargeTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ChargeTime, input);
    }
    bool Leak_Rate(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfdf3bc20);
    }
    bool Leak_Rate(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xfdf3bc20, result);
    }
    const EA::Reflection::Float &Leak_Rate() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xfdf3bc20);
    }
    bool SET_Leak_Rate(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xfdf3bc20, input);
    }
    bool Vol_Blowoff1(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0b08309d);
    }
    bool Vol_Blowoff1(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Vol_Blowoff1, result);
    }
    const EA::Reflection::UInt32 &Vol_Blowoff1() const {
        ATTRIB_CODEGEN_GETLAYOUT(Vol_Blowoff1);
    }
    bool SET_Vol_Blowoff1(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Vol_Blowoff1, input);
    }
    bool Vol_Blowoff2(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xe5f75d11);
    }
    bool Vol_Blowoff2(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Vol_Blowoff2, result);
    }
    const EA::Reflection::UInt32 &Vol_Blowoff2() const {
        ATTRIB_CODEGEN_GETLAYOUT(Vol_Blowoff2);
    }
    bool SET_Vol_Blowoff2(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Vol_Blowoff2, input);
    }
    bool Vol_Spool(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x67af0e6b);
    }
    bool Vol_Spool(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Vol_Spool, result);
    }
    const EA::Reflection::UInt32 &Vol_Spool() const {
        ATTRIB_CODEGEN_GETLAYOUT(Vol_Spool);
    }
    bool SET_Vol_Spool(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Vol_Spool, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    turbosfx &ConvertFromInstance(Instance &src) {}
    const turbosfx &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key turbosfx = 0x55624a85;

}; // namespace ClassName

namespace Hash {
namespace turbosfx {

static const Key BankName = 0xbf49a7d9;
static const Key ChargeTime = 0x4a985286;
static const Key Leak_Rate = 0xfdf3bc20;
static const Key Vol_Blowoff1 = 0x0b08309d;
static const Key Vol_Blowoff2 = 0xe5f75d11;
static const Key Vol_Spool = 0x67af0e6b;

}; // namespace turbosfx
}; // namespace Hash

inline Key Gen::turbosfx::ClassKey() {
    return ClassName::turbosfx;
}

}; // namespace Attrib

#endif
