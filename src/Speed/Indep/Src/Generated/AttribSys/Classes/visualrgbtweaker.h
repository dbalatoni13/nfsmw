#ifndef _attrib_gen_visualrgbtweaker_h
#define _attrib_gen_visualrgbtweaker_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct visualrgbtweaker : Instance {
    typedef UMath::Matrix4 TypeOf_blue;
    typedef UMath::Matrix4 TypeOf_green;
    typedef UMath::Matrix4 TypeOf_red;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("visualrgbtweaker");
    visualrgbtweaker(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    visualrgbtweaker(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    visualrgbtweaker(const Instance &src) : Instance(src) {}
    visualrgbtweaker(const visualrgbtweaker &src) : Instance(src) {}
    visualrgbtweaker(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~visualrgbtweaker() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xaf1837ca;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xaf1837ca, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const visualrgbtweaker &operator=(const visualrgbtweaker &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const visualrgbtweaker &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool blue(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x1c641189);
    }
    bool blue(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x1c641189, result);
    }
    const UMath::Matrix4 &blue() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x1c641189);
    }
    bool SET_blue(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x1c641189, input);
    }
    bool green(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x3ffd271c);
    }
    bool green(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x3ffd271c, result);
    }
    const UMath::Matrix4 &green() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x3ffd271c);
    }
    bool SET_green(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x3ffd271c, input);
    }
    bool red(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x5cd90211);
    }
    bool red(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x5cd90211, result);
    }
    const UMath::Matrix4 &red() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x5cd90211);
    }
    bool SET_red(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x5cd90211, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    visualrgbtweaker &ConvertFromInstance(Instance &src) {}
    const visualrgbtweaker &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key visualrgbtweaker = 0xaf1837ca;

}; // namespace ClassName

namespace Hash {
namespace visualrgbtweaker {

static const Key blue = 0x1c641189;
static const Key green = 0x3ffd271c;
static const Key red = 0x5cd90211;

}; // namespace visualrgbtweaker
}; // namespace Hash

inline Key Gen::visualrgbtweaker::ClassKey() {
    return ClassName::visualrgbtweaker;
}

}; // namespace Attrib

#endif
