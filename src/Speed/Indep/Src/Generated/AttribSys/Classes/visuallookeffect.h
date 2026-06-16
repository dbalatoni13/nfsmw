#ifndef _attrib_gen_visuallookeffect_h
#define _attrib_gen_visuallookeffect_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct visuallookeffect : Instance {
    struct _LayoutStruct {
        UMath::Matrix4 graph;              // offset 0x0, size 0x40
        EA::Reflection::Float _testvalue;  // offset 0x40, size 0x4
        EA::Reflection::Float length;      // offset 0x44, size 0x4
        EA::Reflection::Float magnitude;   // offset 0x48, size 0x4
        EA::Reflection::Float heattrigger; // offset 0x4c, size 0x4
    };

    typedef EA::Reflection::Float TypeOf__testvalue;
    typedef UMath::Matrix4 TypeOf_graph;
    typedef EA::Reflection::Float TypeOf_heattrigger;
    typedef EA::Reflection::Float TypeOf_length;
    typedef EA::Reflection::Float TypeOf_magnitude;
    typedef EA::Reflection::Float TypeOf_radialblur_scale;
    typedef EA::Reflection::Float TypeOf_radialblur_uvoffset;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("visuallookeffect");
    visuallookeffect(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallookeffect(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallookeffect(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallookeffect(const visuallookeffect &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    visuallookeffect(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~visuallookeffect() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x6ab2d241;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x6ab2d241, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const visuallookeffect &operator=(const visuallookeffect &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const visuallookeffect &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool _testvalue(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4ef07d5d);
    }
    bool _testvalue(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(_testvalue, result);
    }
    const EA::Reflection::Float &_testvalue() const {
        ATTRIB_CODEGEN_GETLAYOUT(_testvalue);
    }
    bool SET__testvalue(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(_testvalue, input);
    }
    bool graph(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x523266f4);
    }
    bool graph(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(graph, result);
    }
    const UMath::Matrix4 &graph() const {
        ATTRIB_CODEGEN_GETLAYOUT(graph);
    }
    bool SET_graph(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(graph, input);
    }
    bool heattrigger(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1c65e7d4);
    }
    bool heattrigger(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(heattrigger, result);
    }
    const EA::Reflection::Float &heattrigger() const {
        ATTRIB_CODEGEN_GETLAYOUT(heattrigger);
    }
    bool SET_heattrigger(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(heattrigger, input);
    }
    bool length(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf6b8b5ed);
    }
    bool length(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(length, result);
    }
    const EA::Reflection::Float &length() const {
        ATTRIB_CODEGEN_GETLAYOUT(length);
    }
    bool SET_length(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(length, input);
    }
    bool magnitude(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xceb0a9b0);
    }
    bool magnitude(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(magnitude, result);
    }
    const EA::Reflection::Float &magnitude() const {
        ATTRIB_CODEGEN_GETLAYOUT(magnitude);
    }
    bool SET_magnitude(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(magnitude, input);
    }
    bool radialblur_scale(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd0b003f4);
    }
    bool radialblur_scale(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xd0b003f4, result);
    }
    const EA::Reflection::Float &radialblur_scale() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xd0b003f4);
    }
    bool SET_radialblur_scale(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xd0b003f4, input);
    }
    bool radialblur_uvoffset(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8bc39288);
    }
    bool radialblur_uvoffset(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8bc39288, result);
    }
    const EA::Reflection::Float &radialblur_uvoffset() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8bc39288);
    }
    bool SET_radialblur_uvoffset(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8bc39288, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    visuallookeffect &ConvertFromInstance(Instance &src) {}
    const visuallookeffect &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key visuallookeffect = 0x6ab2d241;

}; // namespace ClassName

namespace Hash {
namespace visuallookeffect {

static const Key _testvalue = 0x4ef07d5d;
static const Key graph = 0x523266f4;
static const Key heattrigger = 0x1c65e7d4;
static const Key length = 0xf6b8b5ed;
static const Key magnitude = 0xceb0a9b0;
static const Key radialblur_scale = 0xd0b003f4;
static const Key radialblur_uvoffset = 0x8bc39288;

}; // namespace visuallookeffect
}; // namespace Hash

inline Key Gen::visuallookeffect::ClassKey() {
    return ClassName::visuallookeffect;
}

}; // namespace Attrib

#endif
