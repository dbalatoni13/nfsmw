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
    bool NewBodyKit(TAttrib<EA::Reflection::Float> &result) const;
    bool NewBodyKit(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewBodyKit() const;
    bool SET_NewBodyKit(const EA::Reflection::Float &input);

    bool NewDecal(TAttrib<EA::Reflection::Float> &result) const;
    bool NewDecal(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewDecal() const;
    bool SET_NewDecal(const EA::Reflection::Float &input);

    bool NewHood(TAttrib<EA::Reflection::Float> &result) const;
    bool NewHood(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewHood() const;
    bool SET_NewHood(const EA::Reflection::Float &input);

    bool NewNumbers(TAttrib<EA::Reflection::Float> &result) const;
    bool NewNumbers(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewNumbers() const;
    bool SET_NewNumbers(const EA::Reflection::Float &input);

    bool NewPaint(TAttrib<EA::Reflection::Float> &result) const;
    bool NewPaint(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewPaint() const;
    bool SET_NewPaint(const EA::Reflection::Float &input);

    bool NewRim(TAttrib<EA::Reflection::Float> &result) const;
    bool NewRim(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewRim() const;
    bool SET_NewRim(const EA::Reflection::Float &input);

    bool NewRimPaint(TAttrib<EA::Reflection::Float> &result) const;
    bool NewRimPaint(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewRimPaint() const;
    bool SET_NewRimPaint(const EA::Reflection::Float &input);

    bool NewRoofScoop(TAttrib<EA::Reflection::Float> &result) const;
    bool NewRoofScoop(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewRoofScoop() const;
    bool SET_NewRoofScoop(const EA::Reflection::Float &input);

    bool NewSpoiler(TAttrib<EA::Reflection::Float> &result) const;
    bool NewSpoiler(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewSpoiler() const;
    bool SET_NewSpoiler(const EA::Reflection::Float &input);

    bool NewVinyl(TAttrib<EA::Reflection::Float> &result) const;
    bool NewVinyl(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewVinyl() const;
    bool SET_NewVinyl(const EA::Reflection::Float &input);

    bool NewWindowTint(TAttrib<EA::Reflection::Float> &result) const;
    bool NewWindowTint(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NewWindowTint() const;
    bool SET_NewWindowTint(const EA::Reflection::Float &input);

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

inline bool Gen::fecooling::NewBodyKit(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe52d0fb6);
}
inline const EA::Reflection::Float &Gen::fecooling::NewBodyKit() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe52d0fb6);
}
inline bool Gen::fecooling::NewBodyKit(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe52d0fb6, result);
}
inline bool Gen::fecooling::SET_NewBodyKit(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe52d0fb6, input);
}

inline bool Gen::fecooling::NewDecal(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1f71a77c);
}
inline const EA::Reflection::Float &Gen::fecooling::NewDecal() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x1f71a77c);
}
inline bool Gen::fecooling::NewDecal(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x1f71a77c, result);
}
inline bool Gen::fecooling::SET_NewDecal(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x1f71a77c, input);
}

inline bool Gen::fecooling::NewHood(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x678a0c91);
}
inline const EA::Reflection::Float &Gen::fecooling::NewHood() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x678a0c91);
}
inline bool Gen::fecooling::NewHood(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x678a0c91, result);
}
inline bool Gen::fecooling::SET_NewHood(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x678a0c91, input);
}

inline bool Gen::fecooling::NewNumbers(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa9ef0056);
}
inline const EA::Reflection::Float &Gen::fecooling::NewNumbers() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa9ef0056);
}
inline bool Gen::fecooling::NewNumbers(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa9ef0056, result);
}
inline bool Gen::fecooling::SET_NewNumbers(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa9ef0056, input);
}

inline bool Gen::fecooling::NewPaint(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x072ca287);
}
inline const EA::Reflection::Float &Gen::fecooling::NewPaint() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x072ca287);
}
inline bool Gen::fecooling::NewPaint(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x072ca287, result);
}
inline bool Gen::fecooling::SET_NewPaint(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x072ca287, input);
}

inline bool Gen::fecooling::NewRim(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4192719a);
}
inline const EA::Reflection::Float &Gen::fecooling::NewRim() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4192719a);
}
inline bool Gen::fecooling::NewRim(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4192719a, result);
}
inline bool Gen::fecooling::SET_NewRim(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4192719a, input);
}

inline bool Gen::fecooling::NewRimPaint(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa27de5ff);
}
inline const EA::Reflection::Float &Gen::fecooling::NewRimPaint() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa27de5ff);
}
inline bool Gen::fecooling::NewRimPaint(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa27de5ff, result);
}
inline bool Gen::fecooling::SET_NewRimPaint(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa27de5ff, input);
}

inline bool Gen::fecooling::NewRoofScoop(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf8be97ef);
}
inline const EA::Reflection::Float &Gen::fecooling::NewRoofScoop() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf8be97ef);
}
inline bool Gen::fecooling::NewRoofScoop(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf8be97ef, result);
}
inline bool Gen::fecooling::SET_NewRoofScoop(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf8be97ef, input);
}

inline bool Gen::fecooling::NewSpoiler(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe5d29656);
}
inline const EA::Reflection::Float &Gen::fecooling::NewSpoiler() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe5d29656);
}
inline bool Gen::fecooling::NewSpoiler(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe5d29656, result);
}
inline bool Gen::fecooling::SET_NewSpoiler(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe5d29656, input);
}

inline bool Gen::fecooling::NewVinyl(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x55b9418b);
}
inline const EA::Reflection::Float &Gen::fecooling::NewVinyl() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x55b9418b);
}
inline bool Gen::fecooling::NewVinyl(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x55b9418b, result);
}
inline bool Gen::fecooling::SET_NewVinyl(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x55b9418b, input);
}

inline bool Gen::fecooling::NewWindowTint(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x550807a7);
}
inline const EA::Reflection::Float &Gen::fecooling::NewWindowTint() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x550807a7);
}
inline bool Gen::fecooling::NewWindowTint(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x550807a7, result);
}
inline bool Gen::fecooling::SET_NewWindowTint(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x550807a7, input);
}

}; // namespace Attrib

#endif
