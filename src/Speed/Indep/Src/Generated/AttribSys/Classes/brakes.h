#ifndef _attrib_gen_brakes_h
#define _attrib_gen_brakes_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct brakes : Instance {
    struct _LayoutStruct {
        AxlePair BRAKE_LOCK;          // offset 0x0, size 0x8
        AxlePair BRAKES;              // offset 0x8, size 0x8
        EA::Reflection::Float EBRAKE; // offset 0x10, size 0x4
    };

    typedef AxlePair TypeOf_BRAKES;
    typedef AxlePair TypeOf_BRAKE_LOCK;
    typedef EA::Reflection::Float TypeOf_EBRAKE;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("brakes");
    brakes(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    brakes(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    brakes(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    brakes(const brakes &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    brakes(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~brakes() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x36350867;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x36350867, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const brakes &operator=(const brakes &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const brakes &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BRAKES(TAttrib<AxlePair> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x3a377301);
    }
    bool BRAKES(AxlePair &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BRAKES, result);
    }
    const AxlePair &BRAKES() const {
        ATTRIB_CODEGEN_GETLAYOUT(BRAKES);
    }
    bool SET_BRAKES(const AxlePair &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BRAKES, input);
    }
    bool BRAKE_LOCK(TAttrib<AxlePair> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xf5bec23d);
    }
    bool BRAKE_LOCK(AxlePair &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BRAKE_LOCK, result);
    }
    const AxlePair &BRAKE_LOCK() const {
        ATTRIB_CODEGEN_GETLAYOUT(BRAKE_LOCK);
    }
    bool SET_BRAKE_LOCK(const AxlePair &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BRAKE_LOCK, input);
    }
    bool EBRAKE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdc965a0c);
    }
    bool EBRAKE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EBRAKE, result);
    }
    const EA::Reflection::Float &EBRAKE() const {
        ATTRIB_CODEGEN_GETLAYOUT(EBRAKE);
    }
    bool SET_EBRAKE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EBRAKE, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    brakes &ConvertFromInstance(Instance &src) {}
    const brakes &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key brakes = 0x36350867;

}; // namespace ClassName

namespace Hash {
namespace brakes {

static const Key BRAKES = 0x3a377301;
static const Key BRAKE_LOCK = 0xf5bec23d;
static const Key EBRAKE = 0xdc965a0c;

}; // namespace brakes
}; // namespace Hash

inline Key Gen::brakes::ClassKey() {
    return ClassName::brakes;
}

}; // namespace Attrib

#endif
