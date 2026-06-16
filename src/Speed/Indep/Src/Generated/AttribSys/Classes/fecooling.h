#ifndef _attrib_gen_fecooling_h
#define _attrib_gen_fecooling_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct fecooling : Instance {
    typedef EA::Reflection::Float TypeOf_NewBodyKit;
    typedef EA::Reflection::Float TypeOf_NewDecal;
    typedef EA::Reflection::Float TypeOf_NewHood;
    typedef EA::Reflection::Float TypeOf_NewNumbers;
    typedef EA::Reflection::Float TypeOf_NewPaint;
    typedef EA::Reflection::Float TypeOf_NewRim;
    typedef EA::Reflection::Float TypeOf_NewRimPaint;
    typedef EA::Reflection::Float TypeOf_NewRoofScoop;
    typedef EA::Reflection::Float TypeOf_NewSpoiler;
    typedef EA::Reflection::Float TypeOf_NewVinyl;
    typedef EA::Reflection::Float TypeOf_NewWindowTint;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("fecooling");
    fecooling(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    fecooling(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    fecooling(const Instance &src) : Instance(src) {}
    fecooling(const fecooling &src) : Instance(src) {}
    fecooling(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~fecooling() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x5d417978;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x5d417978, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const fecooling &operator=(const fecooling &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const fecooling &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool NewBodyKit(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe52d0fb6);
    }
    bool NewBodyKit(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe52d0fb6, result);
    }
    const EA::Reflection::Float &NewBodyKit() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe52d0fb6);
    }
    bool SET_NewBodyKit(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe52d0fb6, input);
    }
    bool NewDecal(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1f71a77c);
    }
    bool NewDecal(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x1f71a77c, result);
    }
    const EA::Reflection::Float &NewDecal() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x1f71a77c);
    }
    bool SET_NewDecal(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x1f71a77c, input);
    }
    bool NewHood(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x678a0c91);
    }
    bool NewHood(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x678a0c91, result);
    }
    const EA::Reflection::Float &NewHood() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x678a0c91);
    }
    bool SET_NewHood(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x678a0c91, input);
    }
    bool NewNumbers(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa9ef0056);
    }
    bool NewNumbers(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa9ef0056, result);
    }
    const EA::Reflection::Float &NewNumbers() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa9ef0056);
    }
    bool SET_NewNumbers(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa9ef0056, input);
    }
    bool NewPaint(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x072ca287);
    }
    bool NewPaint(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x072ca287, result);
    }
    const EA::Reflection::Float &NewPaint() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x072ca287);
    }
    bool SET_NewPaint(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x072ca287, input);
    }
    bool NewRim(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4192719a);
    }
    bool NewRim(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4192719a, result);
    }
    const EA::Reflection::Float &NewRim() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4192719a);
    }
    bool SET_NewRim(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4192719a, input);
    }
    bool NewRimPaint(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa27de5ff);
    }
    bool NewRimPaint(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa27de5ff, result);
    }
    const EA::Reflection::Float &NewRimPaint() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa27de5ff);
    }
    bool SET_NewRimPaint(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa27de5ff, input);
    }
    bool NewRoofScoop(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf8be97ef);
    }
    bool NewRoofScoop(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf8be97ef, result);
    }
    const EA::Reflection::Float &NewRoofScoop() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf8be97ef);
    }
    bool SET_NewRoofScoop(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf8be97ef, input);
    }
    bool NewSpoiler(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe5d29656);
    }
    bool NewSpoiler(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe5d29656, result);
    }
    const EA::Reflection::Float &NewSpoiler() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe5d29656);
    }
    bool SET_NewSpoiler(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe5d29656, input);
    }
    bool NewVinyl(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x55b9418b);
    }
    bool NewVinyl(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x55b9418b, result);
    }
    const EA::Reflection::Float &NewVinyl() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x55b9418b);
    }
    bool SET_NewVinyl(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x55b9418b, input);
    }
    bool NewWindowTint(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x550807a7);
    }
    bool NewWindowTint(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x550807a7, result);
    }
    const EA::Reflection::Float &NewWindowTint() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x550807a7);
    }
    bool SET_NewWindowTint(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x550807a7, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    fecooling &ConvertFromInstance(Instance &src) {}
    const fecooling &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key fecooling = 0x5d417978;

}; // namespace ClassName

namespace Hash {
namespace fecooling {

static const Key NewBodyKit = 0xe52d0fb6;
static const Key NewDecal = 0x1f71a77c;
static const Key NewHood = 0x678a0c91;
static const Key NewNumbers = 0xa9ef0056;
static const Key NewPaint = 0x072ca287;
static const Key NewRim = 0x4192719a;
static const Key NewRimPaint = 0xa27de5ff;
static const Key NewRoofScoop = 0xf8be97ef;
static const Key NewSpoiler = 0xe5d29656;
static const Key NewVinyl = 0x55b9418b;
static const Key NewWindowTint = 0x550807a7;

}; // namespace fecooling
}; // namespace Hash

inline Key Gen::fecooling::ClassKey() {
    return ClassName::fecooling;
}

}; // namespace Attrib

#endif
